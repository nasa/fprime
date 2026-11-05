###
# FPP Test
#
# Builds unit tests for FPP autocoder
###

cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0048 NEW)
project(FppTest C CXX)

include(CheckCXXCompilerFlag)

if (FPRIME_ENABLE_DIRECT_PORT_CALLS)
    set(FPRIME_ENABLE_FRAMEWORK_UTS OFF)
    add_compile_options(-DFW_DIRECT_PORT_CALLS=1)
endif ()

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime.cmake")

fprime_add_validation_flags()
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime-Code.cmake")

# Required by Google Test typed tests
check_cxx_compiler_flag(-Wno-variadic-macro-arguments-omitted VAR_MAC_OMITTED_SUPPORTED)
if(VAR_MAC_OMITTED_SUPPORTED)
    add_compile_options(-Wno-variadic-macro-arguments-omitted)
endif()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FppTest/")
