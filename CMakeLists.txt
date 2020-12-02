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
#
# We test -Wall with -Wstringop-truncation turned off, which detects overflows when using strncpy and strncat.
# Unfortunately, this warning is buggy in gcc. It triggers false positive warnings on some of our usages
# of strncpy, despite the fact that we're explicitly NULL terminating the end of the buffer. Our usage
# of strncpy should be refactored and eventually removed, but there's no drop-in replacement in the C stdlib,
# so we will simply ignore the false positive warnings for now.
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-stringop-truncation -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-stringop-truncation -Werror")

# Include the build for F prime.
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime-Code.cmake")
