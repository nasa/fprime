####
# autocoder/ai-ut-impl:
#
# Unit test template autocoder. Only run when BUILD_TESTING is set. Otherwise it is not registered.
####
include(utilities)
include(autocoder/ai-shared)

# Bail if not testing
if (NOT BUILD_TESTING)
    return()
endif()

# Handles source files individually
set(HANDLES_INDIVIDUAL_SOURCES TRUE)

####
# `is_supported`:
#
# Given a single input file, determines if that input file is processed by this autocoder. Sets the variable named
# IS_SUPPORTED in parent scope to be TRUE if the source file is an AI XML component file or FALSE otherwise. This only
# processes component ai xml files.
#
# AC_INPUT_FILE: filepath for consideration
####
function(is_supported AC_INPUT_FILE)
    set(IS_SUPPORTED FALSE PARENT_SCOPE)
    if (AC_INPUT_FILE MATCHES ".*ComponentAi\\.xml")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    endif()
endfunction (is_supported)

####
# get_generated_files:
#
# This autocoder always generates Tester.cpp, Tester.hpp, and TestMain.cpp.  Sets GENERATED_FILES in parent scope to
# hold this information.
####
function(get_generated_files AC_INPUT_FILE)
    set(GENERATED_FILES ${CMAKE_CURRENT_LIST_DIR}/Tester.cpp ${CMAKE_CURRENT_LIST_DIR}/Tester.hpp ${CMAKE_CURRENT_LIST_DIR}/TestMain.cpp PARENT_SCOPE)
endfunction(get_generated_files)

####
# get_dependencies:
#
# No dependencies, this function is a no-op.
####
function(get_dependencies AC_INPUT_FILE)
endfunction(get_dependencies)

####
# setup_autocode:
#
# Setup the autocoder build commands. This is a required function of a given autocoder implementation.
####
function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    set(EXTRA_COMMANDS ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_LIST_DIR}/TesterBase.hpp ${CMAKE_CURRENT_LIST_DIR}/TesterBase.cpp  ${CMAKE_CURRENT_LIST_DIR}/GTestBase.hpp ${CMAKE_CURRENT_LIST_DIR}/GTestBase.cpp)
    setup_ai_autocode_variant("-u" "${CMAKE_CURRENT_LIST_DIR}" "${EXTRA_COMMANDS}" "${AC_INPUT_FILE}"
                              "${GENERATED_FILES}" "${MODULE_DEPENDENCIES}" "${FILE_DEPENDENCIES}")
endfunction()
