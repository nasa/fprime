####
# autocoder/ai-impl:
#
# Implementation template autocoder file.
####
include(utilities)
include(autocoder/helpers)
include(autocoder/ai-shared)

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
function(ai_impl_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("ComponentAi.xml" "${AC_INPUT_FILE}")
endfunction (ai_impl_is_supported)


####
# `setup_autocode`:
#
# Sets up the AI XML autocoder to generate files.
####
function(ai_impl_setup_autocode AC_INPUT_FILE)
    ai_split_xml_path("${AC_INPUT_FILE}")
    set(AUTOCODER_GENERATED
        "${CMAKE_CURRENT_SOURCE_DIR}/${OBJ_NAME}${XML_TYPE}Impl.hpp-template"
        "${CMAKE_CURRENT_SOURCE_DIR}/${OBJ_NAME}${XML_TYPE}Impl.cpp-template"
    )
    # Get the shared setup for all AI autocoders
    ai_shared_setup("${CMAKE_CURRENT_SOURCE_DIR}")
    # Specifically setup the `add_custom_command` call as the requires extra commands to run
    add_custom_command(
        OUTPUT ${AUTOCODER_GENERATED}
        COMMAND ${AI_BASE_SCRIPT} -t "${AC_INPUT_FILE}"
        DEPENDS "${AC_INPUT_FILE}"
    )
    set(AUTOCODER_GENERATED "${AUTOCODER_GENERATED}" PARENT_SCOPE)
endfunction(ai_impl_setup_autocode)
