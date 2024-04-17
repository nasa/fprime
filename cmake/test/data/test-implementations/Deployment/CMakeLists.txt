####
# 'TestDeployment' Deployment:
#
# A deployment used to test extra functions of the CMake setup.
####

# Base settings
cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)
project(TestImplementation VERSION 1.0.0 LANGUAGES C CXX)

include("${CMAKE_CURRENT_LIST_DIR}/../../../../../cmake/FPrime.cmake")
# NOTE: register custom targets between these two lines
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/TestModule")
set(FPRIME_CURRENT_MODULE Deployment)
set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Main.cpp")
set(MOD_DEPS Fw_Types Deployment/TestModule)
choose_fprime_implementation(Test/Override Test/Override/Override) # Choose an override
choose_fprime_implementation(Os/File Os/File/Posix)
register_fprime_executable()
