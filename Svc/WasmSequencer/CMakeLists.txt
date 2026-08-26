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

set(FPRIME_SPACEWASM_LIB
    "${FPRIME_SPACEWASM_TDIR}/${RUST_TARGET_TRIPLE}/${FPRIME_SPACEWASM_PROFILE}/libfprime_spacewasm.a")

get_property(_fp_cfg_dests GLOBAL PROPERTY FPRIME_CONFIG_DESTINATIONS)
set(WASM_SEQ_CONFIG_DIR "")
foreach(_dest IN LISTS _fp_cfg_dests)
    get_filename_component(_name "${_dest}" NAME)
    if(_name STREQUAL "WasmSequencerSpacewasmConfig.h")
        get_filename_component(WASM_SEQ_CONFIG_DIR "${_dest}" DIRECTORY)
        break()
    endif()
endforeach()
if(NOT WASM_SEQ_CONFIG_DIR)
    fprime_cmake_fatal_error("Could not find WasmSequencerSpacewasmConfig.h config directory")
endif()
set(WASM_SEQ_CONFIG_HEADER "${WASM_SEQ_CONFIG_DIR}/WasmSequencerSpacewasmConfig.h")

if (CARGO)
    # Optimization/codegen flags live in the crate's [profile.release]
    # (opt-level, lto, codegen-units, panic=abort); --release selects it.
    add_custom_command(
        OUTPUT  "${FPRIME_SPACEWASM_LIB}"
        COMMAND "${CMAKE_COMMAND}" -E env "WASM_SEQ_CONFIG_DIR=${WASM_SEQ_CONFIG_DIR}"
                "${CARGO}" build --locked ${FPRIME_SPACEWASM_PROFILE_FLAG}
                --manifest-path "${FPRIME_SPACEWASM_DIR}/Cargo.toml"
                --target        "${RUST_TARGET_TRIPLE}"
                --target-dir    "${FPRIME_SPACEWASM_TDIR}"
        DEPENDS "${FPRIME_SPACEWASM_DIR}/src/lib.rs"
                "${FPRIME_SPACEWASM_DIR}/build.rs"
                "${FPRIME_SPACEWASM_DIR}/Cargo.toml"
                "${FPRIME_SPACEWASM_DIR}/Cargo.lock"
                "${WASM_SEQ_CONFIG_HEADER}"
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
        "${CMAKE_CURRENT_LIST_DIR}/WasmSequencerInterpreter.cpp"
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
        Fw_Test # For Test::UnitTestAssert (non-aborting FW_ASSERT hook)
    UT_AUTO_HELPERS
)

# Point the unit tests at the committed golden WebAssembly modules
# (Svc/WasmSequencer/test/wasm). Read directly from the source tree at runtime;
# no build-time copy is needed.
if(TARGET Svc_WasmSequencer_ut_exe)
    target_compile_definitions(Svc_WasmSequencer_ut_exe PRIVATE
        WASM_SEQ_ASSET_DIR="${CMAKE_CURRENT_LIST_DIR}/test/wasm")
endif()
