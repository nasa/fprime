####
# autocoder/test.cmake
#
# Test an autocoder integration within the CMake system.
####

# Call to handle individual sources with this autocoder
autocoder_setup_for_individual_sources()

# Supports files ending in TestComponent.fpp
function(test_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("TestComponent.fpp" "${AC_INPUT_FILE}")
endfunction (test_is_supported)

# Super basic autocoder script
function(test_setup_autocode AC_INPUT_FILE)
    set(AUTOCODER_INPUTS "" PARENT_SCOPE)
    set(GENERATED "${CMAKE_BINARY_DIR}/test-ac-1" "${CMAKE_BINARY_DIR}/test-ac-2")
    set(AUTOCODER_SCRIPT ${CMAKE_COMMAND} -E touch ${GENERATED} PARENT_SCOPE)
    set(AUTOCODER_GENERATED "${GENERATED}" PARENT_SCOPE)
endfunction(test_setup_autocode)
