####
# FPrime.cmake:
#
# This file is the entry point for building fprime projects, libraries. It does not setup F prime as a project, but
# rather allows the users to build against fprime, fprime libraries while taking advantage of fprime's autocoding
# support. This file includes the cmake build system setup for building like fprime.
####
cmake_minimum_required(VERSION 3.13)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
set_property(GLOBAL PROPERTY FPRIME_MODULES)
# Include the Options, and platform files. These are files that change the build
# setup. Users may need to add items to these files in order to ensure that all
# specific project builds work as expected. Since Options.cmake handles cache
# variables, the path handling is setup in between.
include(options)
include(settings)
include(profile/profile)
# Sets up the build locations of the CMake system. This becomes the root of files
# being searched for in the cmake system.
set(FPRIME_BUILD_LOCATIONS "${FPRIME_FRAMEWORK_PATH}" ${FPRIME_LIBRARY_LOCATIONS} "${FPRIME_PROJECT_ROOT}")
list(REMOVE_DUPLICATES FPRIME_BUILD_LOCATIONS)
message(STATUS "Searching for F prime modules in: ${FPRIME_BUILD_LOCATIONS}")
message(STATUS "Autocoder constants file: ${FPRIME_AC_CONSTANTS_FILE}")
message(STATUS "Configuration header directory: ${FPRIME_CONFIG_DIR}")

include(required)
include(platform/platform)
# Include the support files that provide all the functions, utilities, and other
# hidden items in the CMake system. Typically a user should not interact with any
# of these files, as they are a library that automates FPrime builds.
include(module)
include(autocoder/autocoder)
include(target/target)
include(API)

# Set the install directory for the package
if (DEFINED FPRIME_INSTALL_DEST)
    set(CMAKE_INSTALL_PREFIX ${FPRIME_INSTALL_DEST} CACHE PATH "Install dir" FORCE)
elseif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR CMAKE_INSTALL_PREFIX STREQUAL "")
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/build-artifacts CACHE PATH "Install dir" FORCE)
endif()
message(STATUS "Installation directory: ${CMAKE_INSTALL_PREFIX}")

# Setup the global include directories
include_directories("${CMAKE_BINARY_DIR}")
include_directories("${CMAKE_BINARY_DIR}/F-Prime")

# Must always include the F prime core directory, as its headers are relative to
# that directory. Same with the project directory for separated projects.
include_directories("${FPRIME_PROJECT_ROOT}")
foreach (LIBRARY_DIR IN LISTS FPRIME_LIBRARY_LOCATIONS)
    # Including manifests from libraries
    file(GLOB MANIFESTS RELATIVE "${LIBRARY_DIR}" "${LIBRARY_DIR}/*.cmake")
    message(STATUS "Including library ${LIBRARY_DIR} with manifests ${MANIFESTS}")
    # Check to see if the cmake directory exists and add it
    if (IS_DIRECTORY "${LIBRARY_DIR}/cmake")
        list(APPEND CMAKE_MODULE_PATH "${LIBRARY_DIR}/cmake")
    endif()
    include_directories("${LIBRARY_DIR}")
endforeach()
include_directories("${FPRIME_FRAMEWORK_PATH}")
include_directories("${FPRIME_CONFIG_DIR}")

# Two type of builds are supported: fprime, and fprime-locs
if (FPRIME_FPP_LOCS_BUILD)
    register_fprime_target(target/fpp-locs)
else()
    file(REMOVE "${CMAKE_BINARY_DIR}/hashes.txt")
    message(STATUS "[autocode/fpp] Generating fpp locator file")
    # Using this just to get the target generation functions
    include(target/fpp-locs)
    generate_fpp_locs()
    message(STATUS "[autocode/fpp] Generating fpp locator file -- DONE")

    register_fprime_target(target/noop)
    register_fprime_target(target/version)
    register_fprime_target(target/build)
    register_fprime_target(target/dict)
    register_fprime_target(target/install)
    register_fprime_ut_target(target/ut)

    # fprime-util support targets
    if (FPRIME_ENABLE_UTIL_TARGETS)
        register_fprime_target(target/impl)
        register_fprime_ut_target(target/check)
        register_fprime_ut_target(target/check_leak)
        register_fprime_ut_target(target/coverage)
        register_fprime_ut_target(target/testimpl)
    endif()
endif()
