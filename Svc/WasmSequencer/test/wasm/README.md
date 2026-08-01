# WasmSequencer golden WASM test set

This directory holds the WebAssembly modules used by the `WasmSequencer` unit tests.

- `src/*.wat` — hand-written WebAssembly text sources (the editable form).
- `*.wasm` — assembled golden binaries, **committed to git**. The unit-test build reads
  these directly (via the `WASM_SEQ_ASSET_DIR` compile definition), so building and
  running the tests does **not** require a WebAssembly toolchain.
- `build_wasm.py` — regenerates the `*.wasm` files from `src/*.wat` and writes two
  hand-crafted invalid binaries. Run it only when the sources change:

  ```
  python3 build_wasm.py
  ```

  Requires `wat2wasm` (WABT) on `PATH`.

## Constraints these modules honor

- **Import module name is `fprime_v1`.** The component registers its host functions under
  the module `fprime_v1` (see `WasmSequencerHost.cpp`). Importing from `fprime` (as the
  stale root-level `example.wasm`/`test.wasm` scratch files do) fails to load — captured
  deliberately in `bad_import_module.wat`.
- **1-byte custom pages.** Guest linear memory is served from a 2048-byte bump pool
  (`GUEST_MEMORY_SIZE` in the WasmSequencer config). Standard 64 KiB pages would overflow
  it, so every module declares `(memory N (pagesize 1))`. `bigmem.wat` intentionally asks
  for more than 2048 pages to exercise the guest-allocator failure path.
- Host function signatures match `WasmSequencerHost.cpp`:
  `exit/panic (i32)`, `event (i32 i32 i32)`, `cmd (i32 i32)->i32`,
  `tlm (i64 i32 i32 i32 i32)->i32`, `prm (i64 i32 i32)->i32`, `rsleep/asleep (i64)`.
  `tlm` requires `time_size == Fw::Time::SERIALIZED_SIZE` (11).

## Invalid binaries (not built from WAT)

- `malformed.wasm` — bad magic bytes; fails at the first decode step.
- `truncated.wasm` — valid magic+version then cut off mid-section; EOF-class decode error.
