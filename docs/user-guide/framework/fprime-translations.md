# F´ Translation Guide: Software Engineering to F Prime Nomenclature

This guide provides a mapping between common software engineering concepts and their equivalent implementations in the F´ framework. It serves as a reference for developers new to F´ development.

## Data Structures & Containers

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Queue | Os::Queue | Thread-safe queue implementation |
| String | Fw::String | Safe string implementation with size limits |
| Buffer | Fw::Buffer | Memory buffer with size tracking |

## Communication & Synchronization

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Function Call | Synchronous Port | Direct component-to-component calls |
| Callback | Port Registration | Components register ports for callbacks |
| Event Loop | Active Component | Components with their own execution thread |
| Message Queue | Async Port | Asynchronous component communication supported by a queue |
| Mutex | Os::Mutex | Thread synchronization primitive |
| Thread | Os::Task | OS task abstraction |

## Memory Management

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Stack Allocation | Local variables | Standard stack allocation |
| Heap Allocation | Fw::MemAllocator | Managed heap allocation |
| Memory Pool | Svc::BufferManager | Fixed-size buffer management |
| Smart Pointer | Fw::Buffer | Buffer containing pointer, size, and context |

## System Architecture

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Module | Component | Basic unit of functionality |
| Interface | Port | Component communication interface |
| Service | Service Component | Components providing system services |
| Driver | Driver Component | Hardware abstraction components |
| Configuration | Parameters | Component configuration management via ground-commanded parameters |

## Error Handling

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Exception | Assert + Event | Assertion and event logging |
| Error Code | Status Type | Enumerated status returns |
| Logging | Events | System event logging framework |
| Debug Print | Fw::Logger | Debug output facility |

