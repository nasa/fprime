# Include platform file based on system name
message(STATUS "Target build system: ${CMAKE_SYSTEM_NAME}")

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_SYSTEM_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${CMAKE_SYSTEM_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${CMAKE_SYSTEM_NAME}")

message(STATUS "Including ${CMAKE_CURRENT_LIST_DIR}/${CMAKE_SYSTEM_NAME}.cmake")

# Include host machine settings
if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${CMAKE_SYSTEM_NAME}.cmake")
  message(FATAL_ERROR "\n[F-PRIME] No platform config for '${CMAKE_SYSTEM_NAME}'. Please create: '${CMAKE_CURRENT_LIST_DIR}/${CMAKE_SYSTEM_NAME}.cmake'\n")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/${CMAKE_SYSTEM_NAME}.cmake")
