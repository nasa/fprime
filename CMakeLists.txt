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

# Set default warning flags for all builds
# Specific build modules that do not comply with these flags can disable one or more of them
#
# -Wno-unused-parameter: Disable the unused parameter warning for now. F' has a lot of interfaces,
# so unused method parameters are common in the F prime code base. Eventually all intentionally
# unused parameters should be annotated to avoid this error.
#
# -Wno-vla: Variable length arrays are required to support sending to async serializable
# ports. https://github.com/nasa/fprime/issues/945
#
add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    -pedantic
    -Wall
    -Wextra
    -Wconversion
    -Wdouble-promotion
    -Wshadow
    -Werror
    -Wno-unused-parameter
    -Wno-vla
)
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime-Code.cmake")
