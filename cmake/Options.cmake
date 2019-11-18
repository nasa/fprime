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
# `GENERATE_AC_IN_SOURCE:`
#
# Allows for generating auto-coded files in the source tree. Typically, this is frowned upon as
# this prevents using F prime as a library. However, this is necessary to support historic
# project development practices.
#
# **WARNING:** due to the limitations of the autocoder, currently the CMake system generates
#              in-source and moves the files out.
# 
# **Values:**
# - ON: generate auto-coding files in source-tree.
# - OFF: (default) generate auto-coding files in the build-tree.
#
# e.g. `-DGENERATE_AC_IN_SOURCE=ON`
####
option(GENERATE_AC_IN_SOURCE "Ac.cpp and Ac.hpp are placed with source files." OFF)

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


# Note: document other system options here.


####
# `TESTING:`
#
# Testing build type used to build UTs and setting up the `make check` target. If the unit testing
# is desired run with this build type.
#
# e.g. `-DCMAKE_BUILD_TYPE=TESTING`
####
SET(CMAKE_CXX_FLAGS_TESTING "-g -DBUILD_UT -DPROTECTED=public -DPRIVATE=public -fprofile-arcs -ftest-coverage"
    CACHE STRING "Testing C++ flags." FORCE)
SET(CMAKE_C_FLAGS_TESTING "-g -DBUILD_UT -DPROTECTED=public -DPRIVATE=public -fprofile-arcs -ftest-coverage"
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
