# Download and unpack googletest at configure time if it doesn't exit already
if (NOT IS_DIRECTORY "${FPRIME_FRAMEWORK_PATH}/fpp/bin/fpp-depend")
    configure_file("${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt.in" fpp-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fpp-download )
    if(result)
      message(FATAL_ERROR "CMake step for fpp failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fpp-download )
    if(result)
      message(FATAL_ERROR "Build step for fpp failed: ${result}")
    endif()
endif()

# Setup the fpp-tools
find_program(FPP_DEPEND fpp-depend PATH ${CMAKE_BINARY_DIR}/fpp-tools/src/compiler/bin/)
find_program(FPP_TO_XML fpp-to-xml PATH ${CMAKE_BINARY_DIR}/fpp-tools/src/compiler/bin/)
find_program(FPP_TO_CPP fpp-to-cpp PATH ${CMAKE_BINARY_DIR}/fpp-tools/src/compiler/bin/)
