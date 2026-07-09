// ======================================================================
// build.rs
//
// Generates Rust FFI bindings for the C++ -> Rust import surface declared
// in `Svc/WasmSequencer/WasmSequencerFFI.hpp`. These are the functions and
// types that the C++ `Svc::WasmSequencer` component provides and that this
// crate calls into (currently the page allocator hook + its result enum).
//
// The generated bindings are written to `$OUT_DIR/wasmsequencer_ffi.rs` and
// pulled into the crate via `include!` in `src/allocator.rs`, replacing the
// hand-written `extern "C"` block that must otherwise be kept in sync with
// the header by hand.
// ======================================================================

use std::env;
use std::path::PathBuf;

fn main() {
    // The FFI header lives in the parent Svc/WasmSequencer directory.
    let header = "../WasmSequencerFFI.hpp";

    // Rebuild the bindings whenever the header changes.
    println!("cargo:rerun-if-changed={header}");
    println!("cargo:rerun-if-changed=build.rs");

    let bindings = bindgen::Builder::default()
        .header(header)
        // The header is C++ (`enum class`, `extern "C"`); parse it as such.
        .clang_arg("-x")
        .clang_arg("c++")
        .clang_arg("-std=c++17")
        // This crate is `#![no_std]`: emit `core::` paths instead of `std::`.
        .use_core()
        .default_enum_style(bindgen::EnumVariation::Rust {
            non_exhaustive: false,
        })
        // Match the crate edition so generated `unsafe extern` blocks compile.
        .rust_edition(bindgen::RustEdition::Edition2024)
        // Only pull in the symbols we actually consume from the header,
        // keeping incidental `<cstdint>` declarations out of the bindings.
        .allowlist_file("../WasmSequencerFFI.hpp")
        .generate()
        .expect("Unable to generate WasmSequencerFFI bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR not set"));
    bindings
        .write_to_file(out_path.join("wasmsequencer_ffi.rs"))
        .expect("Unable to write WasmSequencerFFI bindings");
}
