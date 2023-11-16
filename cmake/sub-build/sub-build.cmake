####
# sub-build.cmake:
#
# CMake occasionally requires sub-builds in order to generate certain types of information.  The sub-build mechanics are
# implemented in this file. This includes the ability to setup targets, builds, and handle information passing back and
# forth between the modules.
#
# This file runs CMake on fprime, but registering only a set of target: (e.g. [fpp_locs](./target/fpp_locs.cmake) that
# do the work on the build. This allows CMake to generate build information to be consumed at the generate phase while
# maintaining the efficiency of properly expressed builds.
####
include_guard()
include(utilities)
include(sub-build/sub-build-config)

####
# Function `run_sub_build`:
#
# Runs a sub-build with the supplied SUB_BUILD_NAME. Output is set in PARENT_SCOPE to the name variable named by OUTPUT.
# The targets registered and run as part of the sub-build are supplied in ARGN.
#
# Targets specified in ARGN **must** define top-level global targets in-order for the sub-build target to build
# correctly. Defining a no-op global target will result in only the generate step performing any useful actions, which
# may, in some cases, be desired.
#
# `SUB_BUILD_NAME`: name of the sub-build and directory to contain its build cache
# `JOBS`: number of jobs to use in sub-build
# `ARGN`: list of targets to run as part of this sub-build. These will be registered, and run in order.
#####
function(run_sub_build SUB_BUILD_NAME)
    if (NOT DEFINED FPRIME_SUB_BUILD_TARGETS)
        message(STATUS "[sub-build] Performing sub-build: ${SUB_BUILD_NAME}")
        _get_call_properties()

        # Run CMake as efficiently as possible
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/sub-build-${SUB_BUILD_NAME}")
        message(STATUS "[sub-build] Generating: ${SUB_BUILD_NAME} with ${ARGN}")
        string(REPLACE ";" "\\;" TARGET_LIST_AS_STRING "${ARGN}")
        execute_process_or_fail("[sub-build] Failed to generate: ${SUB_BUILD_NAME}"
            "${CMAKE_COMMAND}"
            -G "${CMAKE_GENERATOR}"
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "-DFPRIME_SUB_BUILD_TARGETS=${TARGET_LIST_AS_STRING}"
            "-DFPRIME_SKIP_TOOLS_VERSION_CHECK=ON"
            "-DFPRIME_BINARY_DIR=${CMAKE_BINARY_DIR}"
            ${CALL_PROPS}
            RESULT_VARIABLE result
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/sub-build-${SUB_BUILD_NAME}"
        )

        set(BUILD_EXTRA_ARGS)
        # When specified add specific number of jobs to the sub build
        if (DEFINED FPRIME_SUB_BUILD_JOBS)
            list(APPEND BUILD_EXTRA_ARGS "--" "--jobs=${FPRIME_SUB_BUILD_JOBS}")
        # Otherwise specify the MAKEFLAGS variable to speed-up makefile driven systems
        else()
            cmake_host_system_information(RESULT CPU_COUNT QUERY NUMBER_OF_PHYSICAL_CORES)
            if(CPU_COUNT GREATER 0)
                set(ENV{MAKEFLAGS} "--jobs=${CPU_COUNT}")
            endif()
        endif()
        foreach (TARGET IN LISTS ARGN)
            get_filename_component(TARGET_NAME "${TARGET}" NAME_WE)
            message(STATUS "[sub-build] Executing: ${SUB_BUILD_NAME} with ${TARGET_NAME}")
            execute_process_or_fail("[sub-build] Failed to execute: ${SUB_BUILD_NAME}/${TARGET_NAME}"
                "${CMAKE_COMMAND}"
                --build
                "${CMAKE_BINARY_DIR}/sub-build-${SUB_BUILD_NAME}"
                --target
                "${TARGET_NAME}"
                ${BUILD_EXTRA_ARGS}
                RESULT_VARIABLE result
            )
        endforeach()
        message(STATUS "[sub-build] Performing sub-build: ${SUB_BUILD_NAME} - DONE")
    endif()
endfunction(run_sub_build)

####
# Function `_get_call_properties`:
#
# Gets a list of properties to send to the call based on the values of these properties in the current build. This will
# set CALL_PROPS in PARENT_SCOPE and is only intended to be run from within the ,
####
function(_get_call_properties)
    set(CALL_PROPS)
    get_cmake_property(CACHE_VARS CACHE_VARIABLES)
    foreach (PROPERTY IN LISTS CACHE_VARS)
        get_property(CACHE_TYPE CACHE "${PROPERTY}" PROPERTY TYPE)
        # Exclude listed properties and empty properties
        if ("${PROPERTY}" IN_LIST SUB_BUILD_EXCLUDED_CACHE_VARIABLES)
            continue()
            # Exclude empty values
        elseif("${${PROPERTY}}" STREQUAL "")
            continue()
            # Exclude internal cache values
        elseif("${CACHE_TYPE}" IN_LIST TYPES_DISALLOWED_LIST)
            continue()
        endif()
        # Add escaping for list type variables (for pass to function call and then again to another function call)
        string(REPLACE ";" "\\\\;" PROP_VALUE "${${PROPERTY}}" )
        # Check for debugging output
        if (CMAKE_DEBUG_OUTPUT)
            message(STATUS "[sub-build] Adding cache variable: '${PROPERTY}=${PROP_VALUE}'")
        endif()
        set(CALL_PROP "-D${PROPERTY}=${PROP_VALUE}")
        list(APPEND CALL_PROPS "${CALL_PROP}")
    endforeach()
    set(CALL_PROPS "${CALL_PROPS}" PARENT_SCOPE)
endfunction(_get_call_properties)