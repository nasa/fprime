####
# 'TestDeployment' Deployment:
#
# A deployment used to test extra functions of the CMake setup.
####

# Base settings
cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)
project(TestConfigDeployment VERSION 1.0.0 LANGUAGES C CXX)

include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime.cmake")
# NOTE: register custom targets between these two lines
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/override/project")

register_fprime_library(
        TestModelOverride
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/TestFPrimeModelOverride.cpp"
    DEPENDS
        project_config
)
register_fprime_library(
        TestHeaderOverride
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/TestFPrimeHeaderOverride.cpp"
    DEPENDS
        project_config
)
register_fprime_library(
        TestFPrimeLibraryOverride
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/TestFPrimeLibraryOverride.cpp"
    DEPENDS
        project_config
)
register_fprime_library(
        TestLibraryNewConfig
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/TestLibraryNewConfig.cpp"
    DEPENDS
        project_config
)