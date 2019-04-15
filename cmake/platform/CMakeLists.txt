####
# platforms:
#
# Platforms in F prime's CMake system setup f prime specific items. This file loads those platforms
# file and uses it to setup F prime's build. See: platform.cmake.template for how to generate these
# files.
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

string(REGEX REPLACE ".cmake" "" PLATFORM_NO_SUFFIX "${PLATFORM}")

message(STATUS "Including ${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NO_SUFFIX}.cmake")

# Include host machine settings
if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NO_SUFFIX}.cmake")
  message(FATAL_ERROR "\n[F-PRIME] No platform config for '${PLATFORM_NO_SUFFIX}'. Please create: '${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NO_SUFFIX}.cmake'\n")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/${PLATFORM_NO_SUFFIX}.cmake")
