####
# CMakeLists.txt:
#
# Build core F prime.
####
cmake_minimum_required(VERSION 3.18)
project(FPrime C CXX)
set(FPRIME_INCLUDE_FRAMEWORK_CODE OFF)
find_package(FPrime REQUIRED PATHS "${CMAKE_CURRENT_LIST_DIR}")
register_fprime_project()

# Validate F Prime against the warning set it publishes. Specific build modules that do not comply
# with these flags can disable one or more of them.
#
# The flags F Prime *requires* in order to build are not listed here: they are usage requirements
# carried by every F Prime target, set up in cmake/flags.cmake.
fprime_add_validation_flags()
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime-Code.cmake")
