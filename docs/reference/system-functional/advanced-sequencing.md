# Advanced Sequencing Functionality

## References

- [F Prime Wasm Sequencing SDD](https://github.com/nasa/fprime/blob/devel/Svc/WasmSequencer/docs/sdd.md)
- [F Prime Sequencing Functionality](./sequencing.md)
- [F Prime Sequence Dispatcher SDD](https://github.com/nasa/fprime/blob/devel/Svc/SeqDispatcher/docs/sdd.md)
- [Wasm Core Specification](https://www.w3.org/TR/wasm-core-1/)
- [F Prime Command Dispatcher SDD](https://github.com/nasa/fprime/blob/devel/Svc/CmdDispatcher/docs/sdd.md)

## Overview

The purpose of advanced sequencing in F Prime is to model spacecraft behaviors at a system level. Advanced
sequences serve much the same purpose as the [standard sequencing](./sequencing.md) capability does though
it allows general interaction with commands, telemetry, parameters, events, and ports. While standard sequencing
is a simple linear list of commands, advanced sequencing can have logic and conditionality etc.

Advanced sequencing in F Prime is based on the WebAssembly (Wasm) standard. F Prime ships with a Wasm interpreter and component capable of loading and executing general Wasm programs (modules).

### Sequence Management

Prior to execution, advanced sequences are Wasm module files stored in the file system and managed with the same file system functionality as any other on-board file. Sequence file names are resolved against a base directory that is configurable at runtime.

Advanced sequences are authored and compiled on the ground rather than on board. A sequence is written in a general-purpose programming language, compiled to a Wasm module, and uplinked as a file. See the [user manual](../../user-manual/gds/wasm-rust.md) for the ground authoring, test, and build workflow, and [Advanced Sequence Size In F´](../../user-manual/gds/wasm-size.md) for what a sequence costs on the wire and on board relative to a standard sequence.

Each sequencing engine reserves its memory up front at deployment configuration time: a fixed pool for the interpreter and its compiled program, a fixed pool for sequence working memory, and a fixed limit on the number of modules that may be resident at once. Nothing is allocated dynamically once the system is running, so the maximum size and working set of a sequence are bounded by these configured limits.

### Sequence Loading and Validation

Sequence validation is performed on board when the module is loaded and is composed of the following checks:

1. Confirm that the file decodes as a well-formed Wasm module.
2. Confirm that the module is valid per the Wasm specification (structural and type validation).
3. Confirm that every external function and value the module requires is provided by the system interface or by an already-loaded module.
4. Confirm that the module's memory and compiled program fit within the configured limits of the engine.

If any of these validation checks fail, the sequence will not load, an error will be reported, and the engine returns to an idle state with no sequence resident.

The same validation can be performed on the ground before uplink, which allows a bad sequence to be rejected during development rather than in flight.

Multiple sequences may be loaded together and reference each other, which allows common behavior to be factored into reusable library sequences. A library sequence must be loaded before the sequence that references it.

### Sequence Execution

F Prime can be configured with multiple advanced sequencing engines. Each engine executes one sequence at a time. Requests that arrive while an engine is busy are rejected. As with standard sequencing, a pool of engines can be coordinated by a Sequence Dispatcher.

Sequences can be executed in two ways:

1. Run - Load a sequence file and execute it in one step.
2. Load then invoke - Stage one or more sequences on board and execute an entry point on demand, without re-reading the files. State held by a loaded sequence persists across invocations.

Execution may be requested as blocking, where the requester is acknowledged only once the sequence has finished, or non-blocking, where the requester is acknowledged as soon as the sequence starts. A separate wait request can be used to await completion of a sequence that was started (in non-blocking or blocking modes).

A running sequence can be paused, resumed, or cancelled. Cancellation returns the engine to idle and discards all loaded sequences and their state.

### System Interaction

A running sequence interacts with the rest of the flight software through a fixed system interface. Through it, a sequence can:

- Dispatch any software command and observe its response.
- Read telemetry channel values and their update times.
- Read parameter values.
- Emit events at the non-reserved severities.
- Read the current spacecraft time.
- Send and receive arbitrary messages on project-defined ports, which lets a project expose behaviors beyond the standard framework interfaces to its sequences.

A sequence executes in isolation from the rest of the flight software. It cannot read or write flight software memory, and all data crossing the boundary is passed by value. Every request a sequence makes is validated, and an invalid request fails the sequence rather than the system.

Values that a loaded sequence chooses to expose can be read, and written when the sequence permits it, by command. This allows tuning constants, thresholds, and flags to be adjusted from the ground without reloading the sequence.

### Sequence Timing

Advanced sequences do not carry a time tag per command. Timing is expressed by the sequence itself, which may wait for a relative duration or until an absolute time, and may decide what to do next based on the current time, telemetry, or parameters.

Waits do not consume execution budget, and are honored to the resolution of the periodic rate at which the engine is scheduled.

Requests that require a reply from another component (a dispatched command, or a blocking read of a project-defined port) are bounded by a configurable timeout, so a reply that never arrives cannot block a sequence indefinitely.

### Off Nominal

A sequence completes successfully when it returns or exits with a zero status. It fails when it returns or exits with a non-zero status, terminates itself abnormally, makes an invalid request of the system interface, exceeds a reply timeout, or is cancelled.

The failure of an individual command dispatched by a sequence does not necessarily abort the sequence. Unlike standard sequencing, the sequence decides whether a command failure halts the sequence, which allows retries, fallbacks, and safing behavior to be expressed within the sequence itself.

On any failure, the engine reports the reason, returns to idle, and discards all loaded sequences and their state. The engine continuously reports its current state, the sequence it is running, the most recent failure reason, and cumulative counts of sequences succeeded, failed, and cancelled along with commands dispatched and failed.
