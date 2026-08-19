####
# TestPlatformResolution/CMakeLists.txt:
#
# Minimal project used to exercise `fprime_find_platform_file` from platform.cmake.
####
cmake_minimum_required(VERSION 3.16)
project(TestPlatformResolution C CXX)

if (NOT DEFINED FPRIME_FRAMEWORK_PATH)
    message(FATAL_ERROR "FPRIME_FRAMEWORK_PATH must be set")
endif()

list(APPEND CMAKE_MODULE_PATH "${FPRIME_FRAMEWORK_PATH}/cmake")
include(platform/platform)

fprime_find_platform_file()
message(STATUS "[test] PLATFORM_FILE=${FPRIME_CACHED_PLATFORM_FILE}")
