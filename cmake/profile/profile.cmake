
option(FPRIME_PROFILE_CMAKE "Enable profiling of the CMake system." ${CMAKE_DEBUG_OUTPUT})

set(PROFILER_OUTPUT "${CMAKE_BINARY_DIR}/fp_profiling")
set(PROFILER_SETUP_DIR "${CMAKE_BINARY_DIR}/profiler-tooling")

function(setup_profiling)
    make_directory(${PROFILER_SETUP_DIR})
    file(REMOVE "${PROFILER_OUTPUT}")
    find_program(PROFILER profiler PATHS ${PROFILER_SETUP_DIR})
    if (NOT FPRIME_PROFILE_CMAKE OR PROFILER)
        return()
    endif()
    message(STATUS "[profiler] Building the profiler C application")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" ${CMAKE_CURRENT_LIST_DIR}
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${PROFILER_SETUP_DIR}
        OUTPUT_QUIET
        ERROR_VARIABLE ERROR_OUT
    )
    if(result)
        message(FATAL_ERROR "[profiler] configure step failed: ${result}\n${ERROR_OUT}")
    endif()
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${PROFILER_SETUP_DIR}
        OUTPUT_QUIET
        ERROR_VARIABLE ERROR_OUT
    )
    if(result)
        message(FATAL_ERROR "[profiler] build step failed: ${result}\n${ERROR_OUT}")
    endif()
    find_program(PROFILER profiler PATHS ${PROFILER_SETUP_DIR})
endfunction(setup_profiling)
setup_profiling() # Set-up profiling at the beginning

function(fp_profile STAGE TOKEN ANNOTATION)
    if (NOT FPRIME_PROFILE_CMAKE)
        return()
    endif()
    find_program(PROFILER profiler PATHS ${PROFILER_SETUP_DIR})
    execute_process(COMMAND ${PROFILER} "${PROFILER_OUTPUT}" "${STAGE}" "${TOKEN}" "${ANNOTATION}")
endfunction(fp_profile)

