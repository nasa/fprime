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
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime-Code.cmake")

add_compile_options(
    -Wall
    -Wconversion
    -Wdouble-promotion
    -Werror
    -Wextra
    -Wold-style-cast
    -Wshadow
    -pedantic
)

# Required by F Prime
add_compile_options(
    -Wno-unused-parameter
    -Wno-vla
)

# Required by Google Test typed tests
check_cxx_compiler_flag(-Wno-variadic-macro-arguments-omitted VAR_MAC_OMITTED_SUPPORTED)
if(VAR_MAC_OMITTED_SUPPORTED)
    add_compile_options(-Wno-variadic-macro-arguments-omitted)
endif()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/array/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/component/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/dp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/state_machine/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/enum/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/interfaces/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/struct/")

set(SOURCE_FILES "source.cpp")
set(MOD_DEPS
    ${PROJECT_NAME}/array
    ${PROJECT_NAME}/component/active
    ${PROJECT_NAME}/component/empty
    ${PROJECT_NAME}/component/passive
    ${PROJECT_NAME}/component/queued
    ${PROJECT_NAME}/dp
    ${PROJECT_NAME}/enum
    ${PROJECT_NAME}/interfaces
    ${PROJECT_NAME}/state_machine/external_instance
    ${PROJECT_NAME}/state_machine/internal/initial
    ${PROJECT_NAME}/state_machine/internal/state
    ${PROJECT_NAME}/state_machine/internal_instance/initial
    ${PROJECT_NAME}/state_machine/internal_instance/state
    ${PROJECT_NAME}/struct
)

register_fprime_deployment()
