####
# autocoder/ai-xml.cmake
#
# Primary Ai XML autocoder that sets up the C++ file generation. This is a implementation of the singular autocoder
# setup of the the original CMake system.
#####
include(utilities)
include(autocoder/helpers)
include(autocoder/ai-shared)

autocoder_setup_for_individual_sources()
####
# `is_supported`:
#
# Given a single input file, determines if that input file is processed by this autocoder. Sets the variable named
# IS_SUPPORTED in parent scope to be TRUE if the source is an AI XML file or FALSE otherwise.
#
# AC_INPUT_FILE: filepath for consideration
####
function(ai_xml_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("Ai.xml" "${AC_INPUT_FILE}")
endfunction (ai_xml_is_supported)

####
# `get_generated_files`:
#
# Given a set of supported autocoder input files, this will produce a list of files that will be generated. It sets the
# following variables in parent scope:
#
# - GENERATED_FILES: a list of files generated for the given input sources
# - MODULE_DEPENDENCIES: inter-module dependencies determined from the given input sources
# - FILE_DEPENDENCIES: specific file dependencies of the given input sources
# - EXTRAS: used to publish the 'xml type' file and files to remove
#
# Note: although this function is only required to set `GENERATED_FILES`, the remaining information is also set as
# setting this information now will prevent a duplicated call to the tooling.
#
# AC_INPUT_FILES: list of supported autocoder input files
####
function(ai_xml_get_generated_files AC_INPUT_FILE)


    set(GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}${XML_TYPE}Ac.hpp"
                        "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}${XML_TYPE}Ac.cpp")


    # Topology also builds dictionary.  If we are excluding topology autocode, it is the only generated file
    if (XML_LOWER_TYPE STREQUAL "topologyapp" AND EXCLUDE_TOP_ACS)
        list(APPEND EXTRAS ${GENERATED_FILES})
        set(GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}${XML_TYPE}Dictionary.xml")
    elseif(XML_LOWER_TYPE STREQUAL "topologyapp")
        list(APPEND GENERATED_FILES "${CMAKE_CURRENT_BINARY_DIR}/${AC_OBJ_NAME}${XML_TYPE}Dictionary.xml")
    endif()
    set(EXTRAS "${EXTRAS}" PARENT_SCOPE)
    set(GENERATED_FILES "${GENERATED_FILES}" PARENT_SCOPE)
endfunction(ai_xml_get_generated_files)


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
# `setup_autocode`:
#
# Sets up the AI XML autocoder to generate files.
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
            DEPENDS "${AC_INPUT_FILE}" "${MODULE_DEPENDENCIES}" "${AC_INPUT_FILE}" "${FILE_DEPENDENCIES}"
        )
    endif()
    set(AUTOCODER_GENERATED "${GENERATED_FILES}" PARENT_SCOPE)
    set(AUTOCODER_DEPENDENCIES "${MODULE_DEPENDENCIES}" PARENT_SCOPE)
endfunction(ai_xml_setup_autocode)
