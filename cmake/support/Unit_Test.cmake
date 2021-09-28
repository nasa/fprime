####
# Unit_Test.cmake:
#
# Testing does not properly handle unit test dependencies in some versions of CMake. Therefore,
# we follow a standard workaround from CMake users and create a "check" target used to run the
# tests while rolling-up the dependencies properly. Thus tests may be run using `make check` as
# opposed to the standard CMake call. The CMake test support functions are still used.
#
####
# Bail if not testing
if (NOT CMAKE_BUILD_TYPE STREQUAL "TESTING" )
    return()
endif()

set(MEM_TEST_CLI_OPTIONS '--leak-check=full --error-exitcode=100 --show-leak-kinds=all -v')

# Enable testing, setup CTest, etc.
enable_testing()
include( CTest )
add_custom_target(check
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} find . -name "*.gcda" -delete
            COMMAND ${CMAKE_CTEST_COMMAND})
add_custom_target(check_leak
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} find . -name "*.gcda" -delete
            COMMAND ${CMAKE_CTEST_COMMAND}
                  --overwrite MemoryCheckCommand=/usr/bin/valgrind
                  --overwrite MemoryCheckCommandOptions=${MEM_TEST_CLI_OPTIONS}
                  -T MemCheck)

function(generate_ut UT_EXE_NAME UT_SOURCES_INPUT MOD_DEPS_INPUT)
    if (FPRIME_FPP_LOCS_BUILD)
        return()
    endif()
    message(STATUS "Adding Unit-Test: ${UT_EXE_NAME}")
    # Basic Unit-Test setup
    add_executable("${UT_EXE_NAME}" "${EMPTY_C_SRC}")
    add_dependencies("${UT_EXE_NAME}" "${MODULE_NAME}")
    target_link_libraries("${UT_EXE_NAME}" "gtest_main" "-lpthread" "${MODULE_NAME}")
    target_include_directories("${UT_EXE_NAME}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    # CMake object type
    if (NOT DEFINED FPRIME_OBJECT_TYPE)
        set(FPRIME_OBJECT_TYPE "Unit-Test")
    endif()

    # Run the autocoder
    run_ac_set("${UT_SOURCES_INPUT}" INFO_ONLY autocoder/fpp autocoder/ai-ut)
    resolve_dependencies(RESOLVED ${MOD_DEPS_INPUT} ${AC_DEPENDENCIES})
    update_module("${UT_EXE_NAME}" "${UT_SOURCES_INPUT}" "${AC_GENERATED}" "${AC_SOURCES}" "${RESOLVED}")

    # Add test and dependencies to the "check" target
    add_test(NAME ${UT_EXE_NAME} COMMAND ${UT_EXE_NAME})
    add_dependencies(check ${UT_EXE_NAME})
    add_dependencies(check_leak ${UT_EXE_NAME})

    # Check target for this module
    # gcda files are generated per object file when executing a binary with coverage enabled
    # make sure all existing coverage files are removed before running unit test executables
    if (NOT TARGET "${MODULE_NAME}_check")
        add_custom_target(
                "${MODULE_NAME}_check"
                COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} find . -name "*.gcda" -delete
                COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        )
    endif()
    if (NOT TARGET "${MODULE_NAME}_check_leak")
        add_custom_target(
                "${MODULE_NAME}_check_leak"
                COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} find . -name "*.gcda" -delete
                COMMAND ${CMAKE_CTEST_COMMAND}
                --overwrite MemoryCheckCommand=/usr/bin/valgrind
                --overwrite MemoryCheckCommandOptions=${MEM_TEST_CLI_OPTIONS}
                --verbose -T MemCheck
        )
    endif()

    # Add top ut wrapper for this module
    if (NOT TARGET "${MODULE_NAME}_ut_exe")
        add_custom_target("${MODULE_NAME}_ut_exe")
    endif()

    add_dependencies("${MODULE_NAME}_check" ${UT_EXE_NAME})
    add_dependencies("${MODULE_NAME}_check_leak" ${UT_EXE_NAME})
    add_dependencies("${MODULE_NAME}_ut_exe" ${UT_EXE_NAME})

    # Link library list output on per-module basis
    if (CMAKE_DEBUG_OUTPUT)
        introspect("${UT_EXE_NAME}")
    endif()
endfunction(generate_ut)
