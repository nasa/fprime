####
# CMakeLists.txt:
#
# Build core F prime.
####
cmake_minimum_required(VERSION 3.5)
project(FPrime C CXX)

# Include the build for F prime.
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/FPrime-Code.cmake")