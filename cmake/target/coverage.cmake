####
# coverage.cmake:
#
# Coverage target adds in gcov endpoint for calculating the coverage of F prime modules. This
# coverage requires the system supply a `gcov` target and the code be built with the `TESTING`
# build type.  This means the  following things to be setup:
# 
# 1. `gcov` must be available on the path
# 2. `-DCMAKE_BUILD_TYPE=TESTING` must be supplied
# 
# Once the CMake build directory has been created the user can run the CMake targers
# `<MODULE>_coverage` where <MODULE> is the name of the module to generate coverage for. These
# _coverage targets perform the following steps:
#
# 1. Build and run UT dependencies using the `<MODULE>_check` target
# 2. Run the `gcov` program on the Module's source files
# 3. Copy the .gcov reports into a "coverage" subdirectory of the module's source
#
# These targets can be run using the build system as shown in the example below:
# 
# **Example**
# ```
# make Svc_CmdDispatcher_Cmd
# ```
# **Note:** although a globale `coverage` target is created, it typically should not be used as
# CTest provides better global coverage with the `Coverage` target.
#
# ## Detailed Function Desceiptions
# 
# The following functions are used to register the _coverage targets into the target system. They
# are required for the system to register custom targets.
####
####
#  Function `add_global_target`:
#
#  Adds a global target. Note: only run for "TESTING" builds.
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
# Creats each module's coverage targets.
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
    # Test for the 'gcov' program or bail with WARNING
    find_program(GCOV_EXE "gcov")
    if (DEFINED GCOV_EXE-NOTFOUND)
        message(WARNING "Failed to find 'gcov' program for calculating coverage")
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
        COMMAND ${GCOV_EXE} -o CMakeFiles/${MODULE_NAME}.dir/ ${COV_FILES} 
        COMMAND ${CMAKE_COMMAND} -E copy *.gcov ${CMAKE_CURRENT_LIST_DIR}/coverage
    )
    add_dependencies(${TARGET_NAME} ${MODULE_NAME}_check)
endfunction(add_module_target)
