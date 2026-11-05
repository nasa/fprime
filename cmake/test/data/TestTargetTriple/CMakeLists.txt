####
# TestTargetTriple/CMakeLists.txt:
#
# Minimal project used to exercise `fprime_get_target_triple` from platform.cmake.
####
cmake_minimum_required(VERSION 3.16)
project(TestTargetTriple C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../..")
include(platform/platform)

# Allow tests to simulate cross-compile compiler targets without affecting compiler checks
if (DEFINED TEST_C_COMPILER_TARGET)
    set(CMAKE_C_COMPILER_TARGET "${TEST_C_COMPILER_TARGET}")
endif()
if (DEFINED TEST_CXX_COMPILER_TARGET)
    set(CMAKE_CXX_COMPILER_TARGET "${TEST_CXX_COMPILER_TARGET}")
endif()

fprime_get_target_triple(TARGET_TRIPLE)
message(STATUS "[test] TARGET_TRIPLE=${TARGET_TRIPLE}")
