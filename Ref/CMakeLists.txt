####
# 'Ref' Deployment:
#
# This sets up the build for the 'Ref' Application, including the custom reference
# components. In addition, it imports FPrime.cmake, which includes the core F Prime
# components.
####

# Allow CMake 3.0 - 4.0 projects to set the VERSION variable in the `project` call
cmake_policy(SET CMP0048 NEW)
# CMake basic setup: version consistent with requirements.txt, and define a project 
cmake_minimum_required(VERSION 3.26)
project(Ref VERSION 1.0.0 LANGUAGES C CXX)

# Find the fprime package and include the core codebase
set(FPRIME_INCLUDE_FRAMEWORK_CODE ON)
find_package(FPrime REQUIRED PATHS "${CMAKE_CURRENT_LIST_DIR}/..")

add_compile_options(
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    -Wall
    -Wconversion
    -Wdouble-promotion
    -Werror
    -Wextra
    -Wno-unused-parameter
    -Wno-vla
    -Wshadow
    -pedantic
)

# Add subdirectories for the Ref project specific components
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecvBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SendBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SignalGen/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/TypeDemo/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/DpDemo/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/BlockDriver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Top/")

register_fprime_deployment(
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Main.cpp"
    DEPENDS
        ${PROJECT_NAME}_Top
)
# The following compile options will only apply to the deployment executable.
# The extra warnings trigger in core F Prime so we don't apply them there.
target_compile_options("${PROJECT_NAME}" PUBLIC -Wall)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wextra)
target_compile_options("${PROJECT_NAME}" PUBLIC -Werror)
target_compile_options("${PROJECT_NAME}" PUBLIC -pedantic)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wshadow)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wconversion)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wsign-conversion)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wformat-security)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wnon-virtual-dtor)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wold-style-cast)
target_compile_options("${PROJECT_NAME}" PUBLIC -Woverloaded-virtual)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wno-unused-parameter)
target_compile_options("${PROJECT_NAME}" PUBLIC -Wundef)
