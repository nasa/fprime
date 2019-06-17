####
# coverage.cmake:
#
# Coverage target adds in gcov endpoint for calculating the coverage of F prime modules.
# This means that the following functions are defined:
#
# - `add_global_target`: global coverarge setup
# - `add_module_target`: adds sub-targets for '<MODULE_NAME>_coverage'
####
####
# Dict function `add_global_target`:
#
# Add target for the `dict` custom target. Dictionaries are built-in targets, but they are defined
# as custom targets. This handles the top-level dictionary target `dict` and registers the steps to
# perform the generation of the target.  TARGET_NAME should be set to `dict`.
#
# - **TARGET_NAME:** target name to be generated
####
function(add_global_target TARGET_NAME)
    # UTs don't supply directories
    if (NOT CMAKE_BUILD_TYPE STREQUAL "TESTING")
        return()
    endif()
    add_custom_target(${TARGET_NAME})
endfunction(add_global_target)
####
# Dict function `add_module_target`:
#
# Adds a module-by-module target for procducing dictionaries. These dictionaries take the outputs
# from the autocoder and copies them into the correct directory. These outputs are then handled as
# part of the global `dict` target above.
#
#
# - **MODULE_NAME:** name of the module
# - **TARGET_NAME:** name of target to produce
# - **AC_INPUTS:** list of autocoder inputs
# - **SOURCE_FILES:** list of source file inputs
# - **AC_OUTPUTS:** list of autocoder outputs
####
function(add_module_target MODULE_NAME TARGET_NAME AC_INPUTS SOURCE_FILES AC_OUTPUTS)
    # UTs don't supply directories, non modules don't get coverage
    if (NOT CMAKE_BUILD_TYPE STREQUAL "TESTING" OR NOT "${FPRIME_OBJECT_TYPE}" STREQUAL "Library")
        return()
    endif()
    set(COV_FILES "")
    foreach(SRC_IN ${SOURCE_FILES};${AC_OUTPUTS})
        get_filename_component(EXTN ${SRC_IN} EXT)
        if (EXTN STREQUAL ".cpp")
            list(APPEND COV_FILES ${SRC_IN})
        endif()
    endforeach()
    # Check target for this module
    if (NOT TARGET "${MODULE_NAME}_check")
	    add_custom_target("${MODULE_NAME}_check" COMMAND ${CMAKE_CTEST_COMMAND})
    endif()
    add_custom_target(
        ${TARGET_NAME}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_LIST_DIR}/coverage
        COMMAND gcov -o CMakeFiles/${MODULE_NAME}.dir/ ${COV_FILES} 
        COMMAND ${CMAKE_COMMAND} -E copy *.gcov ${CMAKE_CURRENT_LIST_DIR}/coverage
    )
    add_dependencies(${TARGET_NAME} ${MODULE_NAME}_check)
endfunction(add_module_target)
