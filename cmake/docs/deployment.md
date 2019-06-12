**Note:** auto-generated from comments in: ../deployment-CMakeLists.txt.template

## Deployment 'CMakeLists.txt':

F prime deployments setup the most basic CMake settings, include the F prime build system, and
list deployment specific modules. In one or more of these deployment modules, executables should
be registered using `register_fprime_executable`. This is usally done in a `Top` module, but not
strictly required.

To create a deployment, create a `CMakeLists.txt` file following this template structure in a
directory. This `CMakeLists.txt` is buildable as part of CMake. Thus, the user can build the
deployment using the following commands. Unless building unit tests, these commands are the
recommended way of building F prime projects.

**Build Commands**
```
mkdir build_dir
cd build_dir
cmake <path to deployment CMakeLists.txt>
```

This file can be constructed in three sections.

### Section 1: Setup the Basic CMake Infrastructure ###

Section 1 sets up the basic CMake infrastructure. This infrastructure configures CMake by setting
the minimum setup for a CMake project. This requires calling several CMake functions, and setting
a CMake variable.

First, the user must call `project` to setup the project and default deployment executable name.
Also the project languages "C" and "CXX" (C++) must be supplied.

Next, call `cmake_minimum_required` and set VERSION to 3.5 or greater.

Finally, if desired set the `CMAKE_BUILD_TYPE` variable. Used to specify DEBUG/RELEASE builds.
See: https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html

**Example:**
```
project(Ref C CXX)
cmake_minimum_required(VERSION 3.5)
```

### Section 2: Include F prime Core Build System

This section includes the `cmake/FPrime.cmake` file from the root of the F prime library. If this
deployment is treating F prime as a subdirectory or external library, then the
FPRIME_CURRENT_BUILD_ROOT must be set **after** the call to include FPrime-Code.cmake. This allows
the deployment to be treated independently from the F prime core code.

**Example:**
```
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime-Code.cmake")
# Only if external to the core F prime code
set(FPRIME_CURRENT_BUILD_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
message(STATUS "F´ BUILD_ROOT currently set to: ${FPRIME_CURRENT_BUILD_ROOT}")
```
**Note:** if custom targets are desired, then they should be registered between the two includes.

### Section 3: Include Modules and Topologies

The last section is to include all the modules that are specific to this deployment. This uses
the `add_fprime_subdirectory` function to help with adding F prime deployment modules and
keeping the F prime import structure correct.

**Note:** Topology directories are included here.

**Example:**
```
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecvBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SendBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SignalGen/")
# Add Topology subdirectory
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Top/")
```


