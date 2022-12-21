####
# autocoder/ai-ut:
#
# Defines functions for the UT autocoder that uses AI XML inputs to generate the TesterBase and GTestBase files. This is
# built on the same shared setup as the other AI autocoders, but only run when `BUILD_TESTING` is set.
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
# `ai_ut_is_supported`:
#
# Required function, processes ComponentAi.xml files.
# `AC_INPUT_FILE` potential input to the autocoder
####
function(ai_ut_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("ComponentAi.xml" "${AC_INPUT_FILE}")
endfunction (ai_ut_is_supported)

####
# `ai_ut_setup_autocode`:
#
# Required function, sets up a custom command to produce TesterBase and GTestBase files.
####
function(ai_ut_setup_autocode AC_INPUT_FILE)
    set(REMOVALS "${CMAKE_CURRENT_BINARY_DIR}/Tester.hpp"
                 "${CMAKE_CURRENT_BINARY_DIR}/Tester.cpp"
                 "${CMAKE_CURRENT_BINARY_DIR}/TestMain.cpp")
    set(AUTOCODER_GENERATED
        "${CMAKE_CURRENT_BINARY_DIR}/TesterBase.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/TesterBase.hpp")

    # GTest flag handling
    if (INCLUDE_GTEST)
        list(APPEND AUTOCODER_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/GTestBase.cpp" "${CMAKE_CURRENT_BINARY_DIR}/GTestBase.hpp")
    else()
        list(APPEND REMOVALS "${CMAKE_CURRENT_BINARY_DIR}/GTestBase.cpp" "${CMAKE_CURRENT_BINARY_DIR}/GTestBase.hpp")
    endif()
    # Extra test helpers file
    if (DEFINED UT_AUTO_HELPERS AND UT_AUTO_HELPERS)
        list(APPEND AUTOCODER_GENERATED "${CMAKE_CURRENT_BINARY_DIR}/TesterHelpers.cpp")
    else()
        list(APPEND REMOVALS "${CMAKE_CURRENT_BINARY_DIR}/TesterHelpers.cpp")
    endif()

    # Get the shared setup for all AI autocoders
    ai_shared_setup("${CMAKE_CURRENT_BINARY_DIR}")
    # Specifically setup the `add_custom_command` call as the requires extra commands to run
    add_custom_command(
        OUTPUT ${AUTOCODER_GENERATED}
        COMMAND ${AI_BASE_SCRIPT} -u "${AC_INPUT_FILE}"
        COMMAND ${CMAKE_COMMAND} -E remove ${REMOVALS}
        DEPENDS "${AC_INPUT_FILE}" "${CODEGEN_TARGET}"
    )
    set(AUTOCODER_GENERATED "${AUTOCODER_GENERATED}" PARENT_SCOPE)
endfunction(ai_ut_setup_autocode)
