# Dynamic Memory Allocation Using `Fw::Buffer`

In embedded systems, dynamic memory allocation (a.k.a heap allocation) is typically avoided to reduce the steady-state
variability in a running system. Avoiding dynamic memory allocation also avoids the problem of what to do in the case of
a failed allocation. However, sometimes dynamic allocation provides for a simpler or more efficient solution.

Safe dynamic allocation is available using the buffer manager pattern in F´. In short, this pattern allows components to
dynamically allocate memory through a port call to a component designed to manage memory for the system. There are three steps in this process:

1. Call allocation port receiving an`Fw::Buffer`
2. Use allocated in the `Fw::Buffer`
3. Call deallocation port providing the `Fw::Buffer`

## Component Setup and Buffer Usage

This section will describe the work done within a component to allocate, use, and deallocate buffers. 

## Allocating and Deallocating `Fw::Buffer`s

Allocation and deallocation are done through port calls to a buffer managing component. The component needing dynamic
memory allocation should include two output ports:

1. Output port of type `Fw::BufferGet` to request a buffer
2. Output port of type `Fw::BufferSend` to deallocate the requested buffer.

In the case that allocation fails, the `Fw::Buffer` return from the `Fw::BufferGet` port will have a size of zero.
Developers must check that the size is not smaller than requested before proceeding to use the memory.

In the example below, the ports are called `allocate` and `deallocate`. First the port definitions are presented
followed by the usage in C++.

**Example Component Definition**
```fpp
    @ Allocation port for a buffer
    output port allocate: Fw.BufferGet

    @ Deallocation port for buffers
    output port deallocate: Fw.BufferSend
    
    @ Allocation failed event
    event MemoryAllocationFailed() severity warning low id 0 format "Failed to allocate memory"
```

**Example Component Allocation and Deallocation**
```c++
    ...
    const U32 needed_size = 1024;
    Fw::Buffer my_buffer = this->allocate_out(0, needed_size);
    
    if (my_buffer.getSize() < needed_size) {
        this->deallocate_out(0, my_buffer);
        this->log_WARNING_LO_MemoryAllocationFailed();
    } else {
        ...
        ...
        this->deallocate_out(0, my_buffer);
    }
```

### Working With F´ Buffers

`Fw::Buffer` objects function as a wrapper for generic memory regions. They consist of a pointer to memory and the size of the memory region pointed to by the pointer. An easy way to work with an `Fw::Buffer` is to use the serialization
representation of the buffer. This allows users to serialize and deserialize from the buffer's data using methods. 

To use this method, get a representation using the `Fw::Buffer.getSerializeRepr()` and then call `.serialize()` or
`.deserialize()` on the returned object.

**Example Using Serialization and Deserialization Methods**

```c++
U32 my_value = 123;
Fw::Buffer my_buffer = ...;
my_buffer.getSerializeRepr().serialize(mv_value);

U32 my_value_again = 0;
my_buffer.getSerializeRepr().deserialize(mv_value_again);
```
**Note:** To use this method types must inherit from `Fw::Serializable` or be basic types.

Users can access the `Fw::Buffer`'s data directly using `Fw::Buffer.getData()`, which will return a `U8*` pointer to the
buffer's memory. Care should be taken as this is a raw pointer and thus buffer overruns are possible.

**Example Using Raw Data**

```c++
Fw::Buffer my_buffer = ...;
U8* const data = my_buffer.getData();

FW_ASSERT(my_buffer.getSize() >= 4); // Prevent overrun on next line
data[3] = 1;
```

Full `Fw::Buffer` documentation is available [here](../api/c++/html/class_fw_1_1_buffer.html).


## Topology Consideration

There are several components designed to allow for memory allocation and they differ in terms of complexity and use
cases. They both support the `Fw::BufferGet` and `Fw::BufferSend` port interface for allocation and deallocation and
thus can be used interchangeably subject to the descriptions in this section.

Each section will describe any special setup needed in the topology and how to hook up the manager's ports.

### Svc.StaticMemory

