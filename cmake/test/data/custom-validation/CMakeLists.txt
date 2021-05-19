####
# 'Ref' Deployment:
#
# This sets up the build for the 'Ref' Application, including the custom reference
# components. In addition, it imports FPrime.cmake, which includes the core F Prime
# components.
#
# This file has several sections.
#
# 1. Header Section: define basic properties of the build
# 2. F prime core: includes all F prime core components, and build-system properties
# 3. Local subdirectories: contains all deployment specific directory additions
####

##
# Section 1: Basic Project Setup
#
# This contains the basic project information. Specifically, a cmake version and
# project definition.
##
project(UT-Custom-Execs C CXX)
cmake_minimum_required(VERSION 3.5)

##
# Section 2: F prime Core
#
# This includes all of the F prime core components, and imports the make-system. F prime core
# components will be placed in the F-Prime binary subdirectory to keep them from
# colliding with deployment specific items.
##
include("${CMAKE_CURRENT_LIST_DIR}/../../../FPrime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../../../FPrime-Code.cmake")

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/main.cpp"  
)
# Note: supply non-explicit dependencies here
set(MOD_DEPS
    Os
    Fw/Logger
)
register_fprime_executable()
