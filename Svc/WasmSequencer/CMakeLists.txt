####
# F Prime CMakeLists.txt:
#
# SOURCES: list of source files (to be compiled)
# AUTOCODER_INPUTS: list of files to be passed to the autocoders
# DEPENDS: list of libraries that this module depends on
#
# More information in the F´ CMake API documentation:
# https://fprime.jpl.nasa.gov/latest/docs/reference/api/cmake/API/
#
####

# Module names are derived from the path from the nearest project/library/framework
# root when not specifically overridden by the developer, i.e. the module defined by
# `MyProj/Some/Path/CMakeLists.txt` will be named `MyProj_Some_Path`.

####
# fprime_spacewasm: thin Rust static library re-exporting the published
# `spacewasm_c_api` crate (the C ABI over the `spacewasm` WebAssembly
# interpreter). Built by cargo and consumed by the C++ component below as an
# IMPORTED static library. The C API surface is the vendored `spacewasm.h`.
####
set(FPRIME_SPACEWASM_DIR  "${CMAKE_CURRENT_LIST_DIR}/fprime_spacewasm")
set(FPRIME_SPACEWASM_TDIR "${CMAKE_CURRENT_BINARY_DIR}/fprime_spacewasm_target")

# Stable, triple-independent location for the built archive. cargo places the
# real `.a` under a target-triple/profile subdirectory whose layout is a cargo
# implementation detail; the build step (below) parses cargo's JSON output to
# find that path and copies it here, so the linker can point at a fixed path
# regardless of the active Rust target triple.
set(FPRIME_SPACEWASM_LIB "${FPRIME_SPACEWASM_TDIR}/libfprime_spacewasm.a")

if (CARGO)
    # The build runs through a `cmake -P` helper: it invokes
    # `cargo build --release --message-format=json`, extracts the archive path
    # from the crate's compiler-artifact record, and copies it to
    # FPRIME_SPACEWASM_LIB. Optimization/codegen flags live in the crate's
    # [profile.release] (opt-level, lto, codegen-units, panic=abort);
    # `--release` selects that profile.
    add_custom_command(
        OUTPUT  "${FPRIME_SPACEWASM_LIB}"
        COMMAND "${CMAKE_COMMAND}"
                -DCARGO=${CARGO}
                -DMANIFEST_PATH=${FPRIME_SPACEWASM_DIR}/Cargo.toml
                -DTARGET_DIR=${FPRIME_SPACEWASM_TDIR}
                -DTARGET_TRIPLE=${RUST_TARGET_TRIPLE}
                -DCRATE_NAME=fprime_spacewasm
                -DOUTPUT_LIB=${FPRIME_SPACEWASM_LIB}
                -P ${CMAKE_CURRENT_LIST_DIR}/cargo_build_staticlib.cmake
        DEPENDS "${FPRIME_SPACEWASM_DIR}/src/lib.rs"
                "${FPRIME_SPACEWASM_DIR}/Cargo.toml"
                "${FPRIME_SPACEWASM_DIR}/Cargo.lock"
        COMMENT "Building fprime_spacewasm Rust static library"
        VERBATIM
    )
endif()

add_custom_target(fprime_spacewasm_build DEPENDS "${FPRIME_SPACEWASM_LIB}")

add_library(fprime_spacewasm STATIC IMPORTED GLOBAL)
add_dependencies(fprime_spacewasm fprime_spacewasm_build)
set_target_properties(fprime_spacewasm PROPERTIES
    IMPORTED_LOCATION "${FPRIME_SPACEWASM_LIB}"
)

register_fprime_library(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencer.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencerHelpers.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencerHost.cpp"
    DEPENDS
)

# FIXME(tumbar) Putting this target in DEPENDS complains because CMake/FPrime CMake
# does not believe it's a library.
if(TARGET Svc_WasmSequencer)
    target_link_libraries(Svc_WasmSequencer PUBLIC fprime_spacewasm)
    # Vendored cbindgen header (spacewasm.h) for `#include "spacewasm.h"`.
    target_include_directories(Svc_WasmSequencer PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/spacewasm_include")
endif()

### Unit Tests ###
# register_fprime_ut(
#     AUTOCODER_INPUTS
#         "${CMAKE_CURRENT_LIST_DIR}/WasmSequencer.fpp"
#     SOURCES
#         "${CMAKE_CURRENT_LIST_DIR}/test/ut/WasmSequencerTestMain.cpp"
#         "${CMAKE_CURRENT_LIST_DIR}/test/ut/WasmSequencerTester.cpp"
#     DEPENDS
#         STest # For rules-based testing
#     UT_AUTO_HELPERS
# )
