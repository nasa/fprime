####
# 'TestConfigConflictDeployment' Deployment:
#
# A deployment used to test that configuration files whose source include path conflicts with
# their build cache include path produce a fatal error.
####

# Base settings
cmake_minimum_required(VERSION 3.18)
cmake_policy(SET CMP0048 NEW)
project(TestConfigConflictDeployment VERSION 1.0.0 LANGUAGES C CXX)

include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime.cmake")
# NOTE: register custom targets between these two lines
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")
# The project root is this directory, thus "config/ConflictCfg.hpp" is available both from the
# source tree and from the build cache: an include path conflict that must produce a fatal error
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/config")
