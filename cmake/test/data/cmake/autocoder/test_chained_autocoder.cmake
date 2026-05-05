####
# autocoder/test_chained_autocoder.cmake:
#
# Second stage of the chained-autocoder test. Takes the output of
# test_chained_input_autocoder (.chained-input.generated.txt) and produces .chained.txt files.
####
include_guard()
include(autocoder/helpers)

autocoder_setup_for_individual_sources()

####
# Function `test_chained_autocoder_is_supported`:
#
# Support all files with the ".chained-input.generated.txt" suffix (output from
# test_chained_input_autocoder).
####
function(test_chained_autocoder_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix(".chained-input.generated.txt" "${AC_INPUT_FILE}" TRUE)
endfunction(test_chained_autocoder_is_supported)

####
# Function `test_chained_autocoder_setup_autocode`:
#
# Sets up the steps to run the chained autocoder and produce chained files during the build.
# Takes .chained-input.generated.txt files and creates .chained.txt files from them.
#
# AC_INPUT_FILES: list of supported autocoder input files (.chained-input.generated.txt files)
####
function(test_chained_autocoder_setup_autocode MODULE_NAME AC_INPUT_FILE)
    # Set up generated sources list - convert .chained-input.generated.txt to .chained.txt
    get_filename_component(BASENAME "${AC_INPUT_FILE}" NAME_WE)
    # Remove the .chained-input.generated part to get the base name
    string(REPLACE ".chained-input.generated" "" BASE_NAME "${BASENAME}")
    set(GENERATED_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${BASE_NAME}.chained.txt")

    # This chained autocoder processes the .chained-input.generated.txt file and creates a .chained.txt file
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
