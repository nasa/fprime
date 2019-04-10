####
# DO SOMETHING PLEASE
#
####
if (NOT DEFINED PLATFORM)
    set(PLATFORM "${CMAKE_SYSTEM_NAME}")
endif()

# Include platform file based on system name
message(STATUS "Target build platform: ${PLATFORM}")

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${PLATFORM}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PLATFORM}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${PLATFORM}")


message(STATUS "Including ${CMAKE_CURRENT_LIST_DIR}/${PLATFORM}.cmake")

# Include host machine settings
if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${PLATFORM}.cmake")
  message(FATAL_ERROR "\n[F-PRIME] No platform config for '${PLATFORM}'. Please create: '${CMAKE_CURRENT_LIST_DIR}/${PLATFORM}.cmake'\n")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/${PLATFORM}.cmake")