Svc.StaticMemory uses a stack-based pool of memory to support allocation. This pool is composed of fixed-size regions
each of which is tied to a specific client. Each client's allocation **must** be deallocated before a subsequent request by
the same client. Since allocation and deallocation ports are port arrays, each client's allocation and deallocation
ports must be hooked up in parallel.

This component is designed for simplicity of implementation. System memory usage is **always** the number of clients
multiplied by the size of the memory regions. This memory is allocated as a large array on the stack. Valid memory 
allocations will always be returned or a software error will be tripped.

Svc.StaticMemory is described in more detail [here](../api/c++/html/svc_static_memory.html).

**When To Use Svc.StaticMemory**

Use Svc.StaticMemory in situations where memory must always be available and sharing or efficient use of memory is a concern. Svc.StaticMemory is typically not suitable for situations where asynchronous memory handling occurs between 
allocation and deallocation.

***Usage Requirements***

Since this component is designed to be simple, its usage has several caveats. These caveats are, for the most part, enforced by assertions, and thus failure to abide by them will result in software termination.

1. Allocations will always return with the size of `Svc::StaticMemoryConfig::STATIC_MEMORY_ALLOCATION_SIZE`
2. Allocations above `Svc::StaticMemoryConfig::STATIC_MEMORY_ALLOCATION_SIZE` is considered an error
3. It is an error for a client to allocate memory before deallocating previously allocated memory

These rules imply that memory allocated from Svc.StaticMemory should never be sent through an asynchronous port as this will risk violating item 3.


**Connections**

All connections to Svc.StaticMemory are done using parallel port indices per-client. This is shown in the Topology
snippet shown below:

```fpp
      client1.allocate -> my_static_memory.bufferAllocate[0]
      client1.deallocate -> my_static_memory.bufferDeallocate[0]
      
      client2.allocate -> my_static_memory.bufferAllocate[1]
      client2.deallocate -> my_static_memory.bufferDeallocate[1]
```

Svc.StaticMemory does not use any other ports. Please review the configuration to ensure that sufficient regions are
available for the number of clients used.

***Configuration and Setup***

Allocation region size is configured in the `StaticMemoryConfig.hpp` header using the `STATIC_MEMORY_ALLOCATION_SIZE`
value and maximum client number is configured in `AcConstants.fpp` using the `StaticMemoryAllocations` value. No other
configuration or setup is necessary.

### Svc.BufferManager

Svc.BufferManager uses multiple bins of memory with fixed-size sub-allocations within a bin. It has a single allocate
and deallocate port that may take any size allocation request. Svc.BufferManager searches all bins with sub-allocation
size larger than the request for an available buffer, which it then marks as used and returns.

There is no restriction on the ordering of calls for allocation and deallocation. Clients may have multiple outstanding allocations and thus asynchronous usage of these allocations is supported.

Svc.BufferManager is described in more detail [here](../api/c++/html/svc_buffer_manager_component.html).

**When To Use Svc.BufferManager**

Svc.BufferManager must be used when asynchronous handling of memory is needed or sharing of memory is desired. It can
be used generically but comes at the cost of complexity of implementation and setup.

**Usage Requirements**

Allocating more memory than available will result in buffers with size 0 being returned and is not an error. However,
buffers must be allocated and returned using the same instance of Svc.BufferManager. 

Buffer manager will assert under the following conditions:
1. A returned buffer has the incorrect manager ID (returned to the wrong instance).
2. A returned buffer has an incorrect buffer ID (invalid buffer returned).
3. A returned buffer is returned with a correct buffer ID but hasn't already been allocated.
4. A returned buffer has an indicated size larger than originally allocated.
5. A returned buffer has a pointer outside the region originally allocated.

**Connections**

All connections to Svc.BufferManager can be done using the single pair of allocate and deallocate ports. This is shown
in the following snippet of a topology:

```fpp
      client1.allocate -> my_buffer_manager.bufferGetCallee
      client1.deallocate -> my_buffer_manager.bufferSendIn
      
      client2.allocate -> my_buffer_manager.bufferGetCallee
      client2.deallocate -> my_buffer_manager.bufferSendIn
```

