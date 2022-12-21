**Note:** auto-generated from comments in: ./target/ut.cmake

## target/ut.cmake:

UTs target implementation.


## `ut_add_global_target`:

Implementation defines the target using `add_custom_target` and nothing more.


## function(ut_add_deployment_target MODULE TARGET SOURCES DEPENDENCIES FULL_DEPENDENCIES)
    if (NOT FPRIME_ENABLE_UTIL_TARGETS)
        return()
    endif()
    add_custom_target("${MODULE}_${UT_TARGET}")
    foreach(DEPENDENCY IN LISTS FULL_DEPENDENCIES)
        get_property(DEPENDENCY_UTS TARGET "${DEPENDENCY}" PROPERTY FPRIME_UTS)
        if (DEPENDENCY_UTS)
            add_dependencies("${MODULE}_${UT_TARGET}" ${DEPENDENCY_UTS})
        endif()
    endforeach()
endfunction(ut_add_deployment_target)



## function(ut_setup_unit_test_include_directories UT_EXE_NAME SOURCE_FILES)
    set(UT_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}")
    # When running with auto-helpers, we need to include the .hpp directories as things are imported without path
    # e.g. "#include <Tester.hpp>" and there is no guarantee for the location of these files
    if (DEFINED UT_AUTO_HELPERS AND UT_AUTO_HELPERS)
        foreach(SOURCE_FILE IN LISTS SOURCE_FILES)
            get_filename_component(SOURCE_EXT "${SOURCE_FILE}" LAST_EXT)
            get_filename_component(SOURCE_DIR "${SOURCE_FILE}" DIRECTORY)
            if (SOURCE_EXT STREQUAL ".cpp" AND NOT SOURCE_DIR IN_LIST UT_INCLUDE_DIRECTORIES)
                list(APPEND UT_INCLUDE_DIRECTORIES "${SOURCE_DIR}")
            endif()
        endforeach()
    endif()
    target_include_directories("${UT_EXE_NAME}" PRIVATE ${UT_INCLUDE_DIRECTORIES})
endfunction(ut_setup_unit_test_include_directories)



## function(ut_add_module_target MODULE_NAME TARGET_NAME SOURCE_FILES DEPENDENCIES)
    # Protects against multiple calls to fprime_register_ut()
    if (NOT BUILD_TESTING OR NOT MODULE_TYPE STREQUAL "Unit Test")
        return()
    endif()
    message(STATUS "Adding Unit Test: ${UT_EXE_NAME}")
    run_ac_set("${SOURCE_FILES}" autocoder/fpp autocoder/ai_ut)
    resolve_dependencies(RESOLVED gtest_main ${DEPENDENCIES} ${AC_DEPENDENCIES})
    build_setup_build_module("${UT_EXE_NAME}" "${SOURCE_FILES}" "${AC_GENERATED}" "${AC_SOURCES}" "${RESOLVED}")

    ut_setup_unit_test_include_directories("${UT_EXE_NAME}" "${SOURCE_FILES}")
    add_test(NAME ${UT_EXE_NAME} COMMAND ${UT_EXE_NAME})

    # Create a module-level target if not already done
    if (NOT TARGET "${MODULE_NAME}_${UT_TARGET}" AND FPRIME_ENABLE_UTIL_TARGETS)
        add_custom_target("${MODULE_NAME}_${UT_TARGET}")
    endif()
    # Add module level target dependencies to this UT
    if (FPRIME_ENABLE_UTIL_TARGETS)
        add_dependencies("${MODULE_NAME}_${UT_TARGET}" "${UT_EXE_NAME}")
        add_dependencies("${UT_TARGET}" "${UT_EXE_NAME}")
        set_property(TARGET "${MODULE_NAME}" APPEND PROPERTY FPRIME_UTS "${UT_EXE_NAME}")
    endif()
    # Link library list output on per-module basis
    if (CMAKE_DEBUG_OUTPUT)
        introspect("${UT_EXE_NAME}")
    endif()
endfunction(ut_add_module_target)
