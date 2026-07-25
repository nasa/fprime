####
# cargo_build_staticlib.cmake
#
# Run via `cmake -P` from an add_custom_command. Builds a cargo `staticlib`
# crate with `--message-format=json`, extracts the real `.a` path out of the
# crate's `compiler-artifact` message, and copies it to OUTPUT_LIB.
#
# Why: cargo places the archive under a target-triple/profile subdirectory whose
# exact layout is a cargo implementation detail. Rather than reconstruct that
# path in CMake (which breaks whenever the triple or profile changes), we let
# cargo report the artifact path in its JSON stream and copy it to a stable,
# triple-independent OUTPUT_LIB that the linker can point at unconditionally.
#
# Required -D args:
#   CARGO         path to the cargo executable
#   MANIFEST_PATH path to the crate's Cargo.toml
#   TARGET_DIR    cargo --target-dir
#   CRATE_NAME    the [lib] name to match in the JSON stream
#   OUTPUT_LIB    stable destination path for the extracted .a
# Optional:
#   TARGET_TRIPLE Rust target triple; passed as --target when non-empty
####

set(_target_args)
if(TARGET_TRIPLE)
    set(_target_args --target "${TARGET_TRIPLE}")
endif()

# Human-readable diagnostics go to stderr and pass straight through to the
# console; only the JSON records (stdout) are captured here.
execute_process(
    COMMAND "${CARGO}" build --release --message-format=json
            ${_target_args}
            --manifest-path "${MANIFEST_PATH}"
            --target-dir    "${TARGET_DIR}"
    OUTPUT_VARIABLE _cargo_json
    RESULT_VARIABLE _cargo_rc
)
if(NOT _cargo_rc EQUAL 0)
    message(FATAL_ERROR "cargo build failed (exit ${_cargo_rc}) for ${MANIFEST_PATH}")
endif()

# cargo emits newline-delimited JSON objects; walk them and find the
# compiler-artifact for our crate, then pull the `.a` out of its filenames.
string(REPLACE "\n" ";" _lines "${_cargo_json}")
set(_lib "")
foreach(_line IN LISTS _lines)
    if(_line STREQUAL "")
        continue()
    endif()

    string(JSON _reason ERROR_VARIABLE _e GET "${_line}" "reason")
    if(_e OR NOT _reason STREQUAL "compiler-artifact")
        continue()
    endif()

    string(JSON _name ERROR_VARIABLE _e GET "${_line}" "target" "name")
    if(_e OR NOT _name STREQUAL "${CRATE_NAME}")
        continue()
    endif()

    string(JSON _n ERROR_VARIABLE _e LENGTH "${_line}" "filenames")
    if(_e OR _n LESS 1)
        continue()
    endif()

    math(EXPR _last "${_n} - 1")
    foreach(_i RANGE 0 ${_last})
        string(JSON _fn GET "${_line}" "filenames" ${_i})
        if(_fn MATCHES "\\.a$")
            set(_lib "${_fn}")
        endif()
    endforeach()
endforeach()

if(NOT _lib)
    message(FATAL_ERROR
        "cargo produced no .a artifact for crate '${CRATE_NAME}' "
        "(no matching compiler-artifact message in the JSON stream)")
endif()

# copy_if_different preserves the mtime when the archive is unchanged, so the
# downstream link step only re-runs when cargo actually rebuilt the library.
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${_lib}" "${OUTPUT_LIB}"
    RESULT_VARIABLE _cp_rc
)
if(NOT _cp_rc EQUAL 0)
    message(FATAL_ERROR "failed to copy '${_lib}' -> '${OUTPUT_LIB}'")
endif()
