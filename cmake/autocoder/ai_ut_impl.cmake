####
# autocoder/ai-ut-impl:
#
# Unit test template autocoder. Only run when BUILD_TESTING is set. Otherwise it is not registered.
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
function(ai_ut_impl_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("ComponentAi.xml" "${AC_INPUT_FILE}")
endfunction (ai_ut_impl_is_supported)

####
# setup_autocode:
#
# Setup the autocoder build commands. This is a required function of a given autocoder implementation.
####
function(ai_ut_impl_setup_autocode AC_INPUT_FILE)
    set(AUTOCODER_GENERATED
        ${CMAKE_CURRENT_SOURCE_DIR}/Tester.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tester.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/TestMain.cpp
    )
    # Get the shared setup for all AI autocoders
    ai_shared_setup("${CMAKE_CURRENT_SOURCE_DIR}")
    # Specifically setup the `add_custom_command` call as the requires extra commands to run
    add_custom_command(
            OUTPUT ${AUTOCODER_GENERATED}
            COMMAND ${AI_BASE_SCRIPT} -u "${AC_INPUT_FILE}"
            COMMAND ${CMAKE_COMMAND} -E remove
                ${CMAKE_CURRENT_SOURCE_DIR}/TesterBase.hpp
                ${CMAKE_CURRENT_SOURCE_DIR}/TesterBase.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/GTestBase.hpp
                ${CMAKE_CURRENT_SOURCE_DIR}/GTestBase.cpp
            DEPENDS "${AC_INPUT_FILE}"
    )
    set(AUTOCODER_GENERATED "${AUTOCODER_GENERATED}" PARENT_SCOPE)
endfunction()
