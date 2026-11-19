####
# fprime-util.cmake:
#
# This file contains utility functions for writing the metadata used by `fprime-util` for supporting the standard
# fprime development process.
####
include_guard()

set(FPRIME__INTERNAL_UTILITY_SUFFIX "fprime-util" CACHE INTERNAL "Suffix for fprime-util metadata files" FORCE)
set(FPRIME__INTERNAL_UTILITY_BUILD_TARGETS_FILE "build-targets.${FPRIME__INTERNAL_UTILITY_SUFFIX}" CACHE INTERNAL "File name for fprime-util build targets metadata" FORCE)
set(FPRIME__INTERNAL_UTILITY_SUBDIRECTORY_FILE "sub-directories.${FPRIME__INTERNAL_UTILITY_SUFFIX}" CACHE INTERNAL "File name for fprime-util subdirectory metadata" FORCE)
set(FPRIME__INTERNAL_UTILITY_TESTS_FILE "tests.${FPRIME__INTERNAL_UTILITY_SUFFIX}" CACHE INTERNAL "File name for fprime-util tests metadata" FORCE)
set(FPRIME__INTERNAL_UTILITY_TEST_DIR_FILE "test-dir.${FPRIME__INTERNAL_UTILITY_SUFFIX}" CACHE INTERNAL "File name for fprime-util test directory metadata" FORCE)
set(FPRIME__INTERNAL_UTILITY_SOURCE_LOCATIONS_FILE "fprime-source-locations.${FPRIME__INTERNAL_UTILITY_SUFFIX}" CACHE INTERNAL "File name for fprime-util source locations metadata" FORCE)
set(FPRIME__INTERNAL_UTILITY_BINARY_LOCATIONS_FILE "fprime-binary-locations.${FPRIME__INTERNAL_UTILITY_SUFFIX}" CACHE INTERNAL "File name for fprime-util binary locations metadata" FORCE)
set(FPRIME__INTERNAL_UTILITY_ALL_LOCATIONS_FILE "fprime-locations.${FPRIME__INTERNAL_UTILITY_SUFFIX}" CACHE INTERNAL "File name for fprime-util all locations metadata" FORCE)

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
    file(REMOVE "${CHILD_DIRECTORY}/${FPRIME__INTERNAL_UTILITY_TEST_DIR_FILE}")
endfunction()

####
# Function `fprime_util_metadata_remove`:
#
# Removes the metadata files of a directory with no parent to clear it (project root, directory including fprime) so
# re-configures do not accumulate entries. Removed, not truncated: fprime-util treats missing and empty files differently.
#
# **Parameters:**
# - `DIRECTORY`: binary directory to remove metadata from
####
function(fprime_util_metadata_remove DIRECTORY)
    file(REMOVE
        "${DIRECTORY}/${FPRIME__INTERNAL_UTILITY_BUILD_TARGETS_FILE}"
        "${DIRECTORY}/${FPRIME__INTERNAL_UTILITY_SUBDIRECTORY_FILE}"
        "${DIRECTORY}/${FPRIME__INTERNAL_UTILITY_TESTS_FILE}"
        "${DIRECTORY}/${FPRIME__INTERNAL_UTILITY_TEST_DIR_FILE}"
    )
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

####
# Function `fprime_util_metadata_add_project_build_target`:
#
# Adds a build target to the project-level (root of the build cache) metadata, i.e. the targets built by
# `fprime-util build` at the project root, regardless of the directory F Prime is included from.
#
# - **BUILD_TARGET:** name of the build target to add
####
function(fprime_util_metadata_add_project_build_target BUILD_TARGET)
    file(APPEND "${CMAKE_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_BUILD_TARGETS_FILE}" "${BUILD_TARGET}\n")
endfunction()

####
# Function `fprime_util_metadata_setup_project`:
#
# Sets up the project-level (root of the build cache) metadata: `all` is what `fprime-util build` builds at the project
# root. Runs once per configure; called when a project is registered (`register_fprime_project`) and when the framework
# and library code is set up (`fprime_setup_included_code`), whichever comes first.
####
function(fprime_util_metadata_setup_project)
    get_property(_FPRIME_PROJECT_METADATA_SETUP GLOBAL PROPERTY FPRIME__INTERNAL_PROJECT_METADATA_SETUP)
    if (NOT _FPRIME_PROJECT_METADATA_SETUP)
        set_property(GLOBAL PROPERTY FPRIME__INTERNAL_PROJECT_METADATA_SETUP ON)
        fprime_util_metadata_add_project_build_target("all")
    endif()
endfunction()

####
# Function `fprime_util_metadata_add_project_test`:
#
# Adds a test to the project-level (root of the build cache) metadata, i.e. the tests run by `fprime-util check` at the
# project root (and by `fprime-util check --project` in fprime-tools releases after 4.4.0).
#
# - **TEST_NAME:** name of the test to add
####
function(fprime_util_metadata_add_project_test TEST_NAME)
    file(APPEND "${CMAKE_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_TESTS_FILE}" "${TEST_NAME}\n")
endfunction()

####
# Function `fprime_util_metadata_remove_project_test`:
#
# Removes a test from the project-level (root of the build cache) metadata.
#
# - **TEST_NAME:** name of the test to remove
####
function(fprime_util_metadata_remove_project_test TEST_NAME)
    set(_PROJECT_TESTS_FILE "${CMAKE_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_TESTS_FILE}")
    if (EXISTS "${_PROJECT_TESTS_FILE}")
        file(STRINGS "${_PROJECT_TESTS_FILE}" _PROJECT_TESTS)
        list(REMOVE_ITEM _PROJECT_TESTS "${TEST_NAME}")
        list(JOIN _PROJECT_TESTS "\n" _PROJECT_TESTS_TEXT)
        if (_PROJECT_TESTS)
            string(APPEND _PROJECT_TESTS_TEXT "\n")
        endif()
        file(WRITE "${_PROJECT_TESTS_FILE}" "${_PROJECT_TESTS_TEXT}")
    endif()
endfunction()

####
# Function `fprime_util_metadata_set_test_dir`:
#
# Sets the CTest test directory for the current build directory. This writes a test-dir.fprime-util file that tells
# fprime-util where CTest should look for registered tests. Used when the test enumeration directory differs from the
# CTest test directory (e.g. deployment tests that aggregate component tests from sibling directories).
#
# **Parameters:**
# - `TEST_DIR`: absolute path to use as the CTest --test-dir
####
function(fprime_util_metadata_set_test_dir TEST_DIR)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${FPRIME__INTERNAL_UTILITY_TEST_DIR_FILE}" "${TEST_DIR}")
endfunction()
