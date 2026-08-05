####
# TestFlagsProject:
#
# A project that consumes F Prime through find_package, the way a mission project does, and
# compiles a component whose autocoded dispatch code needs the flags F Prime requires. Building it
# with a strict warning set of its own is what proves those flags travel with the framework.
####
cmake_minimum_required(VERSION 3.18)
project(TestFlagsProject VERSION 1.0.0 LANGUAGES C CXX)

find_package(FPrime REQUIRED PATHS "${CMAKE_CURRENT_LIST_DIR}/../../../..")

register_fprime_project()

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FlagsComponent")
