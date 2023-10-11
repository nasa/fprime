####
# prescan.cmake:
#
# This file performs a prescan of the fprime source. This prescan looks-up FPP files and writes some other cache files
# in order to prepare for running FPP tools before the full source scan and build. This is done because the FPP tools
# need to do two items before the full build: setup a locations index, and (for runtime efficiency) generate per-module
# dependencies.
#
# This file runs CMake on fprime, but registering only a single target: [prescan](./target/prescan.cmake) that does the
# work of setting up the cache and location information. This CMake build is as reduced as possible, turning off
# compiler checks, running an individual target, but it **must** inherit the same properties as specified in the outer
# full build or the information will be wrong.
####

# Directory in-which to build the prescan directory
set(PRESCAN_DIR "${CMAKE_BINARY_DIR}/prescan")

# Fake STest and GTest in the prescan
if (BUILD_TESTING AND DEFINED FPRIME_PRESCAN)
    add_library(STest "${FPRIME_FRAMEWORK_PATH}/cmake/empty.cpp")
endif()



####
# Function `perform_prescan`:
#
# Perform the prescan by internally running CMake. This call needs to be as fast as possible, and should not require
# doing any actual building. That is, it just generates, using the generate byproducts as a cache to later in the real
# generate and build.
#####
function(perform_prescan)
    message(STATUS "Performing CMake source prescan")
    _get_call_properties()

    # Quiet outputs unless we are doing verbose output generate
    set(OUTPUT_ARGS OUTPUT_QUIET)
    if (CMAKE_DEBUG_OUTPUT)
        set(OUTPUT_ARGS)
    endif()

    # Run CMake as efficiently as possible
    file(MAKE_DIRECTORY "${PRESCAN_DIR}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -G "${CMAKE_GENERATOR}"
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "-DFPRIME_PRESCAN=${CMAKE_BINARY_DIR}"
            "-DCMAKE_C_COMPILER_FORCED=TRUE"
            "-DCMAKE_CXX_COMPILER_FORCED=TRUE"
            "-DFPRIME_SKIP_TOOLS_VERSION_CHECK=ON"
            ${CALL_PROPS}
        RESULT_VARIABLE result
        WORKING_DIRECTORY "${PRESCAN_DIR}"
        ${OUTPUT_ARGS}
    )
    # Check run
    if (NOT "${result}" EQUAL 0)
        message(FATAL_ERROR "Failed to run prescan, cannot continue.")
    endif()
    # Process the results of the above run
    file(READ "${CMAKE_BINARY_DIR}/prescan-fpp-list" FPP_LISTING OFFSET 1) # Skips leading ";" preventing null element
    set_property(GLOBAL PROPERTY FP_FPP_LIST ${FPP_LISTING})
    message(STATUS "Performing CMake source prescan - DONE")
endfunction()
