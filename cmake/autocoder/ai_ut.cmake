####
# autocoder/ai-ut:
#
# Unit test support files autocoder. Only run when BUILD_TESTING is set. Otherwise it is not registered.
####
include(utilities)
include(autocoder/helpers)
include(autocoder/ai-shared)



# Bail if not testing
if (NOT BUILD_TESTING)
    return()
endif()


autocoder_setup_for_individual_sources()
####
# `is_supported`:
#
# Given a single input file, determines if that input file is processed by this autocoder. Sets the variable named
# IS_SUPPORTED in parent scope to be TRUE if the source file is an AI XML component file or FALSE otherwise. This only
# processes component ai xml files.
#
# AC_INPUT_FILE: filepath for consideration
####
function(ai_ut_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("ComponentAi.xml" "${AC_INPUT_FILE}")
endfunction (ai_ut_is_supported)

####
# ai_ut_setup:
#
# Sets up the autocoder to generate UT files into the binary directory. This is done such that the UTs can build with
# a complete unit test framework.
####
function(ai_ut_setup_autocode AC_INPUT_FILE)
    set(REMOVAL_LIST "${CMAKE_CURRENT_BINARY_DIR}/Tester.hpp ${CMAKE_CURRENT_BINARY_DIR}/Tester.cpp ${CMAKE_CURRENT_BINARY_DIR}/TestMain.cpp")
    set(AUTOCODER_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/TesterBase.cpp" "${CMAKE_CURRENT_BINARY_DIR}/TesterBase.hpp")
    # GTest flag handling
    if (INCLUDE_GTEST)
        list(APPEND AUTOCODER_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/GTestBase.cpp" "${CMAKE_CURRENT_BINARY_DIR}/GTestBase.hpp")
    else()
        set(REMOVAL_LIST "${REMOVAL_LIST} ${CMAKE_CURRENT_BINARY_DIR}/GTestBase.cpp ${CMAKE_CURRENT_BINARY_DIR}/GTestBase.hpp")
    endif()

    # Get the shared setup for all AI autocoders
    ai_shared_setup("${CMAKE_CURRENT_BINARY_DIR}")
    # Specifically setup the `add_custom_command` call as the requires extra commands to run
    add_custom_command(
        OUTPUT ${AUTOCODER_GENERATED}
        COMMAND ${AI_BASE_SCRIPT} -u "${AC_INPUT_FILE}"
        COMMAND ${CMAKE_COMMAND} -E remove ${REMOVAL_LIST}
        DEPENDS "${AC_INPUT_FILE}"
    )
    set(AUTOCODER_GENERATED "${AUTOCODER_GENERATED}" PARENT_SCOPE)
endfunction(ai_ut_setup_autocode)
