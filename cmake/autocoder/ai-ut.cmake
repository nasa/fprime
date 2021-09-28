
# Bail if not testing
if (NOT CMAKE_BUILD_TYPE STREQUAL "TESTING" )
    return()
endif()

#set(MEM_TEST_CLI_OPTIONS '--leak-check=full --error-exitcode=100 --show-leak-kinds=all -v')

# Enable testing, setup CTest, etc.
#enable_testing()
#include( CTest )
#add_custom_target(check
#        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} find . -name "*.gcda" -delete
#        COMMAND ${CMAKE_CTEST_COMMAND})
#add_custom_target(check_leak
#        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} find . -name "*.gcda" -delete
#        COMMAND ${CMAKE_CTEST_COMMAND}
#        --overwrite MemoryCheckCommand=/usr/bin/valgrind
#        --overwrite MemoryCheckCommandOptions=${MEM_TEST_CLI_OPTIONS}
#        -T MemCheck)

function(is_supported AC_INPUT_FILE)
    if (AC_INPUT_FILE MATCHES ".*ComponentAi\\.xml")
        set(IS_SUPPORTED TRUE PARENT_SCOPE)
    else()
        set(IS_SUPPORTED FALSE PARENT_SCOPE)
    endif()
endfunction (is_supported)

function(get_generated_files AC_INPUT_FILE)
    set(GENERATED_FILES ${CMAKE_CURRENT_BINARY_DIR}/GTestBase.cpp ${CMAKE_CURRENT_BINARY_DIR}/TesterBase.cpp
                        ${CMAKE_CURRENT_BINARY_DIR}/GTestBase.hpp ${CMAKE_CURRENT_BINARY_DIR}/TesterBase.hpp PARENT_SCOPE)
endfunction(get_generated_files)

function(get_dependencies AC_INPUT_FILE)
endfunction(get_dependencies)

function(setup_autocode AC_INPUT_FILE GENERATED_FILES MODULE_DEPENDENCIES FILE_DEPENDENCIES EXTRAS)
    add_dependencies(${UT_EXE_NAME} ${CODEGEN_TARGET})
    string(REPLACE ";" ":" FPRIME_BUILD_LOCATIONS_SEP "${FPRIME_BUILD_LOCATIONS}")
    add_custom_command(
            OUTPUT  ${GENERATED_FILES}
            COMMAND ${CMAKE_COMMAND} -E env
            PYTHONPATH=${PYTHON_AUTOCODER_DIR}/src:${PYTHON_AUTOCODER_DIR}/utils
            BUILD_ROOT="${FPRIME_BUILD_LOCATIONS_SEP}:${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/F-Prime"
            FPRIME_AC_CONSTANTS_FILE="${FPRIME_AC_CONSTANTS_FILE}"
            PYTHON_AUTOCODER_DIR=${PYTHON_AUTOCODER_DIR}
            ${FPRIME_FRAMEWORK_PATH}/Autocoders/Python/bin/codegen.py -p ${CMAKE_CURRENT_BINARY_DIR} -u ${AC_INPUT_FILE} --build_root
            COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/Tester.hpp ${CMAKE_CURRENT_BINARY_DIR}/Tester.cpp
            COMMAND ${CMAKE_COMMAND} -E echo "All done Yo!"
            DEPENDS ${AC_INPUT_FILE} ${FPRIME_AC_CONSTANTS_FILE}
    )
endfunction()
