# Memory Management

F´ provides two complementary patterns for managing memory in flight software systems, each designed for different phases of the application lifecycle and use cases. This document provides an overview of these patterns and links to the appropriate in-depth documentation.

## Overview

Flight software coding standards typically forbid dynamic memory allocation during runtime operation for safety and reliability reasons. However, memory management is still necessary during both system initialization and runtime operations. F´ addresses these needs through two distinct patterns:

| Pattern | Phase | Purpose | Key Components |
|---------|-------|---------|----------------|
| [Memory Allocation](#memory-allocation-pattern) | Initialization | Allocate memory blocks (outside of stack) during system initialization for use during runtime | `Fw::MemAllocator` (interface), `Fw::MallocAllocator` (implementation), `Fw::MemAllocatorRegistry` |
| [Buffer Pool](#buffer-pool-pattern) | Runtime | Allocate buffers during runtime from a pre-allocated buffer pool | `Svc::BufferManager` |

## Memory Allocation Pattern

**Used during:** System initialization  
**Document:** [Memory Allocation with Fw::MemAllocator](./memory-allocation.md)

The Memory Allocation pattern uses the `Fw::MemAllocator` interface to allocate memory blocks during system initialization. This is appropriate in the following scenarios:

- Memory requirements are too large for stack allocation
- Components need internal memory of configurable sizes determined at runtime during system initialization
- Multiple component instances require different memory configurations

**Key Features:**
- Provides abstraction layer over project-specified allocation mechanisms
- Includes `Fw::MallocAllocator` implementation which delegates to `malloc()`/`free()`
- Optional `Fw::MemAllocatorRegistry` for managing multiple allocator types

**Example Use Cases:**
- Component internal buffers, such as:
    - The backing memory for `Svc::BufferManager`
    - The accumulation buffer for `Svc::FrameAccumulator`

**Typical Workflow:**
1. Component defines setup method accepting `Fw::MemAllocator&`
2. Memory allocated during initialization (typically in `configureTopology()` via a component `configure()` method)
3. Component uses memory throughout its lifetime
4. Memory deallocated during shutdown (typically in `teardownTopology()` via a component `teardown()` method)

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
3. Component checks buffer validity and handles potential allocation failure
4. Component uses buffer (possibly passing to other components)
5. Buffer returned to BufferManager via `deallocate` port

