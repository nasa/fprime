####
# CMakeLists.txt:
#
# Build core F prime.
####
cmake_minimum_required(VERSION 3.5)
project(FPrime C CXX)
set(FPRIME_FRAMEWORK_PATH "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Location of F prime framework" FORCE)
set(FPRIME_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Root path of F prime project" FORCE)

# This cmake project is only intended to be used by CI and developers to test F-prime
# We enable -Wall on this particular project as a canary to warn about compilation errors without
# impacting real projects, where a warning from an untested compiler could break the build.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# We disable the buggy -Wstringop-truncation warning, which detects overflows when using strncpy and
# strncat, for the gcc compiler. It triggers false positive warnings on some of our usages of strncpy,
# despite the fact that we're explicitly NULL terminating the end of the buffer. Our usage of strncpy
# should be refactored and eventually removed, but there's no drop-in replacement in the C stdlib, so
# we will simply ignore the false positive warnings for now.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-stringop-truncation")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-truncation")
endif()

# Include the build for F prime.
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime-Code.cmake")
