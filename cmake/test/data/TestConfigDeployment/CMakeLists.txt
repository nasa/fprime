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

register_fprime_config(
        project_config
    CONFIGURATION_OVERRIDES
        # Test header override from F Prime
        "${CMAKE_CURRENT_LIST_DIR}/DpCfg.hpp"
        # Test FPP override from F Prime
        "${CMAKE_CURRENT_LIST_DIR}/FpConfig.fpp"
        # Test the library source was overridden
        "${CMAKE_CURRENT_LIST_DIR}/config.cpp"
    INTERFACE
)

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