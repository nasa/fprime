####
# platforms:
#
# Platforms in F prime's CMake system setup f prime specific items w.r.t the OS. This file loads those platforms
# file and uses it to setup F prime's build. See: [Platform Template](./platform/platform-template.md) for how to
# generate these files.
#
####
include_guard()
include(API)
# Basic definitions
get_filename_component(TOOLCHAIN_NAME "${CMAKE_TOOLCHAIN_FILE}" NAME_WE)
# Native toolchains use the system name for the toolchain and FPRIME_PLATFORM
if (NOT TOOLCHAIN_NAME)
    set(TOOLCHAIN_NAME "${CMAKE_SYSTEM_NAME}")
    set(FPRIME_PLATFORM "${CMAKE_SYSTEM_NAME}")
# It is an error to use a "Generic" toolchain without setting FPRIME_PLATFORM correctly
elseif (CMAKE_SYSTEM_NAME STREQUAL "Generic" AND NOT FPRIME_PLATFORM)
    message(FATAL_ERROR "Toolchain '${TOOLCHAIN_NAME}' set CMAKE_SYSTEM_NAME to 'Generic' without setting FPRIME_PLATFORM")
# It is an error to set neither of CMAKE_SYSTEM_NAME and FPRIME_PLATFORM
elseif (NOT CMAKE_SYSTEM_NAME AND NOT FPRIME_PLATFORM)
    message(FATAL_ERROR "Toolchain '${TOOLCHAIN_NAME}' should set CMAKE_SYSTEM_NAME to 'Generic' and set FPRIME_PLATFORM")
# Fallback to CMAKE_SYSTEM_NAME when only CMAKE_SYSTEM_NAME is set
elseif (NOT FPRIME_PLATFORM)
    message(WARNING "Toolchain '${TOOLCHAIN_NAME}' should set CMAKE_SYSTEM_NAME to 'Generic' and set FPRIME_PLATFORM")
    set(FPRIME_PLATFORM "${CMAKE_SYSTEM_NAME}")
endif()

# Include platform file based on system name
message(STATUS "Target build toolchain/platform: ${TOOLCHAIN_NAME}/${FPRIME_PLATFORM}")

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${TOOLCHAIN_NAME}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${TOOLCHAIN_NAME}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${TOOLCHAIN_NAME}")
set(EXPECTED_PLATFORM_FILE "")

# Loop over locations of platform files in order: project, libraries, then framework
foreach(ROOT ${FPRIME_PROJECT_ROOT};${FPRIME_LIBRARY_LOCATIONS};${FPRIME_FRAMEWORK_PATH} )
    set(EXPECTED_PLATFORM_FILE "${ROOT}/cmake/platform/${FPRIME_PLATFORM}.cmake")
    # Include host machine settings
    if (EXISTS "${EXPECTED_PLATFORM_FILE}")
        message(STATUS "Including ${EXPECTED_PLATFORM_FILE}")
        include("${EXPECTED_PLATFORM_FILE}")
        break()
    endif()
endforeach()
# Ensure the last attempt for the platform file was successful, otherwise error.
if (NOT EXISTS "${EXPECTED_PLATFORM_FILE}")
  message(FATAL_ERROR "\n[F-PRIME] No platform config for '${FPRIME_PLATFORM}'. Please create: '${FPRIME_PLATFORM}.cmake'\n")
endif()
