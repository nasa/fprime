####
# autocoder/test_target_autocoder.cmake:
#
# Tests that a target-triggered autocoder runs as expected.
####
include_guard()
include(autocoder/helpers)

autocoder_setup_for_individual_sources()

####
# Function `test_target_autocoder_is_supported`:
#
# Support all files with the ".test-build.txt" suffix 
####
function(test_target_autocoder_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix(".test-target.txt" "${AC_INPUT_FILE}" TRUE)
endfunction(test_target_autocoder_is_supported)

####
# Function `test_target_autocoder_setup_autocode`:
#
# Sets up the steps to run the autocoder and produce the files during the build.
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(test_target_autocoder_setup_autocode MODULE_NAME AC_INPUT_FILE)
    # Set up generated sources list - add .generated suffix to avoid name collision
    get_filename_component(BASENAME "${AC_INPUT_FILE}" NAME_WE)
    set(GENERATED_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.test-target.generated.txt")

    # This autocoder copies the input file and adds .generated suffix
    add_custom_command(
        OUTPUT ${GENERATED_SOURCE}
        COMMAND "${CMAKE_COMMAND}" -E copy ${AC_INPUT_FILE} "${GENERATED_SOURCE}"
        DEPENDS ${AC_INPUT_FILE}
        COMMENT "Generating target file from ${AC_INPUT_FILE}"
    )

    # Generate files, mark them as build sources
    set(AUTOCODER_GENERATED_OTHER "${GENERATED_SOURCE}" PARENT_SCOPE)
    set(AUTOCODER_GENERATED_AUTOCODER_INPUTS "${GENERATED_SOURCE}" PARENT_SCOPE)
endfunction(test_target_autocoder_setup_autocode)
