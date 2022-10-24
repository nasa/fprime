####
# `autocoder/ai-shared.cmake`:
#
# Shared implementation for AI XML files. Contains helper and base functions for writing those autocoder more easily.
####
include_guard()
include(utilities)
###
# Function `cheetah`:
#
# This function sets up the ability to call cheetah to compile cheetah templates and prepare them for use
# within the auto-coder. Here we setup the "compilation" of Cheetah templates.
#
# - **CHEETAH_TEMPLATES:** list of cheetah templates
##
function(cheetah CHEETAH_TEMPLATES)
    # Derive output file names, which will be added as dependencies for the code generation step. This
    # forces cheetah templates to be compiled and up-to-date before running the auto-coder.
    string(REPLACE ".tmpl" ".py" PYTHON_TEMPLATES "${CHEETAH_TEMPLATES}")
    # Setup the cheetah-compile command that runs the physical compile of the above statement. This
    # controls the work to be done to create PYTHON_TEMPLATES from ${CHEETAH_TEMPLATES.
    find_program(CHEETAH_EXE NAMES "cheetah-compile" "cheetah-compile3")
    add_custom_command(
            OUTPUT ${PYTHON_TEMPLATES}
            COMMAND ${CHEETAH_EXE} ${CHEETAH_TEMPLATES}
            DEPENDS ${CHEETAH_TEMPLATES}
    )
    # Add the above PYTHON_TEMPLATES to the list of sources for the CODEGEN_TARGET target. Thus they will be
    # built as dependencies for the CODEGEN step.
    set_property(GLOBAL APPEND PROPERTY CODEGEN_OUTPUTS ${PYTHON_TEMPLATES})
endfunction(cheetah)

####
# Function `ai_split_xml_path`:
#
# Splits the XML path into the object type (component, port, ...) and the name (e.g. SignalGen) for consumption in
# various ai autocoders.
#
# `AC_INPUT_FILE`: path to ai file. Need not exist.
# OUTPUTS: XML_LOWER_TYPE, XML_TYPE, OBJ_TYPE
####
function(ai_split_xml_path AC_INPUT_FILE)
    get_filename_component(AC_INPUT_NAME "${AC_INPUT_FILE}" NAME)
    foreach(AI_TYPE IN ITEMS "Component" "Port" "Enum" "Serializable" "Array" "TopologyApp")
        ends_with(HAS_SUFFIX "${AC_INPUT_NAME}" "${AI_TYPE}Ai.xml")
        if (HAS_SUFFIX)
            set(XML_TYPE "${AI_TYPE}")
            break()
        endif()
    endforeach()
    # Required check of output of Ai.xml generator
    if (NOT XML_TYPE)
        message(FATAL_ERROR "[Autocode/ai-xml] Cannot support Ai file of name ${AC_INPUT_NAME}")
    endif()
    ##string(TOLOWER "${XML_TYPE}" XML_LOWER_TYPE)
    string(REPLACE "${XML_TYPE}Ai.xml" "" OBJ_NAME "${AC_INPUT_NAME}")
    # Set outputs in the parent scope
    string(TOLOWER ${XML_TYPE} XML_LOWER_TYPE)
    set(XML_LOWER_TYPE "${XML_LOWER_TYPE}" PARENT_SCOPE)
    set(XML_TYPE "${XML_TYPE}" PARENT_SCOPE)
    set(OBJ_NAME "${OBJ_NAME}" PARENT_SCOPE)
endfunction()

####
# Macro `ai_shared_setup`:
#
# Get the shared setup for the ai autocoder (e.g. codegen invocation). Used in multiple ai autocoders.
#
# OUTPUT_DIR: directory to place outputs into
# Returns: AI_BASE_SCRIPT, configured codegen call.
####
macro(ai_shared_setup OUTPUT_DIR)
    # Ai autocoder dependencies are **always** the codegen target (builds cheetah)
    set(AUTOCODER_DEPENDENCIES "${CODEGEN_TARGET}" PARENT_SCOPE)

    string(REPLACE ";" ":" FPRIME_BUILD_LOCATIONS_SEP "${FPRIME_BUILD_LOCATIONS}")
    # Base script must setup environment for codegen, call codegen.py, and ensure the basic flags are set
    set(AI_BASE_SCRIPT
        "${CMAKE_COMMAND}" -E env
            PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils
            BUILD_ROOT=${FPRIME_BUILD_LOCATIONS_SEP}:${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/F-Prime
            FPRIME_AC_CONSTANTS_FILE=${FPRIME_AC_CONSTANTS_FILE}
            PYTHON_AUTOCODER_DIR=${PYTHON_AUTOCODER_DIR}
        ${PYTHON} ${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/bin/codegen.py -p "${OUTPUT_DIR}" --build_root
    )
endmacro()
