####
# autocoder/ai-xml.cmake
#
# Primary Ai XML autocoder that sets up the C++ file generation. This is a implementation of the singular autocoder
# setup of the original CMake system. It process AI XML files and produces the autocoder output files Ac.hpp and
# Ac.cpp files.
#####
include(utilities)
include(autocoder/helpers)
include(autocoder/ai-shared)

autocoder_setup_for_individual_sources()
####
# `ai_xml_is_supported`:
#
# Required function, processes ComponentAi.xml files.
# `AC_INPUT_FILE` potential input to the autocoder
####
function(ai_xml_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("Ai.xml" "${AC_INPUT_FILE}")
endfunction (ai_xml_is_supported)

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
macro(__ai_info XML_PATH MODULE_NAME)
    set(EXCLUDE_TOP_ACS TRUE)
    set(MODULE_DEPENDENCIES)
    set(FILE_DEPENDENCIES)
    ai_split_xml_path("${XML_PATH}")
    # Ai.xml files can come from multiple places. One is as a generated file from within the BINARY directory and the
    # other as a source file input.  Generated files will follow a convention and would not generate dependencies.
    if (NOT AC_INPUT_FILE MATCHES "^${CMAKE_CURRENT_BINARY_DIR}.*")
        set(EXCLUDE_TOP_ACS FALSE)
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
        read_from_lines("${AI_OUTPUT}" XML_TYPE_FROM_FILE MODULE_DEPENDENCIES FILE_DEPENDENCIES)
    endif()
    list(APPEND FILE_DEPENDENCIES "${FPRIME_AC_CONSTANTS_FILE}")
endmacro(__ai_info)

####
# `ai_xml_setup_autocode`:
#
# Required function, sets up a custom command to produce Ac.hpp and Ac.cpp files.
####
function(ai_xml_setup_autocode AC_INPUT_FILE)
    __ai_info("${AC_INPUT_FILE}" "${MODULE_NAME}")
    ai_shared_setup("${CMAKE_CURRENT_BINARY_DIR}")
    set(GENERATED_FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${OBJ_NAME}${XML_TYPE}Ac.hpp"
        "${CMAKE_CURRENT_BINARY_DIR}/${OBJ_NAME}${XML_TYPE}Ac.cpp"
    )
    # Check type and respond
    if(XML_LOWER_TYPE STREQUAL "topologyapp")
        # Are we excluding the generated files or not
        if (EXCLUDE_TOP_ACS)
            set(REMOVALS "${GENERATED_FILES}")
            set(GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${OBJ_NAME}${XML_TYPE}Dictionary.xml")
        else()
            set(REMOVALS "${CMAKE_BINARY_DIR}}/does-not-exist-cmake-test-file")
            list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${OBJ_NAME}${XML_TYPE}Dictionary.xml")
        endif()
        add_custom_command(
            OUTPUT ${GENERATED_FILES}
            COMMAND ${AI_BASE_SCRIPT} --connect_only --xml_topology_dict "${AC_INPUT_FILE}"
            COMMAND ${CMAKE_COMMAND} -E remove ${REMOVALS}
            DEPENDS "${AC_INPUT_FILE}" "${MODULE_DEPENDENCIES}" "${AC_INPUT_FILE}" "${FILE_DEPENDENCIES}"
        )
    else()
        add_custom_command(
            OUTPUT ${GENERATED_FILES}
            COMMAND ${AI_BASE_SCRIPT} "${AC_INPUT_FILE}"
            DEPENDS "${AC_INPUT_FILE}" "${MODULE_DEPENDENCIES}" "${FILE_DEPENDENCIES}" "${CODEGEN_TARGET}"
        )
    endif()
    set(AUTOCODER_GENERATED "${GENERATED_FILES}" PARENT_SCOPE)
    set(AUTOCODER_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
endfunction(ai_xml_setup_autocode)
