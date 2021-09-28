include(Utilities)

set(HANDLES_INDIVIDUAL_SOURCES TRUE)
function(is_supported AC_INPUT_FILE)
    if (AC_INPUT_FILE MATCHES ".*ComponentAi\\.xml")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    else()
        set(IS_SUPPORTED FALSE PARENT_SCOPE)
    endif()
endfunction (is_supported)

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

function(get_dependencies AC_INPUT_FILE)
endfunction(get_dependencies)


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
    setup_ai_autocode_variant("-t" "${CMAKE_CURRENT_LIST_DIR}" "" "${AC_INPUT_FILE}" "${GENERATED_FILES}"
                              "${MODULE_DEPENDENCIES}" "${FILE_DEPENDENCIES}")
endfunction(setup_autocode)
