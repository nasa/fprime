####
# testimpl.cmake:
#
# Dictionary target definition file. Used to define `<MODULE>_impl` target. Defined as
# a standard target pattern. This means that the following functions are defined:
#
# - `add_global_target`: does nothing
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_impl'
####

####
# Impl function `add_module_target`:
#
# Adds a module-by-module target for procducing implementations. Take in the Ai.xml file and produces a
#
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
####
function(add_module_target MODULE_NAME TARGET_NAME AC_INPUTS SOURCE_FILES AC_OUTPUTS)
    set(AI_XML "")
    foreach (AC_IN ${AC_INPUTS})
        if (AC_IN MATCHES ".*ComponentAi.xml")
            set(AI_XML ${AC_IN})
        endif()
    endforeach()
    # No component AI XMLs
    if ("${AI_XML}" STREQUAL "")
        return()
    endif()
    string(REPLACE "CompnentAi.xml" "${AI_XML}" "ComponentImpl" BASE_TEMPLATE)
    # Try to generate dictionaries for every AC input file
    add_custom_target("${TARGET_NAME}" 
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_COMMAND} -E env PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils BUILD_ROOT=${FPRIME_CURRENT_BUILD_ROOT}
      PYTHON_AUTOCODER_DIR=${PYTHON_AUTOCODER_DIR} FPRIME_CORE_DIR=${FPRIME_CORE_DIR}
      ${FPRIME_CORE_DIR}/Autocoders/Python/bin/testgen.py ${AI_XML}
      DEPENDS ${AI_XML}
    )
endfunction(add_module_target)
