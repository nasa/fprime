####
# autocoder/test_chained_input_autocoder.cmake:
#
# First stage of the chained-autocoder test. Consumes ".chained-input.txt" files and emits
# ".chained-input.generated.txt" files for the second stage (test_chained_autocoder) to chain
# off of. Kept distinct from test_target_autocoder so the chained test does not share an
# intermediate output filename with any other test target.
####
include_guard()
include(autocoder/helpers)

autocoder_setup_for_individual_sources()

function(test_chained_input_autocoder_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix(".chained-input.txt" "${AC_INPUT_FILE}" TRUE)
endfunction(test_chained_input_autocoder_is_supported)

function(test_chained_input_autocoder_setup_autocode MODULE_NAME AC_INPUT_FILE)
    get_filename_component(BASENAME "${AC_INPUT_FILE}" NAME_WE)
    set(GENERATED_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}.chained-input.generated.txt")

    add_custom_command(
        OUTPUT ${GENERATED_SOURCE}
        COMMAND "${CMAKE_COMMAND}" -E copy ${AC_INPUT_FILE} "${GENERATED_SOURCE}"
        DEPENDS ${AC_INPUT_FILE}
        COMMENT "Generating chained-input file from ${AC_INPUT_FILE}"
    )

    set(AUTOCODER_GENERATED_OTHER "${GENERATED_SOURCE}" PARENT_SCOPE)
    set(AUTOCODER_GENERATED_AUTOCODER_INPUTS "${GENERATED_SOURCE}" PARENT_SCOPE)
endfunction(test_chained_input_autocoder_setup_autocode)
