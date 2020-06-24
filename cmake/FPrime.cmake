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
#   1. $ENV{FPRIME_LOCATION}: root directory of F prime's core code
#          reset when building custom code outside of source tree.
####

# Defines the FPRIME_CORE_DIR directory to be the core of the F prime build. It is
# used to specify where F prime core lives allowing it to be separate from the
# add-ons used by projects.
get_filename_component(FPRIME_CORE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if (NOT "${FPRIME_CORE_DIR}" STREQUAL "$ENV{FPRIME_LOCATION}")
    message(FATAL_ERROR "Fprime location $ENV{FPRIME_LOCATION} inconsistent with ${FPRIME_CORE_DIR}. Check settings.ini")
endif()
get_filename_component(FPRIME_PROJECT_ROOT "${FPRIME_PROJECT_ROOT}" ABSOLUTE)

message(STATUS "F prime core directory set to: ${FPRIME_CORE_DIR}")
# Clear hashes file
file(REMOVE "${CMAKE_BINARY_DIR}/hashes.txt")

# Set build type, if unser
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE DEBUG)
else()
    string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
endif()
# Store settings filee and register a dependency on it
set(FPRIME_SETTINGS_FILE "$ENV{FPRIME_SETTINGS_FILE}" CACHE PATH "F prime settings file tracking" FORCE)
set(FPRIME_ENVIRONMENT_FILE "$ENV{FPRIME_ENVIRONMENT_FILE}" CACHE PATH "F prime environment file tracking" FORCE)
set(FPRIME_BUILD_LOCATIONS "${FPRIME_CORE_DIR}" $ENV{FPRIME_LIBRARY_LOCATIONS} "${FPRIME_PROJECT_ROOT}")
list(REMOVE_DUPLICATES FPRIME_BUILD_LOCATIONS)
message(STATUS "Searching for F prime modules in: ${FPRIME_BUILD_LOCATIONS}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "$ENV{FPRIME_SETTINGS_FILE}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "$ENV{FPRIME_ENVIRONMENT_FILE}")

# Include the Options, and platform files. These are files that change the build
# setup. Users may need to add items to these files in order to ensure that all
# specific project builds work as expected.
include("${CMAKE_CURRENT_LIST_DIR}/Options.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/platform/CMakeLists.txt")

# Include validation file next, as it checks that everything is in-order
include("${CMAKE_CURRENT_LIST_DIR}/support/validation/Validation.cmake")
# Include the support files that provide all the functions, utilities, and other
# hidden items in the CMake system. Typically a user should not interact with any
# of these files, as they are a library that automates FPrime builds.
include("${CMAKE_CURRENT_LIST_DIR}/support/Executable.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/support/src/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/support/parser/CMakeLists.txt")
include("${CMAKE_CURRENT_LIST_DIR}/support/Executable.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/support/Module.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/support/Utils.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/support/Unit_Test.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/support/Target.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/API.cmake")
if ("${PROJECT_AC_CONSTANTS_FILE}" STREQUAL "" )
    set(PROJECT_AC_CONSTANTS_FILE "${FPRIME_CORE_DIR}/Fw/Cfg/AcConstants.ini")
endif()
message(STATUS "Using autocoder constants file: ${PROJECT_AC_CONSTANTS_FILE}")

# Set the install directory for the package
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR} CACHE PATH "Install dir" FORCE)
endif()
message(STATUS "Installation directory: ${CMAKE_INSTALL_PREFIX}")

# Let user know on the choice of dictionaries
if (GENERATE_HERITAGE_PY_DICT)
    message(STATUS "Generating Heritage Python Dictionaries")
endif()
# Normal (deployment) outputs
include_directories("${CMAKE_BINARY_DIR}")
include_directories("${CMAKE_BINARY_DIR}/F-Prime")

register_fprime_target("${CMAKE_CURRENT_LIST_DIR}/target/dict.cmake")
register_fprime_target("${CMAKE_CURRENT_LIST_DIR}/target/coverage.cmake")
register_fprime_target("${CMAKE_CURRENT_LIST_DIR}/target/impl.cmake")
register_fprime_target("${CMAKE_CURRENT_LIST_DIR}/target/testimpl.cmake")
# Must always include the F prime core directory, as its headers are relative to
# that directory. Same with the project directory for separated projects.
include_directories("${FPRIME_PROJECT_ROOT}")
include_directories(SYSTEM "${FPRIME_CORE_DIR}")
# Ignore GTest for non-test builds
if (${CMAKE_BUILD_TYPE} STREQUAL "TESTING")
    include_directories(SYSTEM "${FPRIME_CORE_DIR}/gtest/include")
endif()