The buffer manager should also be hooked up to a rate group used to downlink telemetry and it requires standard
telemetry, events, and time connections.

**Configuration and Setup**

The number of sub allocations is configured in the `BufferManagerComponentImplCfg.hpp` header using the 
`BUFFERMGR_MAX_NUM_BINS` value.

When using Svc.BufferManager the `Svc::BufferManagerComponentImpl.setup()` method must be called supplying a U16 manager
ID, a buffer id, an implementation of [Fw::MemAllocator](../api/c++/html/class_fw_1_1_mem_allocator.html) used to
allocate memory for the sub-allocations, and a
[Svc::BufferManagerComponentImpl::BufferBins](../api/c++/html/struct_svc_1_1_buffer_manager_component_impl_1_1_buffer_bin.html)
struct configuring the sub allocations.

The Svc::BufferManagerComponentImpl::BufferBins is a table specifying N buffers of M size per bin. Up to MAX_NUM_BINS
bins can be specified. The table is copied when setup() is called, so it does not need to be retained after the call.

The rules for specifying bins:
1. For each bin (BufferBins.bins[n]), specify the size of the buffers (bufferSize) in the bin and how many buffers for
   that bin (numBuffers).
2. The bins must be ordered based on an increasing bufferSize to allow BufferManager to search for available buffers.
   When receiving a request for a buffer, the component will search for the first buffer from the bins that are equal to
   or greater than the requested size, starting at the beginning of the table.
3. Any unused bins should have numBuffers set to 0.
4. A single bin can be specified if a single size is needed. 
   
**Note:** a pointer to the Fw::MemAllocator used in setup() is stored for later memory cleanup. The instance of the
allocator must persist beyond calling the cleanup() function or the destructor of BufferManager if cleanup() is not
called. If a project-specific manual memory allocator is not needed, Fw::MallocAllocator can be used to supply heap
allocated memory.

**Example Setup of Svc.BufferManager**
```c++
Fw::MallocAllocator allocator;
Svc::BufferManagerComponentImpl my_buffer_manager;

...
{
    Svc::BufferManager::BufferBins my_bins;
    memset(&my_bins, 0, sizeof(my_bins)); // All non-specified bins are zero

    my_bins.bins[0].bufferSize = 1024; // Buffers in bin 0 are of size 1024
    my_bins.bins[0].numBuffers = 2; // Two buffers of size 1024 are available in bin 0
    my_bins.bins[1].bufferSize = 10240; // Buffers in bin 1 are of size 10240
    my_bins.bins[1].numBuffers = 1; // One buffers of size 10240 are available in bin 1
    
    my_buffer_manager.setup(123, 0, allocator, my_bins);
}

...
{
    my_buffer_manager.cleanup();
}
```

**Rules of Thumb for Bin Sizes**

Buffers bins should be tailored based on expected usage. If many small requests are expected, then set up a large number of smaller bins. If larger allocations are expected, set bins of that size.

The above trivial example allows for a few small allocations and one large allocation. In this case, there is a risk that the large allocation is used for the small allocation use case and thus care should be taken to ensure that the smaller use
cases have a sufficient number of buffers to prevent stealing of larger allocations.

## Separation of `Fw::Buffer` Allocation and Deallocation

There is no requirement that the allocating component and the deallocating component are the same. Thus components may
be chained together for multiple processing steps before deallocation. There are two requirements:

1. `Fw::Buffers` must eventually be returned to the instance that allocated them
2. `Svc.StaticMemory` cannot be used in chains involving asynchronous calls

Inter-component connections typically use the same `Fw.BufferSend` port to pass the buffer along the chain. A sample
chain is shown here using Svc.BufferManager as the allocation source.

```fpp
      comp1.allocate -> my_buffer_manager.bufferGetCallee
      comp1.sendToProcess -> comp2.process
      comp2.sendToProcessMore -> comp3.processMore
      comp3.deallocate -> my_buffer_manager.bufferSendIn
```