
# Command Dispatch Functionality

## References

- [F Prime Command Dispatcher SDD](https://github.com/nasa/fprime/blob/devel/Svc/CmdDispatcher/docs/sdd.md)

## Overview

Command dispatching is the process by which encoded command buffers are received, decoded, routed to the appropriate component for execution, and status is reported back to the command source. The Command Dispatcher is the central routing component in the F Prime commanding architecture. It connects command sources (ground systems, sequencers, or other command buffer sources) to the components that implement each command.

### Command Registration

At system startup, each component that implements commands registers its opcodes with the Command Dispatcher. Registration associates each opcode with the specific output port connected to the implementing component. If an opcode is registered more than once on the same port, this is treated as a re-registration and is logged. If an opcode is registered from a different port, this is an assertion failure — normal topology generation prevents this condition, so it indicates a configuration error. The registration table is sized at compile time (default: 150 entries) and must be large enough to hold all opcodes in the system.

### Command Decoding and Routing

When the Command Dispatcher receives a command buffer, it performs the following steps:

1. Deserialize the command packet from the incoming buffer
2. Extract the opcode from the deserialized packet
3. Look up the opcode in the dispatch table
4. Assign a unique sequence number to the command
5. Record the command in the pending command tracker (opcode, sequence number, context value, and source port)
6. Dispatch the command to the registered component via the corresponding output port

The context value is a user-defined value provided by the command source. It is returned with the command status to allow the source to correlate the response with the original command.

### Command Sources

The Command Dispatcher supports multiple command sources connected to separate input ports. Each source port has a corresponding status output port so that command completion status is returned to the correct source. The number of command source ports is configurable at compile time (default: 5).

Typical command sources include:

- Ground system uplink
- Command sequencers
- Other software components issuing commands internally

### Command Status Tracking

After a command is dispatched, it is tracked in a pending command table until the executing component reports completion. The pending command table is sized at compile time (default: 25 entries) and limits the number of commands that can be simultaneously in progress.

When the executing component reports completion, the Command Dispatcher:

1. Matches the reported sequence number to the pending command entry
2. Logs the completion status (success or error)
3. Forwards the status to the command source via the corresponding status output port along with the original context value

### Queue Overflow Protection

The Command Dispatcher's input queue has a finite depth. If the queue is full when a new command buffer arrives, the command is dropped rather than blocking or asserting. The Command Dispatcher's queue processing is typically assigned the highest priority in the system, so overflows are most likely during a denial-of-service or command flood situation. Dropped commands are logged and counted via telemetry.

### Telemetry

The Command Dispatcher reports the following telemetry:

- **Commands Dispatched** — Total number of commands successfully dispatched
- **Command Errors** — Total number of commands that completed with an error status
- **Commands Dropped** — Total number of commands dropped due to queue overflow

### Test Commands

The Command Dispatcher includes several built-in commands for system verification:

- **NO_OP** — A no-operation command that does nothing except confirm the command path is functional
- **NO_OP_STRING** — A no-operation command that accepts a string argument and echoes it via an event
- **TEST_CMD_1** — A test command that accepts multiple argument types (I32, F32, U8) and reports them via an event
- **CLEAR_TRACKING** — Clears the pending command table to recover from situations where components have failed to report command completion status

### Off Nominal

- **Malformed Command** — If the command buffer cannot be deserialized, the command is rejected with a validation error and an event is logged.
- **Invalid Opcode** — If the opcode is not found in the dispatch table, the command is rejected and an error event is logged.
- **Too Many Pending Commands** — If the pending command tracker is full, the command cannot be tracked and is rejected with an execution error event.
- **Queue Overflow** — If the Command Dispatcher's input queue is full, the command is dropped and an overflow event is logged. This event is throttled to prevent log flooding.
- **Missing Status Response** — If a component fails to report command completion, the entry remains in the pending command tracker indefinitely. The CLEAR_TRACKING command can be used to manually recover from this state.
