# Svc::WasmSequencer

A sequence engine based around a WebAssembly (Wasm) interpreter.

## Introduction

The WasmSequencer is a component that implements the F Prime sequencing paradigm to dispatch commands, events, read telemetry parameters and more. It is based around the [WebAssembly](https://webassembly.org/) standard.

The component integrates a [WebAssembly interpreter](https://github.com/nasa/spacewasm) capable of loading, validating and executing WebAssembly modules as well as a state machine to manage the interpreter.

This document assumes a general familiarity with the capabilities and design of WebAssembly and Wasm nomenclature,
for more information, please see [WebAssembly Docs](https://webassembly.org/) for in-depth documentation
and specification of the Wasm standard.

## Build Dependencies

The [spacewasm](https://github.com/nasa/spacewasm) engine is written in Rust and compiled into a static
library at build time, so building this component requires a [Rust toolchain](https://www.rust-lang.org/tools/install)
(`cargo` and `rustc`) on the `PATH`. This is the framework's only Rust dependency, so it is treated as
optional: `cmake/required.cmake` detects `cargo`, and `Svc/CMakeLists.txt` skips `Svc::WasmSequencer`.

## Requirements

| Name         | Description                                                                                                                                                                         | Rationale                                                                                                                                                                                                                                                                       | Validation |
| ------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------- |
| WASM-SEQ-001 | The sequencer shall support loading and validating WebAssembly modules.                                                                                                             | Sequences are distributed as WebAssembly binaries; a module must be decoded and validated before execution so a malformed or untrusted binary cannot corrupt the host.                                                                                                          | Unit Test  |
| WASM-SEQ-002 | The sequencer shall support loading multiple named modules so that the exports of one module may be referenced by another.                                                          | Larger sequences are composed from reusable library modules; naming lets inter-module imports and exports resolve within a single store.                                                                                                                                        | Unit Test  |
| WASM-SEQ-003 | The sequencer shall allocate its module store for a fixed, configured capacity and shall reset it on a failed load.                                                                 | Flight software forbids heap fragmentation; a fixed-capacity store with a full reset on failure keeps allocation deterministic and returns to a known-good state.                                                                                                               | Unit Test  |
| WASM-SEQ-004 | The sequencer shall resolve sequence file paths relative to a configurable base directory.                                                                                          | Onboard sequence storage varies by deployment; a parameterized base directory avoids hard-coding filesystem layout into the flight software.                                                                                                                                    | Unit Test  |
| WASM-SEQ-005 | The sequencer shall support running sequences with arguments.                                                                                                                       | Passing arguments at dispatch time lets a single sequence binary be parameterized, so operators can reuse one module across scenarios instead of rebuilding per run.                                                                                                            | Unit Test  |
| WASM-SEQ-006 | The sequencer shall support invoking a previously loaded module's entry point on demand.                                                                                            | Decoupling load from invoke lets a module be validated and staged ahead of time, then executed without re-reading the file.                                                                                                                                                     | Unit Test  |
| WASM-SEQ-007 | The sequencer shall support both blocking and non-blocking sequence execution.                                                                                                      | Blocking lets a controlling sequence or operator serialize on completion; non-blocking allows fire-and-forget dispatch. Both are needed depending on the caller's intent.                                                                                                       | Unit Test  |
| WASM-SEQ-008 | The sequencer shall bound the number of interpreter instructions executed per cycle.                                                                                                | Fuelling the interpreter keeps a long-running or malicious guest from starving the component thread and preserves responsiveness to pause and cancellation.                                                                                                                     | Unit Test  |
| WASM-SEQ-009 | The sequencer shall support pausing a running sequence before the next directive and later resuming it.                                                                             | Operators need to halt a sequence at a safe point for inspection or intervention and resume it without restarting from the beginning.                                                                                                                                           | Unit Test  |
| WASM-SEQ-010 | The sequencer shall support cancelling a loading, ready, or running sequence and returning to the idle state.                                                                       | An operator must be able to abort a misbehaving or unneeded sequence at any point and clear the store to a known-good idle state.                                                                                                                                               | Unit Test  |
| WASM-SEQ-011 | The sequencer shall fail a sequence whose current blocking host function does not complete within a configurable timeout.                                                           | A command or serial reply that never arrives would block the sequencer indefinitely; a per-host-function timeout bounds the wait and fails the sequence safely.                                                                                                                 | Unit Test  |
| WASM-SEQ-012 | The sequencer shall let a sequence read the current spacecraft time, telemetry channel values, and parameter values.                                                                | Time-aware and conditional sequences must branch on the host's authoritative time, live telemetry, and configured parameters rather than guest-local state.                                                                                                                     | Unit Test  |
| WASM-SEQ-013 | The sequencer shall support sending commands for dispatch.                                                                                                                          | Command dispatch is the primary mechanism by which a sequence actuates the spacecraft, driving the F´ command dispatcher exactly as a ground command would.                                                                                                                     | Unit Test  |
| WASM-SEQ-014 | The sequencer shall let a sequence emit events at the non-reserved F´ severities.                                                                                                   | Guest programs need operator-visible logging; restricting to non-reserved severities prevents untrusted code from triggering the FATAL handler or spoofing command events.                                                                                                      | Unit Test  |
| WASM-SEQ-015 | The sequencer shall let a sequence sleep for a relative or absolute duration.                                                                                                       | Sequences must pace their actions against wall-clock time (settling delays, timed maneuvers) without busy-waiting and consuming interpreter fuel.                                                                                                                               | Unit Test  |
| WASM-SEQ-016 | The sequencer shall let a sequence send a fire-and-forget message out a serial output port.                                                                                         | One-way and telemetry-style peripherals are driven by pushing bytes to a selected serial port index without stalling the sequence; the send resumes the sequence as soon as the port accepts it, and fails gracefully if the port is unconnected or the payload exceeds the configured maximum. | Unit Test  |
| WASM-SEQ-017 | The sequencer shall let a sequence read a message from a serial input port, either blocking until one arrives or returning immediately when none is queued.                         | Request/response peripherals answer on a serial input; a blocking read lets a sequence wait for the reply (bounded by the per-host-function timeout) while a non-blocking read lets it poll without stalling. Inbound messages are buffered in a per-port queue.                | Unit Test  |
| WASM-SEQ-018 | The sequencer shall treat a sequence that terminates with a zero return value or a zero exit code as a success, and a non-zero return value, a non-zero exit code, or a panic as a failure. | Guests need a clean way to end execution; treating a zero return or exit(0) as success and any non-zero return value, non-zero exit code, or panic as a failure lets the host report the correct verdict to the awaiting command.                                               | Unit Test  |
| WASM-SEQ-019 | The sequencer shall validate all host-function arguments from the guest and never fault the host on invalid input, reporting an event and failing or trapping the sequence instead. | Untrusted guest code must never be able to crash the host. This covers oversized/undersized buffers, out-of-range or unconnected serial ports, invalid guest memory pointers, and reserved-severity event requests, all of which are rejected gracefully rather than asserting. | Unit Test  |
| WASM-SEQ-020 | The sequencer shall report its current state, running sequence name, and most recent trap reason as telemetry.                                                                      | Operators need continuous visibility into what the sequencer is doing and why a sequence stopped without pulling a full event log.                                                                                                                                              | Unit Test  |
| WASM-SEQ-021 | The sequencer shall count and report sequences succeeded, failed, and cancelled, and commands dispatched and failed.                                                                | Aggregate counters give operators a quick health indicator and let ground trend sequence and command reliability over a mission.                                                                                                                                                | Unit Test  |

## Design

The design of this component is tightly coupled with the [spacewasm](https://github.com/nasa/spacewasm) WebAssembly engine, which performs module loading, validation, and instruction execution. `Svc::WasmSequencer` is an **active** component (it owns a thread; the queue depth is set per instance in the topology) built around two FPP state machines: a **controller** that manages the module lifecycle (load, validate, initialize, run) and an **interpreter** that executes Wasm instructions and services the host functions a running sequence calls.

### WebAssembly Engine

The engine, [spacewasm](https://github.com/nasa/spacewasm), implements the official WebAssembly [specification](https://www.w3.org/TR/2019/REC-wasm-core-1-20191205/) with the following features:

| Feature | Description |
| --- | --- |
| [MVP](https://github.com/WebAssembly/design/blob/main/MVP.md) | The Wasm MVP |
| [Import/Export of mutable globals](https://github.com/WebAssembly/mutable-global) | Mutable globals (part of Wasm 1.0) |
| [Custom Page Sizes](https://github.com/WebAssembly/custom-page-sizes) | Linear memory sizes < 64 KiB |

Rather than JIT-compiling to native code, spacewasm is an _interpreter_ — more precisely an _IR interpreter_: during load it translates Wasm byte-code into a resolved intermediate representation that it then decodes and executes in a loop. This is slower than a production JIT but gives the determinism and safety guarantees flight software needs. This document does not cover the engine's internals; see the [spacewasm repository](https://github.com/nasa/spacewasm). A few of its design choices drive this component:

- **Fuel.** The interpreter loop runs at most a bounded number of IR instructions per call and then yields, so a long-running or malicious guest cannot starve the component thread and pause/cancel stay responsive. The bound is the `INSTRUCTION_FUEL` parameter.
- **Dynamic memory.** spacewasm allocates its module data structures and compiled byte-code from a "global allocator" the embedder provides. This component backs it with a fixed static pool (`SPACEWASM_PAGE_SIZE` × `SPACEWASM_MAX_PAGES`), keeping peak memory deterministic and free of heap fragmentation. Guest linear memory is served from a separate per-load pool (`GUEST_MEMORY_SIZE`).
- **Panics.** spacewasm is written in Rust and compiled with `panic = "abort"`. A panic is routed through a `spacewasm_panic` callback that logs the location to `Os::Console` and then `FW_ASSERT(false)` — terminal for the FSW, and distinct from a guest-level `panic()` (which fails only the sequence). spacewasm is [fuzzed](https://github.com/nasa/spacewasm/tree/main/fuzz/fuzz_targets) to keep this path from firing on arbitrary input.

**Specification divergences.** spacewasm diverges from the spec in a few documented ways for determinism:

1. `memory.grow` can be disabled at load time; WasmSequencer disables it to keep its allocator simple (this may be relaxed later).
2. A failed module load invalidates the whole store (the engine compiles all modules into one append-only IR arena and commits guest memory eagerly, neither of which can be partially rolled back), so the controller rebuilds the store from scratch on any load failure — the behavior required by WASM-SEQ-003.
3. A module's optional `start` function must resolve to a Wasm function, not a host function, so it can be driven through the same fuel-bounded run loop as `main`.

### Modules, Names, and Linking

The engine holds a single _store_ — a fixed-capacity table of up to `MAX_GUEST_MODULES` guest modules plus the reserved host module — that every loaded module shares. Each guest module is loaded under a **name**, which is how other modules (and later commands) refer to it. Loading is additive: each `LOAD` adds another module to the current store.

- **`LOAD`** loads a module under a caller-chosen name (which must not already be in use). Naming a module is what lets _another_ module import its exports, and lets `INVOKE`, `GLOBAL_GET`, and `GLOBAL_SET` address it afterward. An empty name is the single-module shorthand — a normal name that only one module can occupy.
- **`INVOKE`** runs the `main` entry point of an already-loaded module named by the caller (the empty name for a module loaded without one).
- **`RUN`** is the one-shot convenience for a self-contained module: it _resets the store_, loads the file under the empty name, and invokes its `main`. Because it resets the store first, `RUN` discards any modules previously staged with `LOAD`; stage libraries with `LOAD` and then `INVOKE` the top module when a run needs them.

**Linking.** A guest module's Wasm imports are `(module, field)` pairs resolved at load time against what is already in the store: the reserved host module **`fprime_v1`** (the host functions listed under [Host Functions](#host-functions)) and any guest module already loaded under the referenced name. Each import is checked for a matching signature, so a missing target or a type mismatch fails the _load_ rather than the run. Because resolution happens at load time, **a library module must be loaded before the module that imports it.** A multi-module sequence therefore stages its libraries first:

```
LOAD    mathlib.wasm  "math"    ; exports add, mul
LOAD    app.wasm      "app"     ; imports ("math", "add"), ...
INVOKE  "app"
```

A loaded module's exported, mutable globals can additionally be read and written from the ground between runs with `GLOBAL_GET` / `GLOBAL_SET`, addressed by module name.

### State Machines

The component is driven by two state machines whose (flattened) leaf states are reported on telemetry. The transition detail is in the diagrams below; the FPP sources are `WasmSequencerControllerStateMachine.fppi` and `WasmSequencerInterpreterStateMachine.fppi`.

#### Controller

![Controller State Machine Diagram](ControllerStateMachine.svg)

The controller owns the module lifecycle. From `IDLE` (no store) a `LOAD` reads and validates a module into the store and settles in `READY`; a `RUN` does the same and then chains into running the module's optional `start` function and its `main` entry point. Because a failed load invalidates the store, the controller returns to `IDLE` — whose entry rebuilds a fresh store — on any load failure, and resets the store before every `RUN`. Execution is handed to the interpreter, which reports back when it finishes; the controller then classifies the outcome (a success only if the run finished with a zero return/exit code) and answers the caller — a `BLOCK` caller (and any pending `WAIT`) on completion, a `NO_BLOCK` caller as soon as the sequence starts. Requests that arrive while the controller is busy are rejected with `BUSY`. Cancelling a running sequence is realized through the interpreter: `CANCEL` signals the interpreter, which unwinds and reports back so the controller can count the sequence as cancelled.

#### Interpreter

![Interpreter State Machine Diagram](InterpreterStateMachine.svg)

The interpreter executes the loaded program in fuel-bounded slices and services the host functions it calls. Each slice runs the interpreter until it finishes, traps, exhausts its fuel (loop and spin again — the point at which a pending `PAUSE` or `CANCEL` takes effect), or a guest host call pauses it. Most host functions (reading time, telemetry, parameters or arguments, emitting an event, sending serial output) complete immediately and resume the guest. Two are asynchronous: a dispatched command (`cmd`) awaits its response on `cmdResponseIn`, and a blocking `serial_recv` awaits an inbound message on `serialIn` — both bounded by the `HOST_FUNCTION_TIMEOUT_SECS` parameter and checked on the `checkTimers` tick. Sleeps (`rsleep`/`asleep`) are not bounded by that parameter; they wait on their own guest-requested wake timer, also checked each `checkTimers` tick. When the program ends the interpreter records why — a normal finish (with its return code), a guest `exit`/`panic` (with its code), or a byte-code trap (with its reason) — and reports it to the controller, which emits the matching completion event.

### Allocator Lock

spacewasm exposes a _single, process-wide_ heap allocator with no per-call context, so in a deployment with more than one `WasmSequencer` the active allocator must be selected out-of-band. The `fprime_spacewasm` shim keeps a small registry (capacity `MAX_SEQUENCERS`) of per-instance page pools, and each instance registers a slot at construction. A process-wide mutex brackets the allocating operations — store creation, store teardown, and module load — selecting the calling instance's allocator for their duration; execution runs outside the lock against the already-allocated store. Consequently the allocating operations of all instances are serialized process-wide (execution is not), and the number of live instances is capped at `MAX_SEQUENCERS`.

### Ports

| Port Type | Name | Direction | Kind | Usage |
|---|---|---|---|---|
| `Fw.Com` | `cmdOut` | Output | — | Dispatches commands to the command dispatcher (backs the guest `cmd`). |
| `Fw.CmdResponse` | `cmdResponseIn` | Input | async | Receives responses to dispatched commands. |
| `Fw.TlmGet` | `getTlmChan` | Output | — | Reads telemetry channel values (backs the guest `tlm`). |
| `Fw.PrmGet` | `getParam` | Output | — | Reads parameter values (backs the guest `prm`). |
| `Svc.Sched` | `checkTimers` | Input | async | Periodically drives sleep-wake and host-function-timeout checks. |
| `Svc.Sched` | `writeTelemetry` | Input | async (drop) | Optional periodic telemetry write. |
| `serial` | `serialOut[]` | Output | — | Serial output triggered by the guest `serial_send`. |
| `serial` | `serialIn[]` | Input | async | Inbound serial messages, queued per port. |
| `Svc.CmdSeqIn` | `seqRunIn` | Input | async | Request to run a sequence (as the `RUN` command). |
| `Svc.CmdSeqCancel` | `seqCancelIn` | Input | async | Request to cancel the running sequence (as `CANCEL`). |
| `Svc.CmdSeqIn` | `seqStartOut` | Output | — | Signalled when a sequence begins running. |
| `Fw.CmdResponse` | `seqDoneOut` | Output | — | Signalled when a sequence finishes. |

The serial-port array bound comes from the `Svc.Wasm.SerialPortOutIndex` / `SerialPortInIndex` enums in `config/WasmSequencerCfg.fpp`. The component also uses the standard command, event, telemetry, parameter, and time special ports.

## Commands

All commands are asynchronous except `CANCEL`, which is synchronous.

| Command | Description |
|---|---|
| `RUN` | Reset the store, then load, invoke `main`, and run a module in one step; waits for completion when `$block == BLOCK`. |
| `LOAD` | Load and validate a module into the store under the given name (an empty name for a single standalone module); its exports can then be referenced by other modules. |
| `INVOKE` | Invoke `main` on an already-loaded module. |
| `WAIT` | Wait for the running sequence to finish and return its result. |
| `CANCEL` | Cancel a loading/running sequence and return to idle. |
| `PAUSE` / `CONTINUE` | Pause the running sequence at the next boundary / resume it. |
| `GLOBAL_SET_I32` / `_I64` / `_F32` / `_F64` | Set an exported, mutable global of the given type. |
| `GLOBAL_GET` | Read an exported global's value and report it in an event. |

## Host Functions

A running sequence reaches the host through a single Wasm import module, `fprime_v1`. These imports are the programming interface exposed to guest programs:

| Import | Purpose |
|---|---|
| `exit` / `panic` | End the sequence with an exit code / abort it with a panic code. |
| `args` | Read the sequence's invocation arguments. |
| `time` | Read the current spacecraft time. |
| `tlm` / `prm` | Read a telemetry channel / parameter value. |
| `cmd` | Dispatch an F´ command and await its response. |
| `event` | Emit a log event (non-reserved F´ severities only). |
| `rsleep` / `asleep` | Sleep for a relative / absolute duration. |
| `serial_send` | Send bytes out a serial port (fire-and-forget). |
| `serial_recv` | Read bytes from a serial input port (blocking or non-blocking). |

Guest arguments are validated on every call; invalid input (bad pointers, oversized/undersized buffers, out-of-range or unconnected ports, reserved event severities) fails or traps the sequence and emits a warning event rather than faulting the host (WASM-SEQ-019).

## Parameters

| Parameter | Type | Default | Description |
|---|---|---|---|
| `SEQ_BASE_DIR` | `string` | `""` | Base directory that sequence file paths are resolved against. |
| `INSTRUCTION_FUEL` | `FwSizeType` | `1000` | IR instructions executed per interpreter cycle; larger values run faster but respond to `PAUSE` less quickly. |
| `HOST_FUNCTION_TIMEOUT_SECS` | `F32` | `0` | Timeout for a blocked host function (a dispatched command or a blocking `serial_recv`); `<= 0` disables it. |

## Telemetry and Events

The full set of telemetry channels and events, with their arguments, is defined in the FPP model and enumerated in the generated dictionary. In summary, the component reports the controller and interpreter state-machine states, cumulative counters (sequences succeeded / failed / cancelled, commands dispatched / failed), the most recent trap reason, and the running sequence name (WASM-SEQ-020, WASM-SEQ-021). Its events cover module-load and file errors, the sequence lifecycle and its per-branch completion outcomes (exited / panicked / trapped / host-failure / cancelled, each tagged with the `START` or `MAIN` phase), guest-argument-validation failures, and the guest-emitted log events produced by the `event` host function.

## Configuration

Compile-time configuration lives in `config/WasmSequencerConfig.hpp`:

| Constant | Value | Purpose |
|---|---|---|
| `SPACEWASM_PAGE_SIZE` × `SPACEWASM_MAX_PAGES` | `8192` × `4` | Fixed static pool backing the interpreter store and compiled byte-code (must match the `spacewasm_c_api` crate). |
| `GUEST_MEMORY_SIZE` | `2048` | Pool backing guest linear memory (bounds the largest guest memory accepted; `memory.grow` is disabled). |
| `GUEST_STACK_SIZE` | `256` | Guest operand stack size per store. |
| `MAX_GUEST_MODULES` | `8` | Maximum modules loadable into a store. |
| `MAX_SERIAL_PORT_SIZE` | `256` | Maximum `serial_send` payload; a larger request traps the guest. |
| `SERIAL_IN_QUEUE_SIZE` / `_FULL_BEHAVIOR` | `256` / `DROP_OLDEST` | Size and overflow policy of each inbound `serialIn` queue. |
| `LOAD_READ_CHUNK_SIZE` | `512` | Buffer size for streaming a module file into the decoder. |

Runtime behavior is controlled through the parameters above; the serial-port index enums and default base directory live in `config/WasmSequencerCfg.fpp`.

## Unit Testing

Unit tests live in `test/ut/`. To run them with coverage:

```
fprime-util check --coverage
```
