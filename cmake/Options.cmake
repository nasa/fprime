####
# Command Line Options:
#
# Options used to configure F prime's CMake system. These options control various actions available
# in the CMake system to help users achieve specific results. Each options has a sensible default
# such that the user need not worry about specifing each correctly.
#
# Typically a user need not specified any of the options to build F prime, however; when
# non-standard build behavior is desired, then these options can be used. These options are
# specified with the -D<OPTION>=<VALUE> flag. Usually the value is "ON" or "OFF". Other values are
# noted if the option differs.

# **Non-standard behavior:**
#
# - Build with build system debigging messages
# - Generate autocoding files in-source
# - Build and link with shared libraries
# - Generate heritage python dictionaries
# - Manually specify the build platform
#
# Note: this file also sets up the following "build type" for use by the user. These build types
#       are in addition to the standard cmake build types.
#
#       TESTING: build the unit tests and setup the "make check" target
# 
# @author mstarch
####

####
# Locations `FPRIME_FRAMEWORK_PATH`, `FPRIME_PROJECT_ROOT`, `FPRIME_LIBRARY_LOCATIONS`
# `FPRIME_AC_CONSTANTS_FILE`, and `FPRIME_CONFIG_DIR`:
#
# These locations specify the locations of the needed F prime items. These are described below
# and each has a default if not set.
# 
# FPRIME_FRAMEWORK_PATH: location of F prime, always the directory above this file, however; 
#                        since it is supplied by the project, it is asserted to be correct.
#
# FPRIME_PROJECT_ROOT: root of the project directory to detect partial paths and include pathes
#                      from.  If not supplied, it is assumed to be FPRIME_FRAMEWORK_PATH if
#                      PROJECT_SOURCE_DIR is a child of FPRIME_FRAMEWORK_PATH otherwise 
#                      PROJECT_SOURCE_DIR is used as the project root.
#
# FPRIME_LIBRARY_LOCATIONS: locations of libraries included in the build. ; separated to be
#                           a cmake list. If not supplied: "" is used.
#
# Additional locations FPRIME_SETTINGS_FILE and FPRIME_ENVIRONMENT_FILE:
#
# These files are used for settings in fprime-util. If suppled (typically only by fprime-util) then
# they will be added as dependencies into the build system.
####
get_filename_component(DETECTED_FRAMEWORK_PATH "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if (DEFINED FPRIME_FRAMEWORK_PATH)
     get_filename_component(FPRIME_FRAMEWORK_PATH "${FPRIME_FRAMEWORK_PATH}" ABSOLUTE)
     # Sanity check the framework path as supplied
     if (NOT FPRIME_FRAMEWORK_PATH STREQUAL DETECTED_FRAMEWORK_PATH)
         message(FATAL_ERROR "Inconsistent FPrime location: ${FPRIME_FRAMEWORK_PATH}. Check settings.ini")
     endif()
endif()
# Force framework path to be absolute
set(FPRIME_FRAMEWORK_PATH "${DETECTED_FRAMEWORK_PATH}" CACHE PATH "F Prime framework location" FORCE)

# Setup project root
get_filename_component(FULL_PROJECT_PATH "${CMAKE_PROJECT_DIR}" ABSOLUTE)
file(RELATIVE_PATH TEMP_PATH "${FPRIME_FRAMEWORK_PATH}" "${FULL_PROJECT_PATH}")
# If defined then force it to be absolute
if (DEFINED FPRIME_PROJECT_ROOT)
    get_filename_component(FPRIME_PROJECT_ROOT_ABS "${FPRIME_PROJECT_ROOT}" ABSOLUTE)
    set(FPRIME_PROJECT_ROOT "${FPRIME_PROJECT_ROOT_ABS}" CACHE PATH "F Prime project location" FORCE)
# Forces framework path as project root, if a child
elseif( "${TEMP_PATH}" MATCHES "^[^./].*" )
    set(FPRIME_PROJECT_ROOT "${FPRIME_FRAMEWORK_PATH}" CACHE PATH "F Prime project location" FORCE)
# Force PROJECT_ROOT
else()
    set(FPRIME_PROJECT_ROOT "${FULL_PROJECT_PATH}" CACHE PATH "F Prime project location" FORCE)
endif()
# Force  FPRIME_LIBRARY_LOCATIONS to be absolute
set(FPRIME_LIBRARY_LOCATIONS_ABS)
foreach (LIBLOC  ${FPRIME_LIBRARY_LOCATIONS})
    get_filename_component(LIBLOC_ABS "${LIBLOC}" ABSOLUTE)
    list(APPEND FPRIME_LIBRARY_LOCATIONS_ABS "${LIBLOC_ABS}")
endforeach()
set(FPRIME_LIBRARY_LOCATIONS "${FPRIME_LIBRARY_LOCATIONS_ABS}" CACHE STRING "F prime library locations" FORCE)
# Add in addition file dependencies from CMake
if (DEFINED FPRIME_SETTINGS_FILE)
    get_filename_component(FPRIME_SETTINGS_FILE  "${FPRIME_SETTINGS_FILE}" ABSOLUTE)
    set(FPRIME_SETTINGS_FILE "${FPRIME_SETTINGS_FILE}" CACHE PATH "F prime settings file" FORCE)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${FPRIME_SETTINGS_FILE}")
endif()
if (DEFINED FPRIME_ENVIRONMENT_FILE)
    get_filename_component(FPRIME_ENVIRONMENT_FILE  "${FPRIME_ENVIRONMENT_FILE}" ABSOLUTE)
    set(FPRIME_ENVIRONMENT_FILE "${FPRIME_ENVIRONMENT_FILE}" CACHE PATH "F prime environment file" FORCE)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${FPRIME_ENVIRONMENT_FILE}")
endif()
# Override the AC constants file when specified
if (NOT DEFINED FPRIME_AC_CONSTANTS_FILE)
    set(FPRIME_AC_CONSTANTS_FILE "${FPRIME_FRAMEWORK_PATH}/config/AcConstants.ini" CACHE PATH "F prime AC constants.ini file" FORCE)
endif()
# Settings for F config directory
if (NOT DEFINED FPRIME_CONFIG_DIR)
    set(FPRIME_CONFIG_DIR "${FPRIME_FRAMEWORK_PATH}/config/" CACHE PATH "F prime configuration header directory" FORCE)
endif()

####
# `CMAKE_DEBUG_OUTPUT:`
#
# Turns on the reporting of debug output of the CMake build. Can help refine the CMake system,
# and repair errors. For normal usage, this is not necessary.
#
# **Values:**
# - ON: generate debugging output
# - OFF: (default) do *not* generate debugging output
#
# e.g. `-DCMAKE_DEBUG_OUTPUT=ON`
####
option(CMAKE_DEBUG_OUTPUT "Generate F prime's debug output while running CMake" OFF)

####
# `GENERATE_HERITAGE_PY_DICT:`
#
# This option switches from generating XML dictionaries to generating the heritage python
# dictionatries. This enables backward compatible use with the older Tk GUI and other tools that
# use python fragment dictionaries.
#
# **Values:**
# - ON: generate python dictionaries.
# - OFF: (default) generate XML dictionaries.
#
# e.g. `-DGENERATE_HERITAGE_PY_DICT=ON`
####
option(GENERATE_HERITAGE_PY_DICT "Generate F prime python dictionaries instead of XML based dictionaries." OFF)

####
# `SKIP_TOOLS_CHECK:`
#
# For older clients, the check that validates the tool-suite is installed may fail. This option
# skips the tools check enabling the system to run.
#
# **Values:**
# - ON: skip tools check
# - OFF: (default) run tools check
#
# e.g. `-DSKIP_TOOLS_CHECK=ON`
####
option(SKIP_TOOLS_CHECK "Skip the tools check for older clients." OFF)


# Set build type, if unser
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE DEBUG)
else()
    string(TOUPPER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
endif()

####
# `TESTING:`
#
# Testing build type used to build UTs and setting up the `make check` target. If the unit testing
# is desired run with this build type.
#
# e.g. `-DCMAKE_BUILD_TYPE=TESTING`
####
SET(CMAKE_CXX_FLAGS_TESTING "-g -DBUILD_UT -DPROTECTED=public -DPRIVATE=public -DSTATIC= -fprofile-arcs -ftest-coverage"
    CACHE STRING "Testing C++ flags." FORCE)
SET(CMAKE_C_FLAGS_TESTING "-g -DBUILD_UT -DPROTECTED=public -DPRIVATE=public -DSTATIC= -fprofile-arcs -ftest-coverage"
    CACHE STRING "Testing C flags." FORCE)
SET(CMAKE_EXE_LINKER_FLAGS_TESTING "" CACHE STRING "Testing linker flags." FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS_TESTING "" CACHE STRING "Testing linker flags." FORCE)
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_TESTING
    CMAKE_C_FLAGS_TESTING
    CMAKE_EXE_LINKER_FLAGS_TESTING
    CMAKE_SHARED_LINKER_FLAGS_TESTING )
# Testing setup for UT and coverage builds
if (CMAKE_BUILD_TYPE STREQUAL "TESTING" )
    # These two lines allow for F prime style coverage. They are "unsupported" CMake features, so beware....
    set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
    set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
endif()
####
# `PLATFORM:`
#
# Specifies the platform used when building the F prime using the CMake system. See:
# [platform.md](platform.md) for more information.
####
