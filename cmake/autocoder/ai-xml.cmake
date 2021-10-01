include(utilities)
include(autocoder/ai-shared)

set(HANDLES_INDIVIDUAL_SOURCES TRUE)
function(is_supported AC_INPUT_FILE)
    if (AC_INPUT_FILE MATCHES ".*Ai\\.xml")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    else()
        set(IS_SUPPORTED FALSE PARENT_SCOPE)
    endif()
endfunction (is_supported)

function(get_generated_files AC_INPUT_FILE)

    # Ai.xml files can come from multiple places. One is as a generated file from within the BINARY directory and the
    # other as a source file input.  Generated files will follow a convention and would not generate dependencies.
    if (AC_INPUT_FILE MATCHES "^${CMAKE_CURRENT_BINARY_DIR}.*")
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

        # Generating autocode must setup the dependency information
        set(MODULE_DEPENDENCIES)
        set(FILE_DEPENDENCIES)
    else()
        __ai_info("${AC_INPUT_FILE}" "${MODULE_NAME}")
    endif()

    # Share with other functions without polluting cache
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)

    set(GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}${XML_TYPE}Ac.hpp"
                        "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}${XML_TYPE}Ac.cpp")

    # Topology also builds dictionary
    if (XML_LOWER_TYPE STREQUAL "topologyapp")
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}${XML_TYPE}Dictionary.xml")
    endif()

    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
    set(EXTRAS "${XML_LOWER_TYPE}" PARENT_SCOPE)
endfunction(get_generated_files)

function(get_dependencies AC_INPUT_FILE)
    # Should have been inherited from previous call to `get_generated_files`
    if (NOT DEFINED MODULE_DEPENDENCIES OR NOT DEFINED FILE_DEPENDENCIES)
        message(FATAL "The CMake system is inconsistent. Please contact a developer.")
    endif()
endfunction(get_dependencies)



####
# Function `__ai_info`:
#
# A function used to detect all the needed information for an Ai.xml file. This looks for the following items:
#  1. Type of object defined inside: Component, Port, Enum, Serializable, TopologyApp
#  2. All fprime module dependencies that may be auto-detected
#  3. All file dependencies
#
# - **XML_PATH:** full path to the XML used for sources.
# - **MODULE_NAME:** name of the module soliciting new dependencies
####
function(__ai_info XML_PATH MODULE_NAME)
    find_program(PYTHON NAMES python3 python)
    # Run the parser and capture the output. If an error occurs, that fatals CMake as we cannot continue
    set(MODULE_NAME_NO_SUFFIX "${MODULE_NAME}")
    set(PARSER_PATH "${FPRIME_FRAMEWORK_PATH}/cmake/autocoder/ai-parser/ai_parser.py")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${PARSER_PATH}")
    execute_process(
            COMMAND "${PYTHON}" "${PARSER_PATH}" "${XML_PATH}" "${MODULE_NAME_NO_SUFFIX}" "${FPRIME_CLOSEST_BUILD_ROOT}"
            RESULT_VARIABLE ERR_RETURN
            OUTPUT_VARIABLE AI_OUTPUT
    )
    if(ERR_RETURN)
        message(FATAL_ERROR "Failed to parse ${XML_PATH}. ${ERR_RETURN}")
    endif()

    # Next parse the output matching one line at a time
    read_from_lines("${AI_OUTPUT}" XML_TYPE MODULE_DEPENDENCIES FILE_DEPENDENCIES)

    # Next compute the needed variants of the items needed. This
    string(TOLOWER ${XML_TYPE} XML_LOWER_TYPE)
    get_filename_component(XML_NAME "${XML_PATH}" NAME)
    string(REGEX REPLACE "(${XML_TYPE})?Ai.xml" "" AC_OBJ_NAME "${XML_NAME}")

    # Finally, set all variables into parent scope
    set(XML_TYPE "${XML_TYPE}" PARENT_SCOPE)
    set(XML_LOWER_TYPE "${XML_LOWER_TYPE}" PARENT_SCOPE)
    set(AC_OBJ_NAME "${AC_OBJ_NAME}" PARENT_SCOPE)
    set(MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)
endfunction(__ai_info)



####
# Function `aiwrap`:
#
# This function wraps the actual call to the ai autocoder in order to ensure that the functions are
# performed correctly. This replaces a wrapper shell in order to step toward Windows support. This
# function registers the autocoding steps specific to `codegen.py`.
#
# Note: as the autocoder is rewritten, this will likely need to change.
#
# - **AC_INPUT_FILE:** AI xml input to autocoder
# - **MODULE_NAME:** module name being auto-coded
# - **GENERATED_FILES**: files output by this process
# - **MODULE_DEPENDENCIES:** xml and hand specified module dependencies
# - **FILE_DEPENDENCIES:** xml file dependencies
# - **EXTRAS:** used to carry over the XML type
#
# Implicit from scope: XML_LOWER_TYPE, FPRIME_BUILD_LOCATIONS_SEP, PYTHON_AUTOCODER_DIR, FPRIME_AC_CONSTANTS_FILE,
#     FPRIME_FRAMEWORK_PATH,
####
function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    set(GEN_ARGS)
    if(EXTRAS STREQUAL "topologyapp")
        set(GEN_ARGS "--connect_only" "--xml_topology_dict")
    endif()
    setup_ai_autocode_variant("${GEN_ARGS}" "${CMAKE_CURRENT_BINARY_DIR}" "" "${AC_INPUT_FILE}" "${GENERATED_FILES}"
                              "${MODULE_DEPENDENCIES}" "${FILE_DEPENDENCIES}")
endfunction(setup_autocode)
