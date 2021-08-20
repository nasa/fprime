include(Utilities)

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
        foreach(AI_TYPE IN ITEMS "Component" "Port" "Enum" "Serializable" "Array")
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
    set(AI_XML_TYPE "${XML_TYPE}" PARENT_SCOPE)
    set(AI_XML_LOWER_TYPE "${XML_LOWER_TYPE}" PARENT_SCOPE)
    set(AI_AC_OBJ_NAME "${AC_OBJ_NAME}" PARENT_SCOPE)
    set(AI_MODULE_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(AI_FILE_DEPENDENCIES "${FILE_DEPENDENCIES}" PARENT_SCOPE)

    set(GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}${XML_TYPE}Ac.hpp"
                        "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}${XML_TYPE}Ac.cpp")

    # Topology also builds dictionary
    if (XML_TYPE STREQUAL "topologyapp")
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}Dictionary.xml")
    endif()

    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
endfunction(get_generated_files)

function(get_dependencies AC_INPUT_FILE)
    # Should have been inherited from previous call to `get_generated_files`
    set(MODULE_DEPENDENCIES "${AI_MODULE_DEPENDENCIES}" PARENT_SCOPE)
    set(FILE_DEPENDENCIES "${AI_FILE_DEPENDENCIES}" PARENT_SCOPE)
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
    # Run the parser and capture the output. If an error occurs, that fatals CMake as we cannot continue
    set(MODULE_NAME_NO_SUFFIX "${MODULE_NAME}")
    execute_process(
            COMMAND "${FPRIME_FRAMEWORK_PATH}/cmake/support/parser/ai_parser.py" "${XML_PATH}" "${MODULE_NAME_NO_SUFFIX}" "${FPRIME_CLOSEST_BUILD_ROOT}"
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
#
# Implicit from scope: XML_LOWER_TYPE, FPRIME_BUILD_LOCATIONS_SEP, PYTHON_AUTOCODER_DIR, FPRIME_AC_CONSTANTS_FILE,
#     FPRIME_FRAMEWORK_PATH,
####
function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES)
    add_dependencies(${OBJ_NAME} ${CODEGEN_TARGET})
    if(XML_LOWER_TYPE STREQUAL "topologyapp")
        set(GEN_ARGS "--build_root" "--connect_only" "--xml_topology_dict")
    else()
        set(GEN_ARGS "--build_root")
    endif()

    # If the AI isn't in the final place, copy it over so that we have a clean run
    #get_filename_component(AI_NAME "${AC_INPUT_FILE}" NAME)
    #set(FINAL_AI_XML "${CMAKE_CURRENT_BINARY_DIR}/${AI_NAME}")
    #if (NOT AC_INPUT_FILE STREQUAL FINAL_AI_XML)
    #    add_custom_command(OUTPUT ${FINAL_AI_XML}
    #                       COMMAND ${CMAKE_COMMAND} -E copy ${AC_INPUT_FILE} ${FINAL_AI_XML}
    #                       DEPENDS ${AC_INPUT_FILE}
    #    )
    #endif()
    # Run Ai Autocoder
    string(REPLACE ";" ":" FPRIME_BUILD_LOCATIONS_SEP "${FPRIME_BUILD_LOCATIONS}")
    add_custom_command(
            OUTPUT  ${GENERATED_FILES}
            COMMAND ${CMAKE_COMMAND} -E env
              PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils
              BUILD_ROOT="${FPRIME_BUILD_LOCATIONS_SEP}:${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/F-Prime"
              FPRIME_AC_CONSTANTS_FILE="${FPRIME_AC_CONSTANTS_FILE}"
              PYTHON_AUTOCODER_DIR=${PYTHON_AUTOCODER_DIR}
            ${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/bin/codegen.py -p ${CMAKE_CURRENT_BINARY_DIR} ${GEN_ARGS} ${AC_INPUT_FILE}
            DEPENDS ${AC_INPUT_FILE} ${FILE_DEPENDENCIES} ${FPRIME_AC_CONSTANTS_FILE} ${MODULE_DEPENDENCIES}
    )
endfunction(setup_autocode)
