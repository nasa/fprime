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
register_fprime_executable(
        Deployment
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
    DEPENDS
        Fw_Types Deployment_TestModule
    CHOOSES_IMPLEMENTATIONS
        Test_Override_Override
        Os_File_Posix
)
