####
# TestDirectiveParser/CMakeLists.txt:
#
# Minimal project used to exercise `fprime__process_module_setup` (cmake/module.cmake) directly, one
# case per configure. Driven by cmake/test/src/test_directive_parser.py through these cache variables:
#
# - PARSER_MODULE_TYPE: module type passed to the parser (default "Library")
# - PARSER_ADDITIONAL_CONTROL_SETS: extra control directives passed to the parser (default none)
# - PARSER_ARGS: argument list handed to the parser, exactly as register_fprime_* would forward it
# - PARSER_CURRENT_MODULE: value for FPRIME_CURRENT_MODULE when set
# - PARSER_LEGACY_<VAR>: sets legacy variable <VAR> (e.g. SOURCE_FILES, MOD_DEPS, UT_SOURCE_FILES)
#
# Every INTERNAL_* output the parser defines is reported as "[test] INTERNAL_<name>=<value>".
####
cmake_minimum_required(VERSION 3.16)
project(TestDirectiveParser NONE)

set(FPRIME_FRAMEWORK_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../..")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../..")
include(module)

if (NOT DEFINED PARSER_MODULE_TYPE)
    set(PARSER_MODULE_TYPE "Library")
endif()
if (DEFINED PARSER_CURRENT_MODULE)
    set(FPRIME_CURRENT_MODULE "${PARSER_CURRENT_MODULE}")
endif()
foreach(LEGACY_VARIABLE IN ITEMS SOURCE_FILES MOD_DEPS HEADER_FILES UT_SOURCE_FILES UT_MOD_DEPS UT_AUTO_HELPERS)
    if (DEFINED "PARSER_LEGACY_${LEGACY_VARIABLE}")
        set("${LEGACY_VARIABLE}" "${PARSER_LEGACY_${LEGACY_VARIABLE}}")
    endif()
endforeach()

# Run inside a function so each call sees a fresh scope, as register_fprime_* does
function(run_parser_case)
    fprime__process_module_setup("${PARSER_MODULE_TYPE}" "${PARSER_ADDITIONAL_CONTROL_SETS}" ${PARSER_ARGS})
    set(OUTPUTS MODULE_NAME ${FPRIME__INTERNAL_BASE_CONTROL_SETS} ${PARSER_ADDITIONAL_CONTROL_SETS} CMAKE_ADD_OPTIONS)
    foreach(OUTPUT IN LISTS OUTPUTS)
        if (DEFINED "INTERNAL_${OUTPUT}")
            message(STATUS "[test] INTERNAL_${OUTPUT}=${INTERNAL_${OUTPUT}}")
        endif()
    endforeach()
endfunction()
run_parser_case()
