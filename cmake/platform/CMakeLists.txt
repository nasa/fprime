####
# platforms:
#
# Platforms in F prime's CMake system setup f prime specific items w.r.t the OS. This file loads those platforms
# file and uses it to setup F prime's build. See: [Platform Template](./platform/platform-template.md) for how to
# generate these files.
#
####
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

# Build platform locations as a list
set(PLATFORM_LOCATIONS ${FPRIME_LIBRARY_LOCATIONS})
list(INSERT PLATFORM_LOCATIONS 0 ${FPRIME_FRAMEWORK_PATH})
list(APPEND PLATFORM_LOCATIONS ${FPRIME_PROJECT_ROOT})

# Loop over all possible platform files
foreach(ROOT IN LISTS PLATFORM_LOCATIONS )
    set(EXPECTED_PLATFORM_FILE "${ROOT}/cmake/platform/${CMAKE_SYSTEM_NAME}.cmake")
    # Include host machine settings
    if (EXISTS "${EXPECTED_PLATFORM_FILE}")
        message(STATUS "Including ${EXPECTED_PLATFORM_FILE}")
        include("${EXPECTED_PLATFORM_FILE}")
    else()
        set(EXPECTED_PLATFORM_FILE "")
    endif()
endforeach()
# Ensure it was found
if ("${EXPECTED_PLATFORM_FILE}" STREQUAL "")
  message(FATAL_ERROR "\n[F-PRIME] No platform config for '${CMAKE_SYSTEM_NAME}'. Please create: '${CMAKE_SYSTEM_NAME}.cmake'\n")
endif()
