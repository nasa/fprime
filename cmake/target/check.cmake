include(target/ut)

####
# check.cmake:
#
# Check target adds in an endpoint for running the individual unit tests.
####
####
#  Function `add_global_target`:
#
#  Adds a global target. Note: only run for "BUILD_TESTING=ON" builds.
#
# - **TARGET_NAME:** target name to be generated
####
function(check_add_global_target TARGET_NAME)
    add_custom_target(${TARGET_NAME} COMMAND ${CMAKE_CTEST_COMMAND})
endfunction(check_add_global_target)

####
# Function `add_deployment_target`:
#
# Creates a target for "check" per-deployment, to run all UTs within that deployment.
#
# - **MODULE:** name of the module
# - **TARGET:** name of target to produce
# - **SOURCES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
# - **FULL_DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(check_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    set(ALL_UTS)
    foreach(DEPENDENCY IN LISTS FULL_DEPENDENCIES)
        get_property(DEPENDENCY_UTS TARGET "${DEPENDENCY}" PROPERTY FPRIME_UTS)
        list(APPEND ALL_UTS ${DEPENDENCY_UTS})
    endforeach()
    # Only run deployment UTs when some are defined
    if (ALL_UTS)
        string(REPLACE ";" "\\|" JOINED_UTS "${ALL_UTS}")
        add_custom_target(${MODULE}_${TARGET_NAME}
            COMMAND ${CMAKE_CTEST_COMMAND} -R "${JOINED_UTS}"
            DEPENDS ${ALL_UTS}
        )
    else()
        add_custom_target(${MODULE}_${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -E echo "No unit tests defined for ${MODULE}"
        )
    endif()
endfunction()

####
# Function `check_add_module_target`:
#
# Creates each module's check targets. Note: only run for "BUILD_TESTING=ON" builds.
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **SOURCE_FILES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(check_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
    # Protects against multiple calls to fprime_register_ut()
    if (NOT BUILD_TESTING OR NOT MODULE_TYPE STREQUAL "Unit Test")
        return()
    endif()
    # UTs MODULE_NAME defaults to <FPRIME_MODULE_NAME>_ut_exe
    # The below handling gives CHECK_TARGET_NAME = <FPRIME_MODULE_NAME>_check
    string(REGEX REPLACE "_${UT_TARGET}$" "" CHECK_TARGET_NAME "${MODULE_NAME}")
    string(APPEND CHECK_TARGET_NAME "_${TARGET_NAME}")
    if (NOT TARGET ${CHECK_TARGET_NAME})
        add_custom_target(
            "${CHECK_TARGET_NAME}"
            COMMAND ${CMAKE_CTEST_COMMAND} --verbose
        )
    endif()
    add_dependencies("${CHECK_TARGET_NAME}" ${UT_EXE_NAME})
    add_dependencies(check ${UT_EXE_NAME})
endfunction(check_add_module_target)
