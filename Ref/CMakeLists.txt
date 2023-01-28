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
cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0048 NEW)
project(Ref VERSION 1.0.0 LANGUAGES C CXX)

##
# Section 2: F prime Core
#
# This includes all of the F prime core components, and imports the make-system. F prime core
# components will be placed in the F-Prime binary subdirectory to keep them from
# colliding with deployment specific items.
##
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime.cmake")
# NOTE: register custom targets between these two lines
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime-Code.cmake")
##
# Section 3: Components and Topology
#
# This section includes deployment specific directories. This allows use of non-
# core components in the topology, which is also added here.
##
# Add component subdirectories
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecvBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SendBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SignalGen/")

# Add Topology subdirectory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Top/")

set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Main.cpp")
set(MOD_DEPS ${PROJECT_NAME}/Top)

register_fprime_deployment()
# The following compile options will only apply to the deployment executable.
# The extra warnings trigger in core F Prime so we don't apply them there.
target_compile_options("${PROJECT_NAME}" PUBLIC -Wall)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wextra)
target_compile_options("${PROJECT_NAME}" PUBLIC -Werror)
#target_compile_options("${PROJECT_NAME}" PUBLIC -Wshadow)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wconversion)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wsign-conversion)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wformat-security)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wnon-virtual-dtor)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wold-style-cast)
target_compile_options("${PROJECT_NAME}" PUBLIC -Woverloaded-virtual)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wno-unused-parameter)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wundef)
set_property(TARGET "${PROJECT_NAME}" PROPERTY CXX_STANDARD 11)
