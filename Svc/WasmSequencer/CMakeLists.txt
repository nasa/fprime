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

# Resolve the Rust target triple. A cross-compile toolchain may set
# RUST_TARGET_TRIPLE; otherwise fall back to rustc's host triple. Passing an
# explicit --target makes cargo's output path deterministic (see below).
if (NOT RUST_TARGET_TRIPLE)
    execute_process(
        COMMAND rustc -vV
        OUTPUT_VARIABLE _rustc_version
        RESULT_VARIABLE _rustc_rc
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (_rustc_rc EQUAL 0 AND _rustc_version MATCHES "host: ([^ \n]+)")
        set(RUST_TARGET_TRIPLE "${CMAKE_MATCH_1}")
    endif()
endif()

# Select the cargo profile from the CMake build type. The profile name is also
# the subdirectory cargo writes the archive into.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(FPRIME_SPACEWASM_PROFILE "debug")
    set(FPRIME_SPACEWASM_PROFILE_FLAG "")
else()
    set(FPRIME_SPACEWASM_PROFILE "release")
    set(FPRIME_SPACEWASM_PROFILE_FLAG "--release")
endif()

# With an explicit --target and --target-dir, cargo writes the archive to a
# fixed, documented path: <target-dir>/<triple>/<profile>/lib<crate>.a. Point
# the linker straight at it -- no JSON parsing or copy step needed.
set(FPRIME_SPACEWASM_LIB
    "${FPRIME_SPACEWASM_TDIR}/${RUST_TARGET_TRIPLE}/${FPRIME_SPACEWASM_PROFILE}/libfprime_spacewasm.a")

if (CARGO)
    # Optimization/codegen flags live in the crate's [profile.release]
    # (opt-level, lto, codegen-units, panic=abort); --release selects it.
    add_custom_command(
        OUTPUT  "${FPRIME_SPACEWASM_LIB}"
        COMMAND "${CARGO}" build --locked ${FPRIME_SPACEWASM_PROFILE_FLAG}
                --manifest-path "${FPRIME_SPACEWASM_DIR}/Cargo.toml"
                --target        "${RUST_TARGET_TRIPLE}"
                --target-dir    "${FPRIME_SPACEWASM_TDIR}"
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
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencerController.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencerEngine.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencerHelpers.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencerHost.cpp"
    DEPENDS
        Utils_Types # Types::CircularBuffer
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
register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencer.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/WasmSequencerTestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/WasmSequencerTester.cpp"
    DEPENDS
        STest # For rules-based testing
    UT_AUTO_HELPERS
)

# Point the unit tests at the committed golden WebAssembly modules
# (Svc/WasmSequencer/test/wasm). Read directly from the source tree at runtime;
# no build-time copy is needed.
if(TARGET Svc_WasmSequencer_ut_exe)
    target_compile_definitions(Svc_WasmSequencer_ut_exe PRIVATE
        WASM_SEQ_ASSET_DIR="${CMAKE_CURRENT_LIST_DIR}/test/wasm")
endif()
