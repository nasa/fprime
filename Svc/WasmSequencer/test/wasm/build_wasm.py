#!/usr/bin/env python3
# ======================================================================
# build_wasm.py
#
# Regenerates the golden WebAssembly test modules for the WasmSequencer
# unit tests from the WAT sources in ./src, plus two hand-crafted invalid
# binaries used by the load-failure tests.
#
# The generated *.wasm files are COMMITTED to git (a golden set) so the
# unit-test build does not require a WebAssembly toolchain. Only run this
# script when the WAT sources change.
#
# Requirements: `wat2wasm` (from WABT) on PATH, built with custom
# page-size support. Guest modules use 1-byte custom pages so their linear
# memory fits the WasmSequencer's 2048-byte guest pool
# (Svc/WasmSequencer default config GUEST_MEMORY_SIZE).
#
# Usage:  python3 build_wasm.py
# ======================================================================

import os
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(HERE, "src")


def build_wat():
    wat2wasm = shutil.which("wat2wasm")
    if wat2wasm is None:
        print("ERROR: wat2wasm not found on PATH (install WABT).", file=sys.stderr)
        return False

    ok = True
    for name in sorted(os.listdir(SRC)):
        if not name.endswith(".wat"):
            continue
        src_path = os.path.join(SRC, name)
        out_path = os.path.join(HERE, name[:-4] + ".wasm")
        cmd = [wat2wasm, "--enable-custom-page-sizes", src_path, "-o", out_path]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"FAIL {name}:\n{result.stderr}", file=sys.stderr)
            ok = False
        else:
            size = os.path.getsize(out_path)
            print(f"  {name} -> {os.path.basename(out_path)} ({size} bytes)")
    return ok


def build_invalid():
    # malformed.wasm: bad magic/version -- fails at the very first decode step.
    malformed = os.path.join(HERE, "malformed.wasm")
    with open(malformed, "wb") as f:
        f.write(b"\x00not-a-wasm-module\x00")
    print(f"  (hand-crafted) -> malformed.wasm ({os.path.getsize(malformed)} bytes)")

    # truncated.wasm: valid magic + version, then cut off mid-stream -> EOF-class
    # decode error.
    truncated = os.path.join(HERE, "truncated.wasm")
    with open(truncated, "wb") as f:
        f.write(b"\x00asm")          # magic
        f.write(b"\x01\x00\x00\x00")  # version 1
        f.write(b"\x01\x07\x01")      # start of a type section, declared len 7, then EOF
    print(f"  (hand-crafted) -> truncated.wasm ({os.path.getsize(truncated)} bytes)")


def main():
    print("Assembling WAT sources:")
    ok = build_wat()
    print("Writing hand-crafted invalid binaries:")
    build_invalid()
    if not ok:
        sys.exit(1)
    print("Done.")


if __name__ == "__main__":
    main()
