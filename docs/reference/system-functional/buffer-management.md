
# Buffer Management Functionality

## References

- [F Prime BufferManager SDD](https://github.com/nasa/fprime/blob/devel/Svc/BufferManager/docs/sdd.md)
- [F Prime BufferAccumulator SDD](https://github.com/nasa/fprime/blob/devel/Svc/BufferAccumulator/docs/sdd.md)
- [F Prime BufferRepeater SDD](https://github.com/nasa/fprime/blob/devel/Svc/BufferRepeater/docs/sdd.md)
- [F Prime BufferLogger SDD](https://github.com/nasa/fprime/blob/devel/Svc/BufferLogger/docs/sdd.md)
- [F Prime StaticMemory SDD](https://github.com/nasa/fprime/blob/devel/Svc/StaticMemory/docs/sdd.md)
- [Fw::Buffer SDD](https://github.com/nasa/fprime/blob/devel/Fw/Buffer/docs/sdd.md)

## Overview

Buffer management provides memory allocation, distribution, and lifecycle services for variable-size data buffers used throughout the system. Components that need temporary memory for communication data, file transfers, or data products request buffers from a buffer manager, use them, and return them when done. Additional utility components provide buffer accumulation, replication, and logging.

### Buffer Allocation

Two buffer allocation strategies are available:

- **Buffer Manager** — Allocates buffers from a pool of dynamically managed memory. The pool is configured at setup time with bin sizes and counts, allowing efficient allocation of buffers at various sizes without per-allocation heap calls. The allocator tracks outstanding buffers and detects leaks.

- **Static Memory** — Allocates buffers from statically defined memory regions. This is a drop-in replacement for the Buffer Manager that avoids dynamic memory complexity entirely. Each output port corresponds to a fixed-size memory region. This is appropriate when buffer sizes and usage patterns are fully known at design time.

In both cases, components request buffers via the Fw::BufferGet port and return them via the Fw::BufferSend port.

### Buffer Accumulation

The Buffer Accumulator accepts incoming buffers and queues them for later processing. This is useful when the data arrival rate may temporarily exceed the processing rate. Buffers are stored in an internal queue and drained in order. If the queue fills, the accumulator can either drop the newest buffer or assert, depending on the configuration. Operators can pause and resume buffer draining via command.

### Buffer Replication

The Buffer Repeater takes an incoming buffer and replicates it to multiple output ports. This is useful when the same data needs to be consumed by multiple downstream components simultaneously.

### Buffer Logging

The Buffer Logger writes incoming buffers to files on the file system. It is typically connected in the communication path to record all transmitted or received data for post-analysis. Log files are rotated based on a configurable size limit.

### Off Nominal

- If the Buffer Manager runs out of available buffers in the requested size bin, it returns an empty (invalid) buffer. The requesting component must check validity before use.
- The Buffer Accumulator reports a warning when its queue is full and a buffer must be dropped.
- Buffer leaks (buffers that are never returned) are tracked by the Buffer Manager and reported as warnings.
