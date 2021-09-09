message(STATUS "Searching/installing for sbt")
find_program(SBT sbt)
# Download and unpack googletest at configure time if it doesn't exit already
if (SBT STREQUAL "SBT-NOTFOUND" )
    configure_file("${CMAKE_CURRENT_LIST_DIR}/CMakeLists.sbt.txt.in" sbt-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sbt-download )
    if(result)
      message(FATAL_ERROR "CMake step for sbt failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sbt-download )
    if(result)
      message(FATAL_ERROR "Build step for sbt failed: ${result}")
    endif()
    find_program(SBT sbt PATH ${CMAKE_BINARY_DIR}/sbt/bin)

endif()
message(STATUS "sbt found at: ${SBT}")

