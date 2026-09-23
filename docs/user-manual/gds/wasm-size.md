# Advanced Sequence Size In F´

An [advanced sequence](./wasm-rust.md) is a compiled program, so it _can_ be bigger than the
[standard sequence](./seqgen.md) binary that encodes the same list of commands. This page gives
measured sizes for both, the marginal cost of adding one more command to each, and the two places
where a Wasm sequence gets expensive far out of proportion to what it appears to do.

> [!IMPORTANT]
> String formatting with `format!("{my_variable}")` anywhere in a sequence costs roughly 3.4 KB,
> which is more than every other sizing decision on this page put together.
> See [String formatting](#string-formatting).

## Two budgets

File size is the uplink cost, and it is the number this page compares. It is not the number that decides whether a
sequence loads. On board, a module is sized against the memory the `Svc.WasmSequencer` instance was configured with,
and `fprime-wasm verify` reports all of it:

```
Limits (defaults): memory 8192 B, heap 8 pages, code 256 pages, operand stack 1024 words, page 8192 B

  Module                   Bytes  Memory  Heap  Code  Fits
  -----------------------  -----  ------  ----  ----  ----
  baseline                    50     512     2     1  ok
  cmd_no_args                122     516     2     1  ok
  ...
  example                    641     941     2     2  ok
  mixed_max                 1956    1004     2     6  ok
```

- **Bytes** is the uplinked file.
- **Memory** is declared guest linear memory, which starts at the reserved guest stack (512 B by default) and grows
  with the sequence's data.
- **Heap** and **Code** are interpreter-side pages: the store and the compiled intermediate representation.

A module can be small on the wire and still fail to load if it overruns one of the others, so gate on `verify`
(`--json` for CI) rather than on file size alone.

## Baseline size

|                | Standard (`.bin`) | Wasm (`.wasm`) |
| -------------- | ----------------: | -------------: |
| Empty sequence |              15 B |           50 B |

A standard sequence's 15 bytes are an 11-byte header and a 4-byte CRC. A Wasm sequence's 50 bytes are the module
scaffolding and an entry point that does nothing. Everything else on this page reads as a marginal cost over these.

## Command sizing

One command with constant arguments, nothing else, for each argument shape the dictionary offers. The Wasm column is
split into the module's code and data sections, because which one grows is the whole story:

| Arguments                               | Example command            | Standard (`.bin`) | Wasm (`.wasm`) | Code | Data |
| --------------------------------------- | -------------------------- | ----------------: | -------------: | ---: | ---: |
| None (opcode only)                      | `CMD_NO_OP`                |              34 B |          122 B | 19 B |  8 B |
| One `bool`                              | `SEND_BOOL`                |              35 B |          126 B | 19 B | 12 B |
| One enum                                | `CHOICE`                   |              38 B |          129 B | 19 B | 15 B |
| One aliased scalar                      | `SEND_ALIAS`               |              38 B |          129 B | 19 B | 15 B |
| One 64-bit-ranged enum                  | `SEND_WIDE_CHOICE`         |              42 B |          129 B | 19 B | 15 B |
| `F32` + `F64`                           | `SEND_FLOATS`              |              46 B |          131 B | 19 B | 17 B |
| Three mixed scalars                     | `CMD_TEST_CMD_1`           |              43 B |          134 B | 19 B | 20 B |
| Eight integers, `I8` through `U64`      | `SEND_INTS`                |              64 B |          155 B | 19 B | 41 B |
| Array of 2 `bool`                       | `SEND_BOOL_ARRAY`          |              36 B |          126 B | 19 B | 12 B |
| Array of 2 enums                        | `CHOICES`                  |              42 B |          133 B | 19 B | 19 B |
| Array of 3 `F32`                        | `SEND_FLOAT_ARRAY`         |              46 B |          135 B | 19 B | 21 B |
| Array of 5 `U32`                        | `SEND_U32_ARRAY`           |              54 B |          145 B | 19 B | 31 B |
| 2×2 array of enums                      | `SEND_2D`                  |              50 B |          137 B | 19 B | 23 B |
| Struct of 2 enums                       | `CHOICE_PAIR`              |              42 B |          133 B | 19 B | 19 B |
| Scalar + struct + scalar                | `CHOICE_PAIR_WITH_FRIENDS` |              44 B |          135 B | 19 B | 21 B |
| Struct of nested arrays and a struct    | `GLUTTON_OF_CHOICE`        |              64 B |          155 B | 19 B | 41 B |
| Struct of 10 scalars                    | `SEND_SCALARS`             |              76 B |          161 B | 19 B | 47 B |
| 4-character literal, `string size 8`    | `SEND_TINY_STRING`         |              40 B |          131 B | 19 B | 17 B |
| 23-character literal, `string size 40`  | `CMD_NO_OP_STRING`         |              59 B |          150 B | 19 B | 36 B |
| 23-character literal, `string size 240` | `LOAD`                     |              59 B |          150 B | 19 B | 36 B |

### Size Breakdown

A standard sequence stores one record per command. Every record carries the same fixed **19 bytes**, then the
serialized arguments on top:

- 1-byte record descriptor
- 8-byte time tag
- 4-byte record size
- 2-byte packet descriptor
- 4-byte opcode (really it's an `FwOpcodeType` which could be any type but it's typically 4 bytes)

A Wasm sequence does not encode with fixed records like the standard sequencer does.
It has sections with different purposes. The "code" section is the executable instructions that tell the
interpreter what to do in order. The "data" section includes bytes to include in the interpreter's memory
that the code can point to.

1. **Compile-time serialization** When every argument to a command is a compile-time
   constant, the whole command is encoded into a fixed byte buffer in the module's data section.
   In standard sequencing, all commands have constant arguments.
2. **Code cost does not vary.** Every row above costs the same 19 bytes of code, from a bare opcode to a
   struct of ten scalars. Only the data section grows with the serialized bytes.

This is the property to design around: **constant arguments are nearly free.** What
costs code is computing them at run time. See [Runtime arguments](#runtime-arguments).

## Incremental sizing

The marginal cost of one more command:

| Added command                                    | Standard (`.bin`) | Wasm (`.wasm`) |
| ------------------------------------------------ | ----------------: | -------------: |
| A repeat of a command already in the sequence    |   +19 B ... +28 B |         ~ +3 B |
| A distinct `Dp` (enum), `U32` (9 B of arguments) |             +28 B |        ~ +22 B |
| A distinct string command (~11-character string) |             +32 B |        ~ +25 B |

A standard sequence has no notion of a repeat: the tenth identical command costs another full record. A Wasm sequence
merges identical constant buffers to one address, so a repeat costs only the instructions to call it again.

Measured totals behind those margins:

| Sequence                    | Standard (`.bin`) | Wasm (`.wasm`) |
| --------------------------- | ----------------: | -------------: |
| 8x identical `Dp`           |             239 B |          145 B |
| 8x distinct `Dp`            |             239 B |          283 B |
| 16x distinct `Dp`           |             463 B |          463 B |
| 8x distinct string commands |             271 B |          325 B |

Because the Wasm marginal cost per command is *lower* than the standard one, the two curves cross:

- Around **5 commands** if the sequence repeats itself.
- Around **16 commands** if every command is distinct.

Past that point the Wasm module is the smaller artifact, and it stays smaller as the sequence grows. For a handful of
one-shot commands, a standard sequence is smaller.

For the size of every benchmark sequence, see the [size comment][size-comment] CI posts on every `fprime-wasm` pull
request. It carries the whole table, each row linked to the sequence's source, measured the same way as the
figures on this page.

## Runtime arguments

> [!NOTE]
> The rest of this document does not compare standard with advanced sequencing as these
> features are only supported in advanced sequencing.

An argument that is *not* a compile-time constant cannot be encoded ahead of time. The module links the runtime
serializer for that argument's type instead, and pays for it in code:

| Benchmark                                          |  Total |   Code | What makes it a runtime argument     |
| -------------------------------------------------- | -----: | -----: | ------------------------------------ |
| [`cmd_no_args`][cmd_no_args]                       |  122 B |   19 B | *(constant, for reference)*          |
| [`fallback_scalar`][fallback_scalar]               |  244 B |  125 B | a scalar read from telemetry         |
| [`fallback_string`][fallback_string]               |  266 B |  146 B | a string chosen between two literals |
| [`fallback_struct`][fallback_struct]               |  293 B |  173 B | a structure built at run time        |
| [`fallback_struct_string`][fallback_struct_string] | 1094 B |  975 B | a structure with a string member     |
| [`fallback_nested_string`][fallback_nested_string] | 1294 B | 1163 B | an array of strings                  |

Two things to take from this table. A scalar or a whole structure computed at run time costs a hundred bytes or two.
A **string inside a struct costs a kilobyte**.

## Strings

A string *argument* written as a literal is constant-encoded like any other argument: it costs its own characters and
nothing more, whatever capacity the argument declares (see [Command sizing](#command-sizing)). A literal longer than
its capacity is even truncated at compile time, so it costs only the bytes that will actually fly
([`cmd_string_truncated`][cmd_string_truncated], 135 B).

What is not free is a string the compiler cannot see:

| Benchmark                                          |  Total | Why                                                    |
| -------------------------------------------------- | -----: | ------------------------------------------------------ |
| [`fallback_string`][fallback_string]               |  266 B | chosen at run time, so the string serializer is linked |
| [`serial_struct`][serial_struct]                   |  857 B | a structure sent out a serial port                     |
| [`fallback_struct_string`][fallback_struct_string] | 1094 B | a string member inside a structure                     |
| [`fallback_nested_string`][fallback_nested_string] | 1294 B | an array of strings                                    |
| [`serial_string`][serial_string]                   | 1614 B | a string serialized out a serial port                  |

A string nested in a structure or array is a fixed-capacity buffer rather than a `&str`, so that shape can *never* be
constant-encoded. If a command or a serial message with a string member is on a sequence's hot path for size,
consider whether the string can be an argument in its own right instead of a struct member, or whether the value can be an enum.

## String formatting

Be wary, here be dragons! Using Rust's formatting machinery like `print_message!`,
the `format!` macro, or any `{}` placeholder links `core::fmt`, which costs about **3.4 KB**.

Reading a channel and reporting its value in an event is the pattern that walks into it:

```rust
let voltage = Ref.power.BatteryVoltage().0;

// That `{voltage}` is the 3.4 KB
print_message!(EventSeverity::WarningHi, "battery at {voltage} V");
```

| Sequence                                         |  Total |
| ------------------------------------------------ | -----: |
| `message(severity, "a literal event message")`   |  118 B |
| `print_message!(severity, "dropped {dropped}")`  | 3518 B |
| `format!(64, "dropped {dropped}")`, then message | 3516 B |
| One float placeholder                            | 3429 B |
| One integer *and* one float in the same message  | 3904 B |
| Four separate formatted messages                 | 3759 B |

Subsequent uses of format strings will not require an additional 3.4 KB.

Cheaper alternatives, best first:

1. Emit one literal per branch, reporting which case it is rather than the number behind it:

   ```rust
   let voltage = Ref.power.BatteryVoltage().0;

   if voltage < 21.5 {
       message(EventSeverity::WarningHi, "battery low");
   } else {
       message(EventSeverity::ActivityLo, "battery nominal");
   }
   ```

2. Send the number as a command argument, so the ground decodes and displays it from the dictionary and it is never
   rendered on board. The argument is not a constant so it costs more than the above (see
   [Runtime arguments](#runtime-arguments)):

   ```rust
   Ref.power.REPORT_MARGIN(voltage);
   ```

3. Split off string formatting into a separate module. Wasm modules can call each other as long as they are `LOAD`-ed in order.
   Certain pre-uplinked modules may include large expensive abstractions which other sequences can utilize.

> [!NOTE]
> A guest event message is truncated to `Wasm.GUEST_EVENT_MESSAGE_SIZE` (128 B by default), so a long formatted
> message pays the code cost and then loses its tail.

## Rules of thumb

- Keep command arguments compile-time constants where the logic allows.
- Repeat commands freely. Duplicate commands are reused.
- Prefer literal event messages. Treat the first `{}` as a 3.4 KB decision.
- Keep strings out of structures and arrays.
- A handful of unconditional commands suits a standard sequence. Reach for an advanced sequence for logic, or past
  roughly a dozen distinct commands.
- Gate on `fprime-wasm verify`, not on file size.

[size-comment]: https://github.com/fprime-community/fprime-wasm/pull/9#issuecomment-5786644972
[cmd_no_args]: https://github.com/fprime-community/fprime-wasm/blob/main/crates/bench/src/bin/cmd_no_args.rs
[cmd_string_truncated]: https://github.com/fprime-community/fprime-wasm/blob/main/crates/bench/src/bin/cmd_string_truncated.rs
[fallback_nested_string]: https://github.com/fprime-community/fprime-wasm/blob/main/crates/bench/src/bin/fallback_nested_string.rs
[fallback_scalar]: https://github.com/fprime-community/fprime-wasm/blob/main/crates/bench/src/bin/fallback_scalar.rs
[fallback_string]: https://github.com/fprime-community/fprime-wasm/blob/main/crates/bench/src/bin/fallback_string.rs
[fallback_struct]: https://github.com/fprime-community/fprime-wasm/blob/main/crates/bench/src/bin/fallback_struct.rs
[fallback_struct_string]: https://github.com/fprime-community/fprime-wasm/blob/main/crates/bench/src/bin/fallback_struct_string.rs
[serial_string]: https://github.com/fprime-community/fprime-wasm/blob/main/crates/bench/src/bin/serial_string.rs
[serial_struct]: https://github.com/fprime-community/fprime-wasm/blob/main/crates/bench/src/bin/serial_struct.rs
