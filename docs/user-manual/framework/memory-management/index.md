# Memory Management

F´ provides two complementary patterns for managing memory in flight software systems, each designed for different phases of the application lifecycle and use cases. This document provides an overview of these patterns and links to the appropriate in-depth documentation.

## Overview

Flight software coding standards typically forbid dynamic memory allocation during runtime operation for safety and reliability reasons. However, memory management is still necessary during both system initialization and runtime operations. F´ addresses these needs through two distinct patterns:

| Pattern | Phase | Purpose | Key Components |
|---------|-------|---------|----------------|
| [Memory Allocation](#memory-allocation-pattern) | Initialization | Allocate memory blocks during system startup | `Fw::MemAllocator` (interface), `Fw::MallocAllocator` (implementation) |
| [Buffer Pool](#buffer-pool-pattern) | Runtime | Safely manage pre-allocated buffer pools during operation | `Svc::BufferManager` |

## Memory Allocation Pattern

**Used during:** System initialization  
**Document:** [Memory Allocation with Fw::MemAllocator](./memory-allocation.md)

The Memory Allocation pattern uses the `Fw::MemAllocator` interface to allocate memory blocks during system initialization. This is appropriate in the following scenarios:

- Memory requirements are too large for stack allocation
- Components need internal memory of configurable sizes determined at runtime
- Multiple component instances require different memory configurations

**Key Features:**
- Provides abstraction layer over project-specified allocation mechanisms
- Memory is allocated at startup, and freed later on (usually at shutdown)
- Includes `Fw::MallocAllocator` implementation which delegates to `malloc()`/`free()`
- Optional `Fw::MemAllocatorRegistry` for managing multiple allocator types

**Example Use Cases:**
- Component internal buffers, such as:
    - the backing memory for `Svc::BufferManager`
    - the accumulation buffer for `Svc::FrameAccumulator`

**Typical Workflow:**
1. Component defines setup method accepting `Fw::MemAllocator&`
2. Memory allocated during `configureTopology()`
3. Component uses memory throughout its lifetime
4. Memory deallocated during `teardownTopology()`

## Buffer Pool Pattern

**Used during:** Runtime operation  
**Document:** [Buffer Pool with Svc.BufferManager](../memory-management/buffer-pool.md)

The Buffer Pool pattern provides safe runtime buffer management through pre-allocated buffer pools. This pattern allows components to request and return buffers during normal operation without violating flight software coding standards. This is appropriate when:

- Components need temporary working memory during operation
- Buffer size vary based on runtime conditions
- Memory must be shared efficiently across the system

**Key Features:**
- Buffer pools allocated at initialization, then reused at runtime (no runtime heap allocation)
- Standard pattern for passing buffers between components, possibly asynchronously
- Port-based allocation/deallocation interface
- Support for multiple buffer sizes through configurable bins

**Example Use Cases:**
- Communication packet buffers
- File transfer data blocks
- Framing and deframing operations
- Data processing pipelines

**Typical Workflow:**
1. BufferManager initialized with memory pools during startup (tip: this leverages the Memory Allocation pattern!)
2. Component requests buffer via `allocate` port during operation
3. Component uses buffer (possibly passing to other components)
4. Buffer returned to BufferManager via `deallocate` port

