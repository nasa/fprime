# Svc::WasmSequencer

A sequence engine based around a WebAssembly (Wasm) interpreter.

## Introduction

The WasmSequencer is a component that implements the F Prime sequencing paradigm to dispatch commands, events, read telemetry parameters and more. It is based around the [WebAssembly](https://webassembly.org/) standard.

The component integrates a [WebAssembly interpreter](https://github.com/nasa/spacewasm) capable of loading, validating and executing WebAssembly modules as well as a state machine to manage the interpreter.

This document assumes a general familiarity with the capabilities and design of WebAssembly and Wasm nomenclature,
for more information, please see [WebAssembly Docs](https://webassembly.org/) for in-depth documentation
and specification of the Wasm standard.

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
| WASM-SEQ-011 | The sequencer shall fail a sequence whose current statement does not complete within a configurable timeout.                                                                        | A command or serial reply that never arrives would block the sequencer indefinitely; a per-statement timeout bounds the wait and fails the sequence safely.                                                                                                                     | Unit Test  |
| WASM-SEQ-012 | The sequencer shall let a sequence read the current spacecraft time, telemetry channel values, and parameter values.                                                                | Time-aware and conditional sequences must branch on the host's authoritative time, live telemetry, and configured parameters rather than guest-local state.                                                                                                                     | Unit Test  |
| WASM-SEQ-013 | The sequencer shall support sending commands for dispatch.                                                                                                                          | Command dispatch is the primary mechanism by which a sequence actuates the spacecraft, driving the F´ command dispatcher exactly as a ground command would.                                                                                                                     | Unit Test  |
| WASM-SEQ-014 | The sequencer shall let a sequence emit events at the non-reserved F´ severities.                                                                                                   | Guest programs need operator-visible logging; restricting to non-reserved severities prevents untrusted code from triggering the FATAL handler or spoofing command events.                                                                                                      | Unit Test  |
| WASM-SEQ-015 | The sequencer shall let a sequence sleep for a relative or absolute duration.                                                                                                       | Sequences must pace their actions against wall-clock time (settling delays, timed maneuvers) without busy-waiting and consuming interpreter fuel.                                                                                                                               | Unit Test  |
| WASM-SEQ-016 | The sequencer shall let a sequence invoke a serial output port either synchronously or asynchronously, awaiting a reply.                                                            | Fire-and-forget peripherals use the synchronous form; request/response peripherals use the asynchronous form and block on the matching reply, subject to the statement timeout.                                                                                                 | Unit Test  |
| WASM-SEQ-017 | The sequencer shall let a sequence terminate itself, reporting a non-zero exit code or a panic as a sequence failure.                                                               | Guests need a clean way to end execution; distinguishing a zero exit from a non-zero code or panic lets the host report success versus failure to the awaiting command.                                                                                                         | Unit Test  |
| WASM-SEQ-018 | The sequencer shall validate all host-function arguments from the guest and never fault the host on invalid input, reporting an event and failing or trapping the sequence instead. | Untrusted guest code must never be able to crash the host. This covers oversized/undersized buffers, out-of-range or unconnected serial ports, invalid guest memory pointers, and reserved-severity event requests, all of which are rejected gracefully rather than asserting. | Unit Test  |
| WASM-SEQ-019 | The sequencer shall report its current state, running sequence name, and most recent trap reason as telemetry.                                                                      | Operators need continuous visibility into what the sequencer is doing and why a sequence stopped without pulling a full event log.                                                                                                                                              | Unit Test  |
| WASM-SEQ-020 | The sequencer shall count and report sequences succeeded, failed, and cancelled, and commands dispatched and failed.                                                                | Aggregate counters give operators a quick health indicator and let ground trend sequence and command reliability over a mission.                                                                                                                                                | Unit Test  |

## Design

The design of this component is tightly coupled with the design of the SpaceWasm WebAssembly engine.
The WebAssembly engine/interpreter is responsible to performing the loading and validation of the
WebAssembly module as well as the execution webassembly instructions. The component implements a state
machine that drives the engine and all of it's features.

### WebAssembly Engine

The WebAssembly engine, [spacewasm](https://github.com/nasa/spacewasm) is a WebAssembly interpreter
implementing the official WebAssembly [specification](https://www.w3.org/TR/2019/REC-wasm-core-1-20191205/)
with the following features:

| Feature                                                                           | Description                         |
| --------------------------------------------------------------------------------- | ----------------------------------- |
| [MVP](https://github.com/WebAssembly/design/blob/main/MVP.md)                     | The Wasm MVP                        |
| [Import/Export of mutable globals](https://github.com/WebAssembly/mutable-global) | Mutable globals (part of Wasm 1.0)  |
| [Custom Page Sizes](https://github.com/WebAssembly/custom-page-sizes)             | Supports linear memory sizes <64KiB |

Future spacewasm updates may include additional Wasm proposals.

This document will no go into depth of the design of the spacewasm interpreter, for more information on that
please read the documentation in the [spacewasm repository](https://github.com/nasa/spacewasm).
There are a couple of design choices that are highlighted in this document that drive the design
of the component.

#### Execution Engine

Spacewasm is a WebAssembly _interpreter_. This is different from the typical Wasm runtime which typically perform some form of Just-In-Time compilation to translate Wasm byte-code into native instructions. An interpreter will read and implement the decoding/execution pipeline in software rather than using hardware acceleration. While this means the engine is much slower than typical production runtimes, it provides determinism and safety guarentees that JITs cannot.

More accurately spacewasm is an _IR_ interpreter. This means that the engine does not execute Wasm byte-code instructions directly but rather during loading/validation, the WebAssembly byte-code is translated into another intermediate representation (IR). This IR holds resolved information such as branch points and may transform instructions
in other ways to improve execution speed. Once translated, the IR form is decoded and executed in a loop.

##### Fuel

Fuel is a standard concept in Wasm interpreters that essentially bounds the execution time of the interpreter loop to check in with a higher level before continuing. This Wasm interpreter implements fuel through simple instruction counting. The number of instructions to execute per cycle is configurable as a parameter.

#### Dynamic Memory

The spacewasm engine performs "inner" dynamic memory allocation when loading Wasm modules. These allocations hold data-structures such as the module's functions and their signatures, Wasm executable byte-code, imports/exports etc. To perform this
allocation, an engine embedding must provide a "global-allocator" to implement
the spacewasm malloc/free functionality.

The exact design choices of the spacewasm allocation strategy can be found in
the [spacewasm requirements](https://github.com/nasa/spacewasm/blob/main/docs/REQUIREMENTS.md) and the [spacewasm readme](https://github.com/nasa/spacewasm#dynamic-allocation).

#### Panics

The spacewasm engine is written in the [Rust](https://rust-lang.org/) programming language. While this language performs many compile-time checks for memory and type safety, there are certain checks that are left to the run-time. When a certain invariant is not met, Rust will invoke a "panic" is mapped to a function implemented in this component.

Rust panics will provide a `Os::Console` message with the panic file location/line number followed by an `FSW_ASSERT(false)`.

To avoid panics, spacewasm is [fuzzed](https://github.com/nasa/spacewasm/tree/main/fuzz/fuzz_targets) against a variety of targets using the official production-grade Wasm fuzzer: [wasm-smith](https://github.com/bytecodealliance/wasm-tools/tree/main/crates/wasm-smith).

#### Specification Divergences

Spacewasm _slightly_ diverges from the official WebAssembly spec in a few documented ways for the purposes of performance and determinisim.

1. `memory.grow` can be disabled at load-time. This allows the execution engine's allocator to be dead simple and in-compliance with standard FSW practices.

> [!NOTE] Although spacewasm allows loading modules with `memory.grow`, WasmSequencer disables this functionality to keep the allocator provided by this component simple. This restriction may be lifted in the future.

2. Failed module loading invalidates the store and backing memory. This is a common point of mis-interpretation by external reviewers. The implications of this is that a failed module load will require the store/engine to reset completely. This is due to the following two reasons:

    a. 

    b.

1. Module start cannot be mapped directly host functions.

- This is an implementation detail that is very unlikely (actually impossible) to affect the typical user. This requires the optional module start function to be mapped to a Wasm function rather than a host function. This is because the state machine expects to need to spin the interpreter if a 

### Component

The F Prime WasmSequencer is made up of two state machines:

1. Controller: Responsible for tracking the reading, validation, initialization and execution of Wasm modules.
2. Engine: Responsible for executing Wasm instructions and handling asynchronous host behaviors.

#### Controller

![Controller State Machine Diagram](ControllerStateMachine.svg)

The controller state machine manages the loading and initialization of Wasm
modules. 

#### Allocator Lock



## Configuration

<!-- If the component requires configuration at initialization, document here -->
