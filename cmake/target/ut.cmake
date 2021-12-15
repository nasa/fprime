####
# target/ut.cmake:
#
# UTs target implementation.
####

####
# `add_global_target`:
#
# Implementation defines the target using `add_custom_target` and nothing more.
####
function(add_global_target TARGET)
    if (FPRIME_ENABLE_UTIL_TARGETS)
        add_custom_target(${TARGET})
    endif()
endfunction(add_global_target)


# Function `add_deployment_target`:
#
# Creates a target for UTs per-deployment.
#
# - **MODULE:** name of the module
# - **TARGET:** name of target to produce
# - **SOURCES:** list of source file inputs
# - **DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
# - **FULL_DEPENDENCIES:** MOD_DEPS input from CMakeLists.txt
####
function(add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    if (NOT FPRIME_ENABLE_UTIL_TARGETS)
        return()
    endif()
    add_custom_target("${MODULE}_${TARGET}")
    foreach(DEPENDENCY IN LISTS FULL_DEPENDENCIES)
        get_property(DEPENDENCY_UTS TARGET "${DEPENDENCY}" PROPERTY FPRIME_UTS)
        if (DEPENDENCY_UTS)
            add_dependencies("${MODULE}_${TARGET}" ${DEPENDENCY_UTS})
        endif()
    endforeach()
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
function(add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
    # Protects against multiple calls to fprime_register_ut()
    if (NOT BUILD_TESTING)
        return()
    endif()
    message(STATUS "Adding ${FPRIME_OBJECT_TYPE}: ${UT_EXE_NAME}")
    run_ac_set("${SOURCE_FILES}" INFO_ONLY autocoder/fpp autocoder/ai-ut)
    resolve_dependencies(RESOLVED gtest_main -lpthread ${DEPENDENCIES} ${AC_DEPENDENCIES})
    setup_build_module("${UT_EXE_NAME}" "${SOURCE_FILES}" "${AC_GENERATED}" "${AC_SOURCES}" "${RESOLVED}")

    target_include_directories("${UT_EXE_NAME}" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
    add_test(NAME ${UT_EXE_NAME} COMMAND ${UT_EXE_NAME})

    if (NOT TARGET "${MODULE_NAME}_${TARGET_NAME}" AND FPRIME_ENABLE_UTIL_TARGETS)
        add_custom_target("${MODULE_NAME}_${TARGET_NAME}")
    endif()
    if (FPRIME_ENABLE_UTIL_TARGETS)
        add_dependencies("${MODULE_NAME}_${TARGET_NAME}" "${UT_EXE_NAME}")
        add_dependencies("${TARGET_NAME}" "${UT_EXE_NAME}")
        set_property(TARGET "${MODULE_NAME}" APPEND PROPERTY FPRIME_UTS "${UT_EXE_NAME}")
    endif()
    # Link library list output on per-module basis
    if (CMAKE_DEBUG_OUTPUT)
        introspect("${UT_EXE_NAME}")
    endif()
endfunction(add_module_target)
