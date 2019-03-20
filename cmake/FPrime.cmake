####
# FPrime.cmake:
#
# This file is the entry point for building F prime. It does not setup F prime as
# a project, but rather allows the user to build F prime as a library, or as
# part of an external project.  This file includes the cmake build system and 
# sets up the following environment variables used within the build of the
# F prime common components.
#
#  Environment and CMake Variables:
#   1. FPRIME_CORE_DIR: the root directory of F prime's core code
#   2. FPRIME_CURRENT_BUILD_ROOT: BUILD_ROOT for the core F prime code. *MUST* be
#          reset when building custom code outside of source tree.
####

# Defines the FPRIME_CORE_DIR directory to be the core of the F prime build. It is
# used to specify where F prime core lives allowing it to be separate from the
# add-ons used by projects.
set(FPRIME_CORE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
message(STATUS "F´ core directory set to: ${FPRIME_CORE_DIR}")

# BUILD_ROOT is used by F prime to help run the auto-coders, and as a relative
# path from which to do internal operations.
#
#
set(FPRIME_CURRENT_BUILD_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
message(STATUS "F´ BUILD_ROOT currently set to: ${FPRIME_CURRENT_BUILD_ROOT}")

# Library types, used for generating shared objects
#
if (FPRIME_SHARED_LIBS)
    message(STATUS "Generating shared libraries")
    set(FPRIME_LIB_TYPE "SHARED")
else()
    message(STATUS "Generating static libraries")
    set(FPRIME_LIB_TYPE "STATIC")
endif()
# Include the build system and the options that the build system allows.
include("${FPRIME_CORE_DIR}/cmake/CMakeLists.txt")

# In order to generate AC files out-of-source, the ${CMAKE_BINARY_DIR} must
# be included as the AC files will be placed there in a parallel, but separated,
# directory from the source.
if (NOT GENERATE_AC_IN_SOURCE)
    message(STATUS "Running out-of-source AC Generation")
    # Normal (deployment) outputs
    include_directories("${CMAKE_BINARY_DIR}")
    # Core outputs
    include_directories("${CMAKE_BINARY_DIR}/F-Prime")
else()
    message(STATUS "Running in-source AC Generation")
endif()

# Must always include the F prime core directory, as its headers are relative to
# that directory.
include_directories(SYSTEM "${FPRIME_CORE_DIR}")

# Add gtest
include_directories(SYSTEM "${FPRIME_CORE_DIR}/gtest/include")
add_subdirectory("${FPRIME_CORE_DIR}/gtest/" "${CMAKE_BINARY_DIR}/F-Prime")

# Module subdirectories
# Autocoders is first for Cheetah templates
add_subdirectory("${FPRIME_CORE_DIR}/Autocoders/" "${CMAKE_BINARY_DIR}/F-Prime/Autocoders")
add_subdirectory("${FPRIME_CORE_DIR}/Fw/" "${CMAKE_BINARY_DIR}/F-Prime/Fw")
add_subdirectory("${FPRIME_CORE_DIR}/Svc/" "${CMAKE_BINARY_DIR}/F-Prime/Svc")
add_subdirectory("${FPRIME_CORE_DIR}/Os/" "${CMAKE_BINARY_DIR}/F-Prime/Os")
add_subdirectory("${FPRIME_CORE_DIR}/Drv/" "${CMAKE_BINARY_DIR}/F-Prime/Drv")
add_subdirectory("${FPRIME_CORE_DIR}/CFDP/" "${CMAKE_BINARY_DIR}/F-Prime/CFDP")
add_subdirectory("${FPRIME_CORE_DIR}/Utils/" "${CMAKE_BINARY_DIR}/F-Prime/Utils")
