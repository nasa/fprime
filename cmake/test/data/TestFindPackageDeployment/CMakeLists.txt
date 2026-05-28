####
# TestFindPackageDeployment:
#
# A test deployment that uses find_package(FPrime) and register_fprime_project()
# without specifying FPRIME_FRAMEWORK_PATH, FPRIME_LIBRARY_LOCATIONS, or
# FPRIME_PROJECT_ROOT on the command line.
####
cmake_minimum_required(VERSION 3.18)
project(TestFindPackageDeployment VERSION 1.0.0 LANGUAGES C CXX)

find_package(FPrime REQUIRED)

register_fprime_project()

register_fprime_deployment(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
    DEPENDS
        Fw_Types
)
