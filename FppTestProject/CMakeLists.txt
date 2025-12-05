###
# FPP Test
#
# Builds unit tests for FPP autocoder
###

cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)
project(FppTest C CXX)

include(CheckCXXCompilerFlag)

include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime.cmake")

add_compile_options(
    -Wall
    -Wconversion
    -Wdouble-promotion
    -Werror
    -Wextra
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    -Wshadow
    -pedantic
)

# Required by F Prime
add_compile_options(
    -Wno-unused-parameter
    -Wno-vla
)
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime-Code.cmake")

# Required by Google Test typed tests
check_cxx_compiler_flag(-Wno-variadic-macro-arguments-omitted VAR_MAC_OMITTED_SUPPORTED)
if(VAR_MAC_OMITTED_SUPPORTED)
    add_compile_options(-Wno-variadic-macro-arguments-omitted)
endif()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FppTest/")
