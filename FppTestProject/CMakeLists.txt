###
# FPP Test
# Builds unit tests for the FPP autocoder
# Optimized for readability, maintainability, and optional performance
###

cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)

project(FppTest C CXX)

# --------------------------------------------------
# Include F´ helper CMake scripts
# --------------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime-Code.cmake")

# --------------------------------------------------
# Compiler options: strict warnings and checks
# --------------------------------------------------
add_compile_options(
    -Wall                    # Enable all warnings
    -Wextra                  # Extra warnings
    -Wconversion             # Warn on implicit type conversions
    -Wdouble-promotion       # Warn on float -> double promotion
    -Wshadow                 # Warn on variable shadowing
    -pedantic                # Strict ISO compliance
    -Werror                  # Treat warnings as errors
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast> # C++ only
)

# F´-specific exceptions
add_compile_options(
    -Wno-unused-parameter    # Common in test code
    -Wno-vla                 # Variable-length arrays may appear in test setups
)

# --------------------------------------------------
# Google Test typed tests: conditional compiler flag
# --------------------------------------------------
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-Wno-variadic-macro-arguments-omitted VAR_MAC_OMITTED_SUPPORTED)
if(VAR_MAC_OMITTED_SUPPORTED)
    add_compile_options(-Wno-variadic-macro-arguments-omitted)
endif()

# --------------------------------------------------
# Performance flags for release builds
# --------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto")

# --------------------------------------------------
# Add FPP tests subdirectory
# --------------------------------------------------
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FppTest/")
