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
# get_generated_files:
#
# This autocoder always generates TesterBase.cpp, TesterBase.hpp, and GTestBase.{c|h}pp files when INCLUDE_GTEST is set.
# Sets GENERATED_FILES in parent scope to hold this information.
####
function(ai_ut_get_generated_files AC_INPUT_FILE)
    set(GENERATED_FILES ${CMAKE_CURRENT_BINARY_DIR}/TesterBase.cpp ${CMAKE_CURRENT_BINARY_DIR}/TesterBase.hpp)
    # GTest flag handling
    if (INCLUDE_GTEST)
        list(APPEND GENERATED_FILES ${CMAKE_CURRENT_BINARY_DIR}/GTestBase.cpp ${CMAKE_CURRENT_BINARY_DIR}/GTestBase.hpp)
    endif()
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
endfunction(ai_ut_get_generated_files)

####
# get_dependencies:
#
# No dependencies, this function is a no-op.
####
function(ai_ut_get_dependencies AC_INPUT_FILE)
endfunction(ai_ut_get_dependencies)

####
# setup_autocode:
#
# Setup the autocoder build commands. This is a required function of a given autocoder implementation.
####
function(ai_ut_setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    set(REMOVAL_LIST "${CMAKE_CURRENT_BINARY_DIR}/Tester.hpp ${CMAKE_CURRENT_BINARY_DIR}/Tester.cpp ${CMAKE_CURRENT_BINARY_DIR}/TestMain.cpp")
    if (NOT INCLUDE_GTEST)
        set(REMOVAL_LIST "${REMOVAL_LIST} ${CMAKE_CURRENT_BINARY_DIR}/GTestBase.cpp ${CMAKE_CURRENT_BINARY_DIR}/GTestBase.hpp")
    endif()
    set(EXTRA_COMMANDS ${CMAKE_COMMAND} -E remove ${REMOVAL_LIST})
    setup_ai_autocode_variant("-u" "${CMAKE_CURRENT_BINARY_DIR}" "${EXTRA_COMMANDS}" "${AC_INPUT_FILE}"
                              "${GENERATED_FILES}" "${MODULE_DEPENDENCIES}" "${FILE_DEPENDENCIES}")
endfunction(ai_ut_setup_autocode)
