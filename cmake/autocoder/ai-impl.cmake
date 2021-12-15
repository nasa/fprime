####
# autocoder/ai-impl:
#
# Implementation template autocoder file.
####
include(utilities)
include(autocoder/ai-shared)

# Handles singular source files.
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
# This autocoder always generates TesterBase.cpp, TesterBase.hpp, and GTestBase.{c|h}pp files when INCLUDE_GTEST is set.
# Sets GENERATED_FILES in parent scope to hold this information.
####
function(get_generated_files AC_INPUT_FILE)
    get_filename_component(AC_INPUT_NAME "${AC_INPUT_FILE}" NAME)
    foreach(AI_TYPE IN ITEMS "Component" "Port" "Enum" "Serializable" "Array" "TopologyApp")
        if (AC_INPUT_NAME MATCHES ".*${AI_TYPE}Ai.xml$")
            set(XML_TYPE "${AI_TYPE}")
            break()
        endif()
    endforeach()
    # Required check of output of Ai.xml generator
    if (NOT XML_TYPE)
        message(FATAL_ERROR "[Autocode/ai-xml] Cannot support Ai file of name ${AC_INPUT_NAME}")
    endif()

    string(REGEX REPLACE "${XML_TYPE}Ai\\.xml" "" AC_OBJ_NAME "${AC_INPUT_NAME}")
    string(TOLOWER ${XML_TYPE} XML_LOWER_TYPE)
    set(GENERATED_FILES "${CMAKE_CURRENT_LIST_DIR}/${AC_OBJ_NAME}${XML_TYPE}Impl.hpp-template"
                        "${CMAKE_CURRENT_LIST_DIR}/${AC_OBJ_NAME}${XML_TYPE}Impl.cpp-template")
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
endfunction(get_generated_files)

####
# get_dependencies:
#
# No dependencies, this function is a no-op.
####
function(get_dependencies AC_INPUT_FILE)
endfunction(get_dependencies)

####
# `setup_autocode`:
#
# Sets up the AI XML autocoder to generate files.
####
function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    setup_ai_autocode_variant("-t" "${CMAKE_CURRENT_LIST_DIR}" "" "${AC_INPUT_FILE}" "${GENERATED_FILES}"
                              "${MODULE_DEPENDENCIES}" "${FILE_DEPENDENCIES}")
endfunction(setup_autocode)
