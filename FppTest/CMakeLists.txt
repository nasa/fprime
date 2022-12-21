###
# FPP Test
#
# Builds unit tests for FPP autocoder
###

cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0048 NEW)
project(FppTest C CXX)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime-Code.cmake")

if (BUILD_TESTING AND NOT __FPRIME_NO_UT_GEN__)
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/enum/")
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/array/")
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/struct/")
endif()

set(SOURCE_FILES "source.cpp")
set(MOD_DEPS
    ${PROJECT_NAME}/enum
    ${PROJECT_NAME}/array
    ${PROJECT_NAME}/struct
)

register_fprime_deployment()
