####
# TestFindPackageProject:
#
# A test project that uses find_package(FPrime) and register_fprime_project()
# without specifying FPRIME_FRAMEWORK_PATH, FPRIME_LIBRARY_LOCATIONS, or
# FPRIME_PROJECT_ROOT on the command line.
####
cmake_minimum_required(VERSION 3.18)
project(TestFindPackageProject VERSION 1.0.0 LANGUAGES C CXX)

find_package(FPrime REQUIRED PATHS "${CMAKE_CURRENT_LIST_DIR}/../../../..")

register_fprime_project()

# Validate that the library platform file was loaded, not the framework default
if(NOT TEST_PLATFORM_LIBRARY_LOADED)
    message(FATAL_ERROR "Library platform file under lib/ was not loaded. "
        "Expected TEST_PLATFORM_LIBRARY_LOADED to be set by "
        "lib/test-platform-library/cmake/platform/Linux.cmake")
endif()

register_fprime_deployment(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
    DEPENDS
        Fw_Types
)
