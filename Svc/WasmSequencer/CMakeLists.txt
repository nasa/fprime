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
# fprime_spacewasm: Rust static library (C ABI wrapper over the `spacewasm`
# WebAssembly interpreter). Built by cargo and consumed by the C++ component
# below as an IMPORTED static library.
####
set(FPRIME_SPACEWASM_DIR  "${CMAKE_CURRENT_LIST_DIR}/fprime_spacewasm")
set(FPRIME_SPACEWASM_TDIR "${CMAKE_CURRENT_BINARY_DIR}/fprime_spacewasm_target")

find_program(CARGO_EXECUTABLE cargo REQUIRED)

####
# Map the active CMake/F Prime toolchain to a Rust target triple.
#
# TODO(tumbar) This is definitely not the proper way to do it...
####
set(FPRIME_SPACEWASM_RUST_TARGET "" CACHE STRING
    "Rust target triple for the fprime_spacewasm crate (empty = cargo host default)")

if(NOT FPRIME_SPACEWASM_RUST_TARGET AND CMAKE_CROSSCOMPILING)
    # Derive a bare-metal (no-OS) triple from the toolchain's processor.
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
        set(FPRIME_SPACEWASM_RUST_TARGET "aarch64-unknown-none")
    elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
        # ARM_TOOL_SUFFIX is set by the F Prime ARM toolchains: eabihf vs eabi.
        if(ARM_TOOL_SUFFIX STREQUAL "eabihf")
            set(FPRIME_SPACEWASM_RUST_TARGET "armv7a-none-eabihf")
        else()
            set(FPRIME_SPACEWASM_RUST_TARGET "armv7a-none-eabi")
        endif()
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|AMD64")
        set(FPRIME_SPACEWASM_RUST_TARGET "x86_64-unknown-none")
    endif()
    if(NOT FPRIME_SPACEWASM_RUST_TARGET)
        message(FATAL_ERROR
            "fprime_spacewasm: no Rust target triple mapping for "
            "PROCESSOR='${CMAKE_SYSTEM_PROCESSOR}'. "
            "Set -DFPRIME_SPACEWASM_RUST_TARGET=<triple> explicitly.")
    endif()
endif()

# Assemble the cargo `--target` flag and the matching artifact subdirectory.
if(FPRIME_SPACEWASM_RUST_TARGET)
    set(FPRIME_SPACEWASM_CARGO_TARGET_ARG --target "${FPRIME_SPACEWASM_RUST_TARGET}")
    set(FPRIME_SPACEWASM_LIB
        "${FPRIME_SPACEWASM_TDIR}/${FPRIME_SPACEWASM_RUST_TARGET}/release/libfprime_spacewasm.a")
    message(STATUS "fprime_spacewasm: cross-compiling for Rust target '${FPRIME_SPACEWASM_RUST_TARGET}'")
else()
    set(FPRIME_SPACEWASM_CARGO_TARGET_ARG)
    set(FPRIME_SPACEWASM_LIB "${FPRIME_SPACEWASM_TDIR}/release/libfprime_spacewasm.a")
endif()

# Optimization/codegen flags live in the crate's [profile.release] (opt-level,
# lto, codegen-units, panic=abort). `--release` selects that profile here.
add_custom_command(
    OUTPUT  "${FPRIME_SPACEWASM_LIB}"
    COMMAND "${CARGO_EXECUTABLE}" build --release
            ${FPRIME_SPACEWASM_CARGO_TARGET_ARG}
            --manifest-path "${FPRIME_SPACEWASM_DIR}/Cargo.toml"
            --target-dir    "${FPRIME_SPACEWASM_TDIR}"
    DEPENDS "${FPRIME_SPACEWASM_DIR}/src/lib.rs"
            "${FPRIME_SPACEWASM_DIR}/Cargo.toml"
            "${FPRIME_SPACEWASM_DIR}/build.rs"
            "${CMAKE_CURRENT_LIST_DIR}/WasmSequencerFFI.hpp"
    COMMENT "Building fprime_spacewasm Rust static library (spacewasm)"
    VERBATIM
)
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
    DEPENDS
)

# FIXME(tumbar) Putting this target in DEPENDS complains because CMake/FPrime CMake
# does not believe it's a library.
if(TARGET Svc_WasmSequencer)
    target_link_libraries(Svc_WasmSequencer PUBLIC fprime_spacewasm)
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
