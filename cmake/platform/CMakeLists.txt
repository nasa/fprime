####
# platforms:
#
# Platforms in F prime's CMake system setup f prime specific items. This file loads those platforms
# file and uses it to setup F prime's build. See: [Platform Template](./platform/platform-template.md) for how to
# generate these files.
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

get_filename_component(TOOLCHAIN_DIR "${CMAKE_TOOLCHAIN_FILE}" DIRECTORY)
if ("${TOOLCHAIN_DIR}" STREQUAL "")
     set(TOOLCHAIN_DIR "${CMAKE_CURRENT_LIST_DIR}")
endif()
set(EXPECTED_PLATFORM_FILE "${TOOLCHAIN_DIR}/../platform/${PLATFORM_NO_SUFFIX}.cmake")
# Include host machine settings
if (EXISTS "${EXPECTED_PLATFORM_FILE}")
  message(STATUS "Including ${EXPECTED_PLATFORM_FILE}")
  include("${EXPECTED_PLATFORM_FILE}")
else()
  message(FATAL_ERROR "\n[F-PRIME] No platform config for '${PLATFORM_NO_SUFFIX}'. Please create: '${EXPECTED_PLATFORM_FILE}'\n")
endif()
