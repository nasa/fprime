
# Operating System Abstraction Layer (OSAL)

## References

- [OSAL SDD](https://github.com/nasa/fprime/blob/devel/Os/docs/sdd.md)
- [Generic Services SDD](https://github.com/nasa/fprime/blob/devel/Os/Generic/docs/sdd.md)
- [How-To: Implement an OSAL](https://github.com/nasa/fprime/blob/devel/docs/how-to/implement-osal.md)

## Overview

The Operating System Abstraction Layer (OSAL) provides a portable interface to operating system services, enabling F Prime applications to run on multiple operating systems without source code modifications. All platform-specific functionality (threads, files, synchronization, system resources) is accessed through the OSAL rather than through direct OS calls. The OSAL ships with a POSIX backend for Linux/macOS and a stub backend for platforms without full OS support. Additional backends (Zephyr, VxWorks, FreeRTOS) are available through community platform support packages.

### Concurrency

The OSAL provides the following concurrency primitives:

- **Tasks** — Thread creation, joining, and lifecycle management. Tasks support start/stop callbacks and can be delayed for a specified duration. Tasks are the execution context for active components.
- **Mutexes** — Mutual-exclusion locks for protecting shared data. An RAII scope lock helper is provided for automatic acquire/release within a scope.
- **Condition Variables** — Signaling mechanism paired with mutexes for producer/consumer patterns and other synchronization needs.
- **Queues** — Inter-task message passing with configurable depth, optional priority support, and blocking modes. Queues are the underlying mechanism for active component message dispatch.

### File System

The OSAL provides file and directory access:

- **Files** — Open, read, write, seek, flush, and CRC computation on individual files.
- **Directories** — Open, iterate, and create directory entries.
- **File System Operations** — Higher-level operations including remove, move/rename, copy, stat, free space queries, and working directory management.

### System Resources

The OSAL provides access to system-level information:

- **Raw Time** — Access to the system clock for measuring time intervals.
- **CPU** — CPU count and per-CPU usage statistics.
- **Memory** — System memory usage statistics.

### Console

The console service provides a mechanism for writing messages to the system console output, used primarily for text event logging and diagnostic output.

### Generic Services

In addition to the platform-specific core services, the OSAL provides generic services that are implemented independently of the underlying OS:

- **Priority Queue** — A heap-based priority queue implementation used by the framework for priority-aware message dispatch.

### Implementation Architecture

The OSAL uses a delegate pattern where each service has three layers:

1. **Interface** — A pure-virtual base class defining the service contract
2. **Wrapper** — A final concrete class that application code interacts with, forwarding calls to the delegate
3. **Implementation** — A platform-specific class that implements the interface

Services are accessed either as singletons (for global operations like file system queries and CPU statistics) or as handles (for stateful objects like individual files, mutexes, and tasks). The OSAL is initialized once at system startup, though singletons will self-initialize on first use if explicit initialization is omitted.

### Error Handling

All OSAL operations return typed status enumerations specific to each service. Platform backends translate native error codes into these status values so that application code never needs to interpret platform-specific errors.

### Available Backends

- **Posix** — Full implementation using POSIX APIs (pthreads, file I/O, clock_gettime). Default for Linux and macOS.
- **Stub** — No-op implementations that return NOT_SUPPORTED. Used as a starting point for platforms where some services are not yet implemented.
- **Community** — Additional backends for Zephyr RTOS, VxWorks, and FreeRTOS are maintained in the fprime-community GitHub organization.
