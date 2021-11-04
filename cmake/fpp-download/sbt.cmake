message(STATUS "[fpp-tools] Searching for sbt")
find_program(SBT sbt)
# Download and unpack googletest at configure time if it doesn't exit already
if (NOT SBT)
    message(STATUS "[fpp-tools] Installing sbt, this will take a few minutes")
    configure_file("${CMAKE_CURRENT_LIST_DIR}/CMakeLists.sbt.txt.in" sbt-download/CMakeLists.txt)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sbt-download
        OUTPUT_QUIET
        ERROR_VARIABLE ERROR_OUT
    )
    if(result)
        message(FATAL_ERROR "CMake step for sbt failed: ${result}\n${ERROR_OUT}")
    endif()
    execute_process(
        COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sbt-download
        OUTPUT_QUIET
        ERROR_VARIABLE ERROR_OUT
    )
    if(result)
        message(FATAL_ERROR "Build step for sbt failed: ${result}\n${ERROR_OUT}")
    endif()
    find_program(SBT sbt PATH ${FPP_TOOLS_PATH}/sbt/bin)
endif()
message(STATUS "[fpp-tools] sbt found at: ${SBT}")
