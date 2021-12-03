####
# autocoder/test.cmake
#
# Test an autocoder integration within the CMake system.
####

set(HANDLES_INDIVIDUAL_SOURCES TRUE)

function(is_supported AC_INPUT_FILE)
    set(IS_SUPPORTED FALSE PARENT_SCOPE)
    if (AC_INPUT_FILE MATCHES "TestComponent\\.fpp")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    endif()
endfunction (is_supported)

function(get_generated_files AC_INPUT_FILE)
    set(GENERATED_FILES "${CMAKE_BINARY_DIR}/test-ac-1" "${CMAKE_BINARY_DIR}/test-ac-2" PARENT_SCOPE)
endfunction(get_generated_files)

function(get_dependencies AC_INPUT_FILE)
    set(MODULE_DEPENDENCIES "TEST_FLAG_1" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "TEST_FLAG_2" PARENT_SCOPE)
endfunction(get_dependencies)

function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    # Check file dependencies is correct
    if (NOT MODULE_DEPENDENCIES STREQUAL "TEST_FLAG_1")
        message(FATAL_ERROR "Failed to set module dependencies")
    endif()

    if (NOT FILE_DEPENDENCIES STREQUAL "TEST_FLAG_2")
        message(FATAL_ERROR "Failed to set file dependencies")
    endif()

    add_custom_command(
            OUTPUT  ${GENERATED_FILES}
            COMMAND ${CMAKE_COMMAND} -E touch ${GENERATED_FILES}
    )
endfunction(setup_autocode)
