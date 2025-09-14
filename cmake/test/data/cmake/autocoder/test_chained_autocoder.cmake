####
# autocoder/test_chained_autocoder.cmake:
#
# Tests that a chained autocoder runs as expected - takes output from test_target_autocoder
# and creates chained files from them.
####
include_guard()
include(autocoder/helpers)

autocoder_setup_for_individual_sources()

####
# Function `test_chained_autocoder_is_supported`:
#
# Support all files with the ".test-target.generated.txt" suffix (output from test_target_autocoder)
####
function(test_chained_autocoder_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix(".test-target.generated.txt" "${AC_INPUT_FILE}" TRUE)
endfunction(test_chained_autocoder_is_supported)

####
# Function `test_chained_autocoder_setup_autocode`:
#
# Sets up the steps to run the chained autocoder and produce chained files during the build.
# Takes .test-target.generated.txt files and creates .chained.txt files from them.
#
# AC_INPUT_FILES: list of supported autocoder input files (.test-target.generated.txt files)
####
function(test_chained_autocoder_setup_autocode MODULE_NAME AC_INPUT_FILE)
    # Set up generated sources list - convert .test-target.generated.txt to .chained.txt
    get_filename_component(BASENAME "${AC_INPUT_FILE}" NAME_WE)
    # Remove the .test-target.generated part to get the base name
    string(REPLACE ".test-target.generated" "" BASE_NAME "${BASENAME}")
    set(GENERATED_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${BASE_NAME}.chained.txt")

    # This chained autocoder processes the .test-target.generated.txt file and creates a .chained.txt file
    add_custom_command(
        OUTPUT ${GENERATED_SOURCE}
        COMMAND "${CMAKE_COMMAND}" -E echo "Chained from: ${AC_INPUT_FILE}" > "${GENERATED_SOURCE}"
        COMMAND "${CMAKE_COMMAND}" -E echo "Generated at: $(date)" >> "${GENERATED_SOURCE}"
        DEPENDS ${AC_INPUT_FILE}
        COMMENT "Creating chained file from ${AC_INPUT_FILE}"
    )

    # Generate files, mark them as other generated files
    set(AUTOCODER_GENERATED_OTHER "${GENERATED_SOURCE}" PARENT_SCOPE)
endfunction(test_chained_autocoder_setup_autocode)
