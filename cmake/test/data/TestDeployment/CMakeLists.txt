####
# 'TestDeployment' Deployment:
#
# A deployment used to test extra functions of the CMake setup.
####

# Base settings
cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0048 NEW)
project(TestDeployment VERSION 1.0.0 LANGUAGES C CXX)

include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime.cmake")

register_fprime_build_autocoder("autocoder/test_build_autocoder" FALSE)
register_fprime_build_autocoder("autocoder/test_header_autocoder" FALSE)
register_fprime_target("target/test") # Test target and autocoder supporting it
register_fprime_target("target/test_recursion") # Test recursive dependencies
register_fprime_target("target/test_autocoder") # Test target-triggered autocoder
register_fprime_target("target/test_chained_autocoder") # Test chained autocoder
register_fprime_target("target/test_rerun_autocoder") # Test rerun autocoder

# NOTE: register custom targets between these two lines
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")
run_sub_build(test-sub-build target/sub)

add_fprime_subdirectory(./TestBuildAutocoder)
add_fprime_subdirectory(./TestHeaderAutocoder)
add_fprime_subdirectory(./TestTargetAutocoder)
add_fprime_subdirectory(./TestChainedAutocoder)
add_fprime_subdirectory(./TestRelative)

set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/Main.cpp")
set(MOD_DEPS  Svc_CmdDispatcher TestLibrary_TestComponent TestLibrary2_TestComponent)
register_fprime_deployment()
