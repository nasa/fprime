# Include platform file based on system name
message(STATUS "Target build system: ${CMAKE_SYSTEM_NAME}")

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_SYSTEM_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}")

message(STATUS "Including ${CMAKE_CURRENT_LIST_DIR}/${CMAKE_SYSTEM_NAME}.cmake")

# TODO: cleaner cross-compile and 
#       
# Include host machine settings
include("${CMAKE_CURRENT_LIST_DIR}/${CMAKE_SYSTEM_NAME}.cmake")

### Include common headers for cross compilation build
##if(${CMAKE_SYSTEM_NAME} STREQUAL "arm-linux-gnueabihf")
##  # Include common linux settings
##  include("${CMAKE_CURRENT_LIST_DIR}/Linux-common.cmake")
##endif()

### Commands to regenerate build system for given platform
##add_custom_target(
##  BUILD_HOST
##  rm "${CMAKE_SOURCE_DIR}/CMakeCache.txt" # Remove cache to wipe out compiler settings
##  COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/CMakeFiles" # Remove rest of build system
##)

##add_custom_target(
##  BUILD_PI
##  ${CMAKE_COMMAND} -E remove "${CMAKE_SOURCE_DIR}/CMakeCache.txt" # Remove cache to wipe out compiler settings
##  COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/CMakeFiles" # Remove rest of build system
##  COMMAND ${CMAKE_COMMAND} "${CMAKE_SOURCE_DIR}" "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/platform/arm-linux-gnueabihf.cmake"
##)
