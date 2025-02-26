# F Prime Translation Guide: Software Engineering Terminology to F Prime Nomenclature

This guide provides a mapping between common software engineering concepts and their equivalent implementations in the F´ framework. It serves as a reference for developers new to F´ development.

## Data Structures & Containers

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| String | Fw::String | Safe string implementation with size limits |
| Buffer | [Fw::Buffer](../../Fw/Buffer/docs/sdd.md) | Memory buffer with size tracking |
| Queue | Os::Queue | Thread-safe queue implementation |

> [!NOTE]
> `Os::Queue` is rarely used directly but rather is used via `async` port calls.

## Communication & Synchronization

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Function Call | Synchronous Port | Direct component-to-component calls |
| Message Queue | Async Port | Asynchronous component communication supported by a queue |
| Event Loop | Active Component | Components with their own execution thread |
| Mutex | Os::Mutex | Thread synchronization primitive |
| Thread | Os::Task | OS task abstraction |


> [!NOTE]
> `Os::Task` is rarely used directly but rather is contained  within `active` components.

## Memory Management

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Stack Allocation | Local variables | Standard stack allocation |
| Heap Allocation | Fw::MemAllocator | Managed heap allocation |
| Memory Pooling | [Svc::BufferManager](../../Svc/BufferManager/docs/sdd.md) | Fixed-size buffer management |
| Smart Pointer | [Fw::Buffer](../../Fw/Buffer/docs/sdd.md) | Buffer containing pointer, size, and context |

## System Architecture

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Module | Component | Basic unit of functionality |
| Interface | Port(s) | Component communication interface |
| System Service | Service Component | Components providing system services |
| Driver | Driver Component | Hardware abstraction components |
| Runtime Configuration | Parameters | Component configuration management via ground-commanded parameters |

## Error Handling

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Exception | Assert + Event | Assertion and event logging |
| Error Code | Status Type | Enumerated status returns |
| Logging | Events | System event logging framework |
| Debug Print | Fw::Logger | Debug output facility |

