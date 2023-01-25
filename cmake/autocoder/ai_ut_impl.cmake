####
# autocoder/ai-ut-impl:
#
# Unit test template autocoder. Only run when BUILD_TESTING is set. Otherwise it is not registered. Generates the
# Tester and TestMain files as templates for autocoding.
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
# `ai_ut_impl_is_supported`:
#
# Required function, processes ComponentAi.xml files.
# `AC_INPUT_FILE` potential input to the autocoder
####
function(ai_ut_impl_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("ComponentAi.xml" "${AC_INPUT_FILE}")
endfunction (ai_ut_impl_is_supported)

####
# `ai_ut_impl_setup_autocode`:
#
# Required function, sets up a custom command to produce Tester and TestMain files.
####
function(ai_ut_impl_setup_autocode AC_INPUT_FILE)
    set(AUTOCODER_GENERATED
        ${CMAKE_CURRENT_SOURCE_DIR}/Tester.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tester.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TestMain.cpp
    )
    set(REMOVALS
        "${CMAKE_CURRENT_SOURCE_DIR}/TesterBase.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/TesterBase.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/GTestBase.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/GTestBase.cpp")
    # Extra test helpers file
    if (NOT DEFINED UT_AUTO_HELPERS OR NOT UT_AUTO_HELPERS)
        list(APPEND AUTOCODER_GENERATED "${CMAKE_CURRENT_SOURCE_DIR}/TesterHelpers.cpp")
    else()
        list(APPEND REMOVALS "${CMAKE_CURRENT_SOURCE_DIR}/TesterHelpers.cpp")
    endif()

    # Get the shared setup for all AI autocoders
    ai_shared_setup("${CMAKE_CURRENT_SOURCE_DIR}")
    # Specifically setup the `add_custom_command` call as the requires extra commands to run
    add_custom_command(
            OUTPUT ${AUTOCODER_GENERATED}
            COMMAND ${AI_BASE_SCRIPT} -u "${AC_INPUT_FILE}"
            COMMAND ${CMAKE_COMMAND} -E remove ${REMOVALS}
            DEPENDS "${AC_INPUT_FILE}" "${CODEGEN_TARGET}"
    )
    set(AUTOCODER_GENERATED "${AUTOCODER_GENERATED}" PARENT_SCOPE)
endfunction()
