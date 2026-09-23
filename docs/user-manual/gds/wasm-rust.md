# Advanced Sequencing In F´ with Rust

F´ supports writing sequences that are programs rather than lists of commands. An advanced sequence can dispatch
commands, read telemetry channels and parameters, emit events, sleep, and exchange messages with the rest of the
flight software.

Sequences are written in [Rust](https://www.rust-lang.org/),
compiled to a [WebAssembly](https://webassembly.org/) module, uploaded, and run
on board by the command sequencer.

For more information on the underlying F´ component, reference the [Svc::WasmSequencer](../../../Svc/WasmSequencer/docs/sdd.md) SDD documentation. For the system-level description of this capability, see [Advanced Sequencing Functionality](../../reference/system-functional/advanced-sequencing.md).

> [!TIP]
> For simple, linear, time-tagged sequences, prefer [standard sequencing](./seqgen.md) and `fprime-seqgen`.

## Dependencies

Advanced sequences are built with the Rust toolchain and the `fprime-wasm` tool.

```shell
# Install Rust: https://doc.rust-lang.org/cargo/getting-started/installation.html
rustup target add wasm32v1-none

cargo install fprime-wasm
cargo install wasm-opt  # optional, optimizes .wasm binaries for size
```

## Creating A Sequence Project

Sequences live in their own Rust project, one project per deployment. `fprime-wasm init` scaffolds it against that
deployment's dictionary. Remember to build the deployment first or the dictionary will not be generated.

```shell
mkdir ref-sequences && cd ref-sequences
fprime-wasm init --dictionary ../fprime/build-artifacts/*/Ref/dict/RefTopologyDictionary.json
```

Each sequence is a file in `src/bin/`, added with `fprime-wasm add`, which also creates a test for it in `tests/`:

```shell
fprime-wasm add safing
```

## Writing A Sequence

The project's dictionary is generated into a typed Rust API, so every command, telemetry channel, and parameter in
the deployment is a named call and a misspelling is a compile error rather than a surprise in flight.

```rust
#![no_std]
#![no_main]

use ref_sequences::*;

#[fprime_main]
pub fn main() {
    // Fail the sequence if a dispatched command does not respond OK
    set_fail_mode(FailMode::Checked);

    // Or let responses bubble up to the caller instead of failing the sequence:
    // set_fail_mode(FailMode::Permissive);

    message(EventSeverity::ActivityHigh, "starting safing");

    if Ref.power.BatteryVoltage().0 < 21.5 {
        Ref.power.PWR_OFF();
        rsleep(5_000_000);  // microseconds
    }

    CdhCore.cmdDisp.CMD_NO_OP_STRING("Hello wasm!");
}
```

A sequence succeeds by returning or by calling `exit(0)`, and fails with a non-zero `exit`, a `panic`, or a Rust
`assert!` failure. A sequence may also loop indefinitely, waiting on a serial port for work to do.

> [!NOTE]
> Sequences are `no_std` programs: there is no heap, no operating system, and no standard library. Everything a
> sequence can ask of the flight software goes through the generated dictionary API and the helpers in `fprime_core`.

## Testing A Sequence

Each generated test runs the compiled sequence on the same interpreter the flight component embeds, against a
spacecraft simulated from the deployment's dictionary. A test states the conversation the sequence should have with
the spacecraft: what it reads, what it commands, and how it ends.

```rust
use fprime_test::*;
use ref_sequences::*;

#[fprime_test(sequence = "safing")]
fn powers_off_when_the_battery_is_low(t: Test) {
    t.initial_telemetry(Ref.power.BatteryVoltage, 21.0);

    t.expect_command(Ref.power.PWR_OFF());
    t.expect_command(CdhCore.cmdDisp.CMD_NO_OP_STRING("Hello wasm!"));
    t.expect_ok();
}
```

```shell
fprime-wasm test
```

The steps must happen in the order listed. A sequence may do other things in between, and commands the test says nothing about succeed.

## Compiling A Sequence

```shell
fprime-wasm build
```

The modules are written to `target/wasm32v1-none/release/*.wasm`, one per file in `src/bin/`. These are the binaries
that get uploaded to the flight software. File uplink is sandboxed to a directory by the deployment, so upload to a
relative destination (`safing.wasm`) rather than an absolute path.

Before uploading, `fprime-wasm verify` loads each module into the on-board interpreter without running it, and sizes
it against the limits in `sequencer.toml`. This is the equivalent of the on-board `LOAD` and makes a cheap CI gate:

```shell
fprime-wasm verify
```

> [!NOTE]
> `sequencer.toml` records the memory and instruction limits of the `Svc.WasmSequencer` instance that will fly the
> sequences. Keep it matched to the deployment's `instances.fpp` configuration, or a sequence that verifies on the
> ground may fail to load on board. A deployment with more than one sequencer instance wants one file per instance.

> [!TIP]
> For what a sequence costs for uplink and on board see
> [Advanced Sequence Size In F´](./wasm-size.md).

## Running The Sequence

Once uploaded, the sequence is run with the `.RUN` command of any `Svc.WasmSequencer` instance. Here we run the
`wasmSeq` instance's `.RUN` command on `safing.wasm`, waiting for it to finish, with no arguments:

```
wasmSeq.RUN "safing.wasm", BLOCK
```

`BLOCK` will hold the command responds of the `RUN` until the sequence has completed

With `BLOCK`, the `RUN` command itself carries the sequence's verdict: it completes `OK` only if the sequence did.
`NO_BLOCK` completes as soon as the sequence starts, and the result can be collected later with `WAIT`.

Other commands control the sequence while it runs, or stage sequences ahead of time:

| Command                       | Description                                                                         |
| ----------------------------- | ----------------------------------------------------------------------------------- |
| `RUN`                         | Load and run a sequence in one step. Waits for completion when `$block` is `BLOCK`. |
| `LOAD`                        | Load and validate a sequence under a name, without running it.                      |
| `INVOKE`                      | Run a previously loaded sequence by name.                                           |
| `WAIT`                        | Wait for the running sequence to finish and return its result.                      |
| `PAUSE` / `CONTINUE`          | Pause the running sequence at its next boundary / resume it.                        |
| `CANCEL`                      | Cancel the running sequence and discard all loaded sequences.                       |
| `GLOBAL_GET` / `GLOBAL_SET_*` | Read or write a value a loaded sequence exposes.                                    |

`LOAD` and `INVOKE` let a sequence be staged and validated ahead of time and then run repeatedly without re-reading
the file, and let one sequence call into another that was loaded first. See the
[SDD](../../../Svc/WasmSequencer/docs/sdd.md) for multi-module sequences and for the project-defined serial ports a
sequence uses to talk to components outside the standard framework interfaces.
