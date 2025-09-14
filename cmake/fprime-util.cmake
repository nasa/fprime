####
# fprime-util.cmake:
#
# This file contains utility functions for writing the metadata used by `fprime-util` for supporting the standard
# fprime development process.
####
include_guard()

set(FPRIME__INTERNAL_UTILITY_SUFFIX "fprime-util")
set(FPRIME__INTERNAL_UTILITY_BUILD_TARGETS_FILE "build-targets.${FPRIME__INTERNAL_UTILITY_SUFFIX}")
set(FPRIME__INTERNAL_UTILITY_SUBDIRECTORY_FILE "sub-directories.${FPRIME__INTERNAL_UTILITY_SUFFIX}")
set(FPRIME__INTERNAL_UTILITY_TESTS_FILE "tests.${FPRIME__INTERNAL_UTILITY_SUFFIX}")

####
# Function `fprime_util_metadata_clear`:
#
# Clears all metadata files written to a subdirectory. This is expected to be called when the subdirectory is added and
# removes all files written to this directory.
#
# **Parameters:**
# - `CHILD_DIRECTORY`: directory to clear metadata from
####
function(fprime_util_metadata_clear CHILD_DIRECTORY)
    file(WRITE "${CHILD_DIRECTORY}/${FPRIME__INTERNAL_UTILITY_BUILD_TARGETS_FILE}" "")
    file(WRITE "${CHILD_DIRECTORY}/${FPRIME__INTERNAL_UTILITY_SUBDIRECTORY_FILE}" "")
    file(WRITE "${CHILD_DIRECTORY}/${FPRIME__INTERNAL_UTILITY_TESTS_FILE}" "")
endfunction()

####
# Function `fprime_util_metadata_add_subdirectory`:
#
# This adds a new subdirectory from a metadata perspective by performing several steps:
# 1. Clears the metadata for the subdirectory
# 2. Adds the subdirectory to the current directory's subdirectory list
#
# **Parameters:**
# - `CHILD_SOURCE_DIRECTORY`: source directory of the subdirectory
# - `CHILD_BINARY_DIRECTORY`: binary directory of the subdirectory
####
function(fprime_util_metadata_add_subdirectory CHILD_SOURCE_DIRECTORY CHILD_BINARY_DIRECTORY)
    fprime_util_metadata_clear("${CHILD_BINARY_DIRECTORY}")
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_SUBDIRECTORY_FILE}" "${CHILD_SOURCE_DIRECTORY}\n")
endfunction()

####
# Function `fprime_util_metadata_add_build_target`:
#
# This adds a new build target from a metadata perspective by adding it to the list in the current directory.
#
# **Parameters:**
# - `BUILD_TARGET`: name of the build target to add
####
function(fprime_util_metadata_add_build_target BUILD_TARGET)
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_BUILD_TARGETS_FILE}" "${BUILD_TARGET}\n")
endfunction()

####
# Function `fprime_util_metadata_add_test`:
#
# This adds a new test from a metadata perspective by adding it to the list in the current directory.
#
# **Parameters:**
# - `TEST_NAME`: name of the test to add
####
function(fprime_util_metadata_add_test TEST_NAME)
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_TESTS_FILE}" "${TEST_NAME}\n")
endfunction()
