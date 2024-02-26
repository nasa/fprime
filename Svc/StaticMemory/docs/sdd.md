\page SvcStaticMemoryComponent Svc::StaticMemory Component
# Svc::StaticMemory Component

The StaticMemory component is a drop in replacement for Svc::BufferManager that allocates memory out of a static pool of
memory. This allows for allocations of known sizes and usage patterns to avoid extra dynamic memory complexity at the
expense of extra allocations.

**Note:** the static memory component operates under the following caveats.

1. Each port index has dedicate memory and should be assigned to one allocator
2. Memory sizes are fixed size
3. Memory must be deallocated before being allocated again.

**See:** Svc::BufferManager for usage examples.

## Configuration

StaticMemory number of allocations and ports are configured using `AcConstants.fpp` as shown below. This sets the number
of allocate and deallocate port indices as well as the memory segments.

```
@ Number of static memory allocations
constant StaticMemoryAllocations = 4
```

The size of each memory segment is set in `StaticMemoryConfig.hpp` as shown below.

```c++
STATIC_MEMORY_ALLOCATION_SIZE = 2048
```

This means the total memory used is `StaticMemoryAllocations` * `STATIC_MEMORY_ALLOCATION_SIZE` bytes. 


## Requirements

| Name | Description | Validation |
|---|---|---|
| STAMEM-001 | The static memory shall allocate a fixed block per port index | unit test |
| STAMEM-002 | The static memory shall define allocate and deallocate ports shadowing Svc::BufferManager | unit test |


## Change Log

| Date | Description |
|---|---|
| 2020-12-21 | Initial Draft |
