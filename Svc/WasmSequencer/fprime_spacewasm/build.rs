use std::env;
use std::path::PathBuf;

fn default_repo_root() -> String {
    let manifest_dir = env::var("CARGO_MANIFEST_DIR").expect("CARGO_MANIFEST_DIR not set");
    PathBuf::from(manifest_dir)
        .ancestors()
        .nth(3)
        .expect("crate is not nested under <repo>/Svc/WasmSequencer/fprime_spacewasm")
        .to_string_lossy()
        .into_owned()
}

fn main() {
    let fprime_project_root =
        env::var("FPRIME_PROJECT_ROOT").unwrap_or_else(|_| default_repo_root());

    let fprime_framework_path = env::var("FPRIME_FRAMEWORK_PATH")
        .unwrap_or_else(|_| default_repo_root() + "/build-fprime-automatic-native/F-Prime");

    let ffi_header = format!("{fprime_project_root}/Svc/WasmSequencer/WasmSequencerFFI.hpp");
    let cfg_header = format!("{fprime_framework_path}/default/config/WasmSequencerConfig.hpp");

    println!("cargo:rerun-if-changed={ffi_header}");
    println!("cargo:rerun-if-changed={cfg_header}");
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-env-changed=FPRIME_SPACEWASM_BINDGEN_INCLUDES");
    println!("cargo:rerun-if-env-changed=FPRIME_PROJECT_ROOT");
    println!("cargo:rerun-if-env-changed=FPRIME_FRAMEWORK_PATH");

    let mut builder = bindgen::Builder::default()
        .header(ffi_header)
        .header(cfg_header);

    let includes: Vec<String> = env::var("FPRIME_SPACEWASM_BINDGEN_INCLUDES")
        .map(|i| {
            i.split(";")
                .filter(|d| !d.is_empty())
                .map(|f| f.to_string())
                .collect()
        })
        .unwrap_or_else(|_| {
            // Provide some sane defaults
            vec![
                fprime_project_root.clone(),
                fprime_framework_path.clone(),
                format!("{fprime_framework_path}/default"),
                format!("{fprime_framework_path}/../"),
                format!("{fprime_framework_path}/../cmake/platform/unix"),
            ]
        });

    eprintln!("FPRIME_SPACEWASM_BINDGEN_INCLUDES");
    for dir in includes {
        eprintln!("-I{dir}");
        builder = builder.clang_arg(format!("-I{dir}"));
    }

    let bindings = builder
        .clang_arg("-x")
        .clang_arg("c++")
        .clang_arg("-std=c++14")
        // This crate is `#![no_std]`: emit `core::` paths instead of `std::`.
        .use_core()
        .default_enum_style(bindgen::EnumVariation::Rust {
            non_exhaustive: false,
        })
        // Match the crate edition so generated `unsafe extern` blocks compile.
        .rust_edition(bindgen::RustEdition::Edition2024)
        .allowlist_file(".*WasmSequencer.*")
        .allowlist_recursively(true)
        .generate()
        .expect("Unable to generate SpaceWasm FFI bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").expect("OUT_DIR not set"));
    bindings
        .write_to_file(out_path.join("wasmsequencer_ffi.rs"))
        .expect("Unable to write WasmSequencerFFI bindings");
}
