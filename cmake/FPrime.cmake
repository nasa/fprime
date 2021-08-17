####
# FPrime.cmake:
#
# This file is the entry point for building fprime projects, libraries. It does not setup F prime as a project, but
# rather allows the users to build against fprime, fprime libraries while taking advantage of fprime's autocoding
# support. This file includes the cmake build system setup for building like fprime.
####
file(REMOVE "${CMAKE_BINARY_DIR}/hashes.txt")
# Include the Options, and platform files. These are files that change the build
# setup. Users may need to add items to these files in order to ensure that all
# specific project builds work as expected. Since Options.cmak handles cache
# variables, the path handling is setup in between.
include("${CMAKE_CURRENT_LIST_DIR}/Options.cmake")
# Sets up the build locations of the CMake system. This becomes the root of files
# being searched for in the cmake system.
set(FPRIME_BUILD_LOCATIONS "${FPRIME_FRAMEWORK_PATH}" ${FPRIME_LIBRARY_LOCATIONS} "${FPRIME_PROJECT_ROOT}")
list(REMOVE_DUPLICATES FPRIME_BUILD_LOCATIONS)
message(STATUS "Searching for F prime modules in: ${FPRIME_BUILD_LOCATIONS}")
message(STATUS "Autocoder constants file: ${FPRIME_AC_CONSTANTS_FILE}")
message(STATUS "Configuration header directory: ${FPRIME_CONFIG_DIR}")

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
register_fprime_target("${CMAKE_CURRENT_LIST_DIR}/target/impl.cmake")
register_fprime_target("${CMAKE_CURRENT_LIST_DIR}/target/testimpl.cmake")
register_fprime_target("${CMAKE_CURRENT_LIST_DIR}/target/package_gen.cmake")

register_fprime_ut_target("${CMAKE_CURRENT_LIST_DIR}/target/coverage.cmake")

# Must always include the F prime core directory, as its headers are relative to
# that directory. Same with the project directory for separated projects.
include_directories("${FPRIME_PROJECT_ROOT}")
foreach (LIBRARY_DIR ${FPRIME_LIBRARY_LOCATIONS})
    include_directories("${LIBRARY_DIR}")
endforeach()
include_directories("${FPRIME_FRAMEWORK_PATH}")
include_directories("${FPRIME_CONFIG_DIR}")
