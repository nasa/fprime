####
# autocoder/test_build_autocoder.cmake:
#
# Tests that a build-triggered autocoder runs as expected.
####
include_guard()
include(autocoder/helpers)

autocoder_setup_for_multiple_sources()

####
# Function `test_header_autocoder_is_supported`:
#
# Support all files with the ".test-build.hpp" suffix 
####
function(test_header_autocoder_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix(".test-build.hpp" "${AC_INPUT_FILE}" TRUE)
endfunction(test_header_autocoder_is_supported)

####
# Function `test_header_autocoder_setup_autocode`:
#
# Sets up the steps to run the autocoder and produce the files during the build.
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(test_header_autocoder_setup_autocode MODULE_NAME AC_INPUT_FILES)
    # Set up generated sources list
    set(GENERATED_SOURCES)
    foreach(AC_INPUT IN LISTS AC_INPUT_FILES)
        get_filename_component(BASENAME "${AC_INPUT}" NAME)
        list(APPEND GENERATED_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}")
    endforeach()

    # This autocoder just touches files
    add_custom_command(
        OUTPUT ${GENERATED_SOURCES}
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/header-dir"
        COMMAND "${CMAKE_COMMAND}" -E copy ${AC_INPUT_FILES} "${CMAKE_CURRENT_BINARY_DIR}/header-dir"
    )

    # Generate files, mark them as build sources
    set(AUTOCODER_GENERATED_BUILD_SOURCES "${GENERATED_SOURCES}" PARENT_SCOPE)
endfunction(test_header_autocoder_setup_autocode)
