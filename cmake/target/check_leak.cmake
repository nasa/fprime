####
# check.cmake:
#
# Check target adds in an endpoint for running the individual unit tests with memory leak checking.
####
set(MEM_TEST_CLI_OPTIONS '--leak-check=full --error-exitcode=100 --show-leak-kinds=all -v')
####
#  Function `add_global_target`:
#
#  Adds a global target. Note: only run for "BUILD_TESTING=ON" builds.
#
# - **TARGET_NAME:** target name to be generated
####
function(check_leak_add_global_target TARGET_NAME)
    find_program(VALGRIND valgrind)
    if (VALGRIND)
        add_custom_target(${TARGET_NAME}
                COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} find . -name "*.gcda" -delete
                COMMAND ${CMAKE_CTEST_COMMAND}
                --overwrite MemoryCheckCommand=${VALGRIND}
                --overwrite MemoryCheckCommandOptions=${MEM_TEST_CLI_OPTIONS}
                -T MemCheck)
    else()
        add_custom_target(${TARGET_NAME} COMMAND ${CMAKE_COMMAND} -E echo "[WARNING] 'valgrind' not found. Will not check for leaks.")
    endif()
endfunction(check_leak_add_global_target)

####
# Function `add_deployment_target`:
#
# Creates a target for "check_leak" per-deployment, to run all UTs within that deployment.
#
# - **MODULE:** name of the module
# - **TARGET:** name of target to produce
# - **SOURCES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
# - **FULL_DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(check_leak_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    find_program(VALGRIND valgrind)
    if (VALGRIND)
        set(ALL_UTS)
        foreach(DEPENDENCY IN LISTS FULL_DEPENDENCIES)
            get_property(DEPENDENCY_UTS TARGET "${DEPENDENCY}" PROPERTY FPRIME_UTS)
            list(APPEND ALL_UTS ${DEPENDENCY_UTS})
        endforeach()
        string(REPLACE ";" "\\|" JOINED_UTS "${ALL_UTS}")
        add_custom_target(${MODULE}_${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} find . -name "*.gcda" -delete
            COMMAND ${CMAKE_CTEST_COMMAND}
            --overwrite MemoryCheckCommand=${VALGRIND}
            --overwrite MemoryCheckCommandOptions=${MEM_TEST_CLI_OPTIONS}
            -T MemCheck -R "${JOINED_UTS}"
            DEPENDS ${ALL_UTS}
        )
        foreach(DEPENDENCY IN LISTS FULL_DEPENDENCIES)
            get_property(DEPENDENCY_UTS TARGET "${DEPENDENCY}" PROPERTY FPRIME_UTS)
            if (DEPENDENCY_UTS)
                add_dependencies("${MODULE}_${TARGET}" ${DEPENDENCY_UTS})
            endif()
        endforeach()
    else()
        add_custom_target(${MODULE}_${TARGET_NAME} COMMAND ${CMAKE_COMMAND} -E echo "[WARNING] 'valgrind' not found. WIll not check for leaks.")
    endif()
endfunction()

####
# Dict function `add_module_target`:
#
# Creates each module's coverage targets. Note: only run for "BUILD_TESTING=ON" builds.
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **SOURCE_FILES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(check_leak_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
    find_program(VALGRIND valgrind)
    # Protects against multiple calls to fprime_register_ut()
    if (NOT VALGRIND)
        if (NOT TARGET ${MODULE_NAME}_${TARGET_NAME})
            add_custom_target(${MODULE_NAME}_${TARGET_NAME} COMMAND ${CMAKE_COMMAND} -E echo "[WARNING] 'valgrind' not found. Will not check for leaks.")
        endif()
        return()
    endif()
    if (NOT TARGET ${MODULE_NAME}_${TARGET_NAME})
        add_custom_target(
            "${MODULE_NAME}_${TARGET_NAME}"
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} find . -name "*.gcda" -delete
            COMMAND ${CMAKE_CTEST_COMMAND}
                --overwrite MemoryCheckCommand=${VALGRIND}
                --overwrite MemoryCheckCommandOptions=${MEM_TEST_CLI_OPTIONS}
                --verbose -T MemCheck
        )
    endif()
    add_dependencies("${MODULE_NAME}_${TARGET_NAME}" ${UT_EXE_NAME})
    add_dependencies("${TARGET_NAME}" ${UT_EXE_NAME})
endfunction(check_leak_add_module_target)
