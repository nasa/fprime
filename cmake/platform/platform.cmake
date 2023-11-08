####
# platforms:
#
# Platforms in F prime's CMake system setup f prime specific items w.r.t the OS. This file loads those platforms
# file and uses it to setup F prime's build. See: [Platform Template](./platform/platform-template.md) for how to
# generate these files.
#
####
include_guard()
# Basic definitions
get_filename_component(TOOLCHAIN_NAME "${CMAKE_TOOLCHAIN_FILE}" NAME_WE)
# Setup fallback toolchain name
if ("${TOOLCHAIN_NAME}" STREQUAL "")
    set(TOOLCHAIN_NAME "${CMAKE_SYSTEM_NAME}")
endif()

# Include platform file based on system name
message(STATUS "Target build toolchain/platform: ${TOOLCHAIN_NAME}/${CMAKE_SYSTEM_NAME}")

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${TOOLCHAIN_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${TOOLCHAIN_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${TOOLCHAIN_NAME}")
set(EXPECTED_PLATFORM_FILE "")

# Loop over locations of platform files in order: project, libraries, then framework
foreach(ROOT ${FPRIME_PROJECT_ROOT};${FPRIME_LIBRARY_LOCATIONS};${FPRIME_FRAMEWORK_PATH} )
    set(EXPECTED_PLATFORM_FILE "${ROOT}/cmake/platform/${CMAKE_SYSTEM_NAME}.cmake")
    # Include host machine settings
    if (EXISTS "${EXPECTED_PLATFORM_FILE}")
        message(STATUS "Including ${EXPECTED_PLATFORM_FILE}")
        include("${EXPECTED_PLATFORM_FILE}")
        break()
    endif()
endforeach()
# Ensure the last attempt for the platform file was successful, otherwise error.
if (NOT EXISTS "${EXPECTED_PLATFORM_FILE}")
  message(FATAL_ERROR "\n[F-PRIME] No platform config for '${CMAKE_SYSTEM_NAME}'. Please create: '${CMAKE_SYSTEM_NAME}.cmake'\n")
endif()
