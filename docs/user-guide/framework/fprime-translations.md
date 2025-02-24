# F´ Translation Guide: Software Engineering to F Prime Nomenclature

This guide provides a mapping between common software engineering concepts and their equivalent implementations in the F´ framework. It serves as a reference for developers transitioning to F´ development.

## Data Structures & Containers

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Queue | Os::Queue | Thread-safe queue implementation for RTOS |
| Array | Fw::Array<T> | Fixed-size array with bounds checking |
| String | Fw::String | Safe string implementation with size limits |
| Buffer | Fw::Buffer | Memory buffer with size tracking |
| Vector | Fw::Array<T> | F Prime uses fixed arrays instead of dynamic vectors |
| Map/Dictionary | Table + for loop | Implement lookup tables with arrays and linear search |
| Set | Bit array/mask | Use bit fields for efficient set operations |

## Communication & Synchronization

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Function Call | Synchronous Port | Direct component-to-component calls |
| Callback | Port Registration | Components register ports for callbacks |
| Event Loop | Active Component | Components with their own execution thread |
| Message Queue | Async Port | Asynchronous component communication |
| Publish/Subscribe | Event Port | Multi-cast port connections |
| Mutex | Os::Mutex | Thread synchronization primitive |
| Semaphore | Os::Semaphore | Resource management between threads |
| Thread | Os::Task | RTOS task abstraction |
| Timer | Rate Group | Time-based execution scheduling |

## Memory Management

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Heap Allocation | Fw::MemAllocator | Managed heap allocation |
| Stack Allocation | Local variables | Standard stack allocation |
| Memory Pool | Svc::BufferManager | Fixed-size buffer management |
| Smart Pointer | Object ownership | Components own their objects |
| Reference Counting | Port references | Managed by component framework |
| Memory Mapping | Drv::BlockDriver | Hardware memory management |

## System Architecture

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Module | Component | Basic unit of functionality |
| Interface | Port | Component communication interface |
| Service | Service Component | Components providing system services |
| Driver | Driver Component | Hardware abstraction components |
| Configuration | Parameters | Component configuration management |
| Plugin | Component Library | Reusable component implementations |
| State Machine | Component States | Internal component state management |

## Error Handling

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Exception | Assert + Event | Assertion and event logging |
| Error Code | Status Type | Enumerated status returns |
| Try/Catch | Command Handler | Command execution with status return |
| Logging | Events | System event logging framework |
| Debug Print | Fw::Logger | Debug output facility |

## Testing

| Software Concept | F Prime Equivalent | Notes |
|-----------------|-------------------|--------|
| Unit Test | GTest Component | Google Test integration |
| Mock Object | Test Component | Component test implementations |
| Test Fixture | Test Harness | Component test setup |
| Integration Test | Integration Test | System-level test scenarios |
| Test Coverage | gcov + STest | Code coverage analysis |

## Common Design Patterns

| Pattern | F Prime Implementation | Notes |
|---------|----------------------|--------|
| Observer | Event Ports | Components subscribe to events |
| Command | Command Ports | Component command handling |
| Factory | Component Constructor | Component instantiation |
| Singleton | System Components | Single-instance components |
| Facade | Port Arrays | Multiple interface aggregation |
| Strategy | Component Implementation | Swappable component behavior |
| Proxy | Port Connection | Inter-component communication |
