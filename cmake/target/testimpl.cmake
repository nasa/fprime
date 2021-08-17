####
# testimpl.cmake:
#
# This target invokes the unit test code template generation available as part of the fprime autocoder. It implements
# the target interface described here: [Targets](Targets.md).
#
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_test_impl'
####

####
# Impl function `add_module_target`:
#
# Adds a module-by-module target for producing implementations. Take in the Ai.xml file and produces a set of test
# templates.
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **GLOBAL_TARGET_NAME:** name of produced global target
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
# - **MOD_DEPS:** hand specified dependencies of target
####
function(add_module_target MODULE_NAME TARGET_NAME GLOBAL_TARGET_NAME AC_INPUTS SOURCE_FILES AC_OUTPUTS MOD_DEPS)
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
    string(REPLACE "ComponentAi.xml" "${AI_XML}" "ComponentImpl" BASE_TEMPLATE)
    string(REPLACE ";" ":" FPRIME_BUILD_LOCATIONS_SEP "${FPRIME_BUILD_LOCATIONS}")
    # Try to generate dictionaries for every AC input file
    add_custom_target("${TARGET_NAME}" 
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_COMMAND} -E env PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils BUILD_ROOT="${FPRIME_BUILD_LOCATIONS_SEP}"
      FPRIME_AC_CONSTANTS_FILE="${FPRIME_AC_CONSTANTS_FILE}"
      PYTHON_AUTOCODER_DIR=${PYTHON_AUTOCODER_DIR} DICTIONARY_DIR=${DICTIONARY_DIR}
      ${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/bin/testgen.py ${AI_XML}
      DEPENDS ${AI_XML}
    )
    # Add dependencies to codegen
    add_dependencies("${TARGET_NAME}" "${CODEGEN_TARGET}")
endfunction(add_module_target)
