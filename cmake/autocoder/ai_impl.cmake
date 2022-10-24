####
# autocoder/ai-impl:
#
# Autocoder implementation for the generation of implementation templates. This is triggered by the `impl` target and
# runs codegen to produce the template files.
####
include(utilities)
include(autocoder/helpers)
include(autocoder/ai-shared)

autocoder_setup_for_individual_sources()

####
# `ai_impl_is_supported`:
#
# Required function, processes ComponentAi.xml files.
# `AC_INPUT_FILE` potential input to the autocoder
####
function(ai_impl_is_supported AC_INPUT_FILE)
    autocoder_support_by_suffix("ComponentAi.xml" "${AC_INPUT_FILE}")
endfunction (ai_impl_is_supported)

####
# `setup_autocode`:
#
# Required function, sets up a custom command to produce .hpp-template and .cpp-template files.
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
        DEPENDS "${AC_INPUT_FILE}" "${CODEGEN_TARGET}"
    )
    set(AUTOCODER_GENERATED "${AUTOCODER_GENERATED}" PARENT_SCOPE)
endfunction(ai_impl_setup_autocode)
