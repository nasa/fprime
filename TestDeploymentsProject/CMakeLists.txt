####
# 'TestDeploymentsProject' Project:
#
# This is the project that contains the set of test deployments. This includes:
# - The 'Ref' deployment, which is a historical reference for testing and demonstration
####

# Allow CMake 3.0 - 4.0 projects to set the VERSION variable in the `project` call
cmake_policy(SET CMP0048 NEW)
# CMake basic setup: version consistent with requirements.txt, and define a project 
cmake_minimum_required(VERSION 3.26)
project(Ref VERSION 1.0.0 LANGUAGES C CXX)

# Add compile options used by all build modules
add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    -Wall
    -Wconversion
    -Wdouble-promotion
    -Werror
    -Wextra
    -Wno-unused-parameter
    -Wno-vla
    -Wshadow
    -pedantic
)

# Find the fprime package and include the core codebase
set(FPRIME_INCLUDE_FRAMEWORK_CODE ON)
find_package(FPrime REQUIRED PATHS "${CMAKE_CURRENT_LIST_DIR}/..")

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Ref/")
