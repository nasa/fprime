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
# Sets up the build locations of the CMake system. This becomes the root of files
# being searched for in the cmake system.
set(FPRIME_BUILD_LOCATIONS "${FPRIME_FRAMEWORK_PATH}" ${FPRIME_LIBRARY_LOCATIONS} "${FPRIME_PROJECT_ROOT}")
list(REMOVE_DUPLICATES FPRIME_BUILD_LOCATIONS)
message(STATUS "Searching for F prime modules in: ${FPRIME_BUILD_LOCATIONS}")
message(STATUS "Autocoder constants file: ${FPRIME_AC_CONSTANTS_FILE}")
message(STATUS "Configuration header directory: ${FPRIME_CONFIG_DIR}")

include(sanitizers) # Enable sanitizers if they are requested
include(required)
include(prescan) #Must come after required if tools detection is to be inherited
include(platform/platform)
# Include the support files that provide all the functions, utilities, and other
# hidden items in the CMake system. Typically a user should not interact with any
# of these files, as they are a library that automates FPrime builds.
include(module)
include(autocoder/autocoder)
include(target/target)
include(API)

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

# To prescan,register target process around safety check
if (DEFINED FPRIME_PRESCAN)
    register_fprime_list_helper(target/prescan FPRIME_TARGET_LIST)
    setup_global_target(target/prescan)
else()
    perform_prescan()
endif()

# FPP locations must come at the front of the list, then build
register_fprime_target(target/fpp_locs)
register_fprime_target(target/build)
register_fprime_build_autocoder(autocoder/fpp)
register_fprime_build_autocoder(autocoder/ai_xml)
register_fprime_build_autocoder(autocoder/packets)
register_fprime_target(target/noop)
register_fprime_target(target/version)
register_fprime_target(target/dict)
register_fprime_target(target/install)
register_fprime_ut_target(target/ut)

if (FPRIME_ENABLE_UTIL_TARGETS)
    register_fprime_target(target/impl)
    register_fprime_ut_target(target/check)
    register_fprime_ut_target(target/check_leak)
    register_fprime_ut_target(target/testimpl)
endif()


