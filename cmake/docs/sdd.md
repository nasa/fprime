# CMake Build System

## 1 Introduction

This document describes the build system based on the popular open source tool CMake
![https://cmake.org/](https://cmake.org/).  The software design document will present a set of
requirements for the build system, some operational concepts, and the candidate build system design.
This build system is intended to replace the legacy F´ build system which is difficult to enhance
and maintain.  The legacy F´ build system was inherited from the JPL Mars Science Laboratory
mission and created over 10 years ago.

## 1.1 Definitions

These terms have specific meaning in this SDD. This table gives a quick reference for the reader.

| Term | Meaning |
|---|---|
| **Host** | Machine or architecture used to build the code. |
| **Target** | Machine or architecture for which the code is built. |
| **Build Commands** | Commands run through the make system. |
| **Out-Of-Source Build** | Generate build artifacts in a separate directory from source code. |
| **Build Configurations** | Different build setups, like different targets, debug flags, different deployments. Typically these are isolated from one another. |
| **F´ Module** | Super set of F´ components and F´ ports. |
| **Deployments** | F´ binary/executable containing framework intended to run as F´. |
| **Executable** | Binary built with the build system, not intended to run as F´ deployment |

## 2 Requirements

Requirement | Description | Rationale | Verification Method |
---- | ---- | ---- | ----
BUILD-01 | The build system shall support native F´ builds on Linux, and Mac OS. | F´ development at JPL takes place on Linux and Mac OS machines | Unit-Test |
BUILD-02 | The build system shall provide templates for supporting various targets. | Templates make adding new targets easier. | Inspection |
BUILD-03 | The build system shall provide a cross-compiled target example. | Cross-compilation is common at JPL and must be provided as an example. | Inspection |
BUILD-04 | The build system shall support custom build commands. | Custom build commands allow for extension of the build system. | Unit-Test |
BUILD-05 | The build system shall support individual component, port, and topology builds. | Compiling a specific component can speed-up development. | Unit Test |
BUILD-06 | The build system shall support unit test building and running system checks. | Unit testing is critical for proper development. | Unit Test |
BUILD-07 | The build system shall support building deployments. | Deployments must build properly | Unit Test |
BUILD-08 | The build system shall not require specific ordering of all modules to build properly. | Ordering of all F´ is difficult when it must be explicit. Note: deployments are exempt. | Unit-Test |
BUILD-09 | The build system shall support separate out-of-source building of F´ | Build artifacts are commonly kept separate from source code. | Inspection |
BUILD-10 | The build system shall support executable and tool building | Not all of F´ is a deployment | Inspection |
BUILD-11 | The build system shall support installation and distribution of outputs, headers, and libraries | Shipping of binary outputs is important for projects. | Inspection |
BUILD-12 | The build system shall support user-configurable builds i.e. debug, release, etc. | F´ may need access to different build variants for debugging purposes | Inspection |
BUILD-13 | The build system shall be easy to use including adding new components | F´'s current build system has a steep learning curve | Inspection |
BUILD-14 | The build system shall not be explicitly slow. | Compilation times are non-trivial | Inspection |
BUILD_15 | Deployments shall configure dependencies independently. | Current F´ has issues with global make config directory | Inspection |
BUILD_16 | The build system shall not be difficult to set up and configure. | Porting existing F´ deployments to the new make system should not require massive efforts | Inspection |
BUILD_17 | The build system shall support treating F´ as a sub-repo, and sub-directory even if F is read-only | Future F´ usage should treat core as an input | Inspection |
BUILD_18 | The build system shall support building F´ core as a set of shared libraries. | Some future missions may benefit from shared F´ core. | Unit-Test |
BUILD_19 | The build system shall support execution of individual, sets, or all gtest based unit tests. | | |
BUILD_20 | The build system shall support execution of the F´ Autocoder. | | Unit-Test |
BUILD_21 | The build system shall verify that required compilers, linkers, libraries, etc. are installed on host where build is being executed. | | Unit-Test |
BUILD_22 | The build system shall support execution of individual, sets, or all F´ Autocoder and associated tooling tests. | | |
BUILD_24 | The build system shall support execution of custom Autocoders. | | Unit-Test |

## 3 Operations Concepts

In the build system, F´ is included as a sub directory to the project. All adaptations are kept out of
the F´ directory, allowing for streamlined F´ update, patching, and freezing of the F´ core
components. This could easily be extended to treat F´ as a library and link against it. Here builds
are performed in user supplied directories. The user creates a named directory to build into, and
then supplies cmake the configuration arguments to setup the build properly.

![F´ As Sub Repository](img/CMake%20Ops%20-%20Recommended.png "F´ As Sub Repository")

As can be seen here, the adaptations and core top-level directory live alongside one another. The
builds go into separate build directories, one for each configuration. This can be setup with the
following commands. Setup needs to be done once and only once for each build type. After that, one
can call make over-and-over.

```
cd <project>
mkdir build_config1
cd build_config1
cmake ../<path-to-deployment> -D<configuration settings>
```

Then when code or make changes occur, this configuration can easily be rebuilt by performing the
following commands (as many times as needed during development iteration).

```
cd <project>/build_config1
make
```

For building individual components, please see section "3.1, *out-of-source* building of individual
components".


### 3.1 Building of Individual Components

In order to build individual components inside F´ when using out-of-source builds, one must change
into the parallel build structure to find the components build directory. F´ core components live in
`<build>/F-Prime/<path-to-component>` and adaptations typically live in
`<build>/<adaptation>/<path-to-component>`.  The user can then issue make in this directory to build
the component.  A reference app example can be seen in the commands below.

**Build F´ Svc Component Individually**

```
cd build_config1/F-Prime/Svc/FatalHandler
make
```

**Build Ref Component Individually**

```
cd build_config1/Ref/PingReceiver/
make
```

### 3.2 Adding in New Components and Topologies

New components can be added by creating a `CMakeLists.txt` file in the directory of that component.
This file is required to call one of the `register_fprime_*` functions passing in directives to control
the source list.

**Module CMakeLists.txt**

```
register_fprime_module(
  AUTOCODER_INPUTS
      "${CMAKE_CURRENT_LIST_DIR}/PingReceiver.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PingReceiver.cpp"
)
```

This file must be added to some parent CMakeLists.txt file using the add_subdirectory file. If it
is a new F´ component this is typically added to `CMakeLists.txt` in the top-level directory this
component goes in. i.e. `fprime/Svc/CMakeLists.txt`. These sub-CMakeLists.txt are there as a
convenience and are included themselves in FPrime.cmake.

**Example Add Subdirectory**

```
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
```

## 4 CMake Build Organization

The CMake system is organized into three pieces. There are the entry-point files supplied by the
deployment or executable, F´ core CMake support files, and the CMakeLists.txt files used to build
core and adaptation F´ components and topologies. These pieces are respectively yellow, green, and
blue/orange in the diagram below.

![F´ CMake File Organization](img/CMake%20File%20Organization.png "F´ CMake File Organization")

### 4.1 Deployment and Executable CMake Files

These files are supplied by the deployment or executable being built. This is typically supplied
by the adaptation project of F´. These files supply two critical functions. Primarily, this must
supply an entry-point of the build system. It contains the standard CMake headers and an inclusion
of the F´ CMake support file `FPrime.cmake` it should also include `FPrime-Code.cmake` to include
the F´ core code. This ensures CMake is ready to run, and all the F´ setup is included.
This should look something like the following:

**CMake Headers and F´ Build System**

```
##
# Section 1: Basic Project Setup
#
# This contains the basic project information. Specifically, a cmake version and
# project definition.
##
cmake_minimum_required(VERSION 3.16)
project(FPrime-Ref C CXX)

##
# Section 2: F´ Core
#
# This includes all of the F´ core components, and imports the make-system. F´ core
# components will be placed in the F-Prime binary subdirectory to keep them from
# colliding with deployment specific items.
##
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../cmake/FPrime-Code.cmake")
```

The secondary function of this file is to include any sub directories that contain adaptation
specific F´ components. This is just like the F´ core sub directory inclusions as described below,
but represent adaptation specific components. To see what these files look like, see section 4.3.

**Including Sub Directories**

```
##
# Section 3: Components and Topology
#
# This section includes deployment specific directories. This allows use of non-
# core components in the topology, which is also added here.
##
# Add component subdirectories
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecvBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SendBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SignalGen/")
```
**Note:** the output of the components are routed to named sub-directories of the build, starting
with the `Ref` prefix.


### 4.2 F´ Core CMake Support Files

These files provide the core CMake functions used to make components, deployments, and modules.
In addition `FPrime-Code.cmake` includes the sub directories that compose F´ core components. In
that way deployments need only include the one CMake file to import all of F´. Functions that
automate the autocoder function, module dependencies, and various other utilities are included as well.
Thus deployments and executables can follow the same pattern as core F´ components when adding
custom components of their own.

### 4.3 F´ Core and Adaptation CMakeLists.txt Files

These files are used to specify the build layout of the F´ components, ports, and topologies. They
are composed in a hierarchy as shown below. These files call the F´ module and deployment functions
to generate the expected build files. The file format is described in section *3.2 Adding in New
Components and Topologies*. These files link source files, and autocoder inputs to the generate
functions. These functions assemble F´ from those constituents.

![F´ CMake Lists Hierarchy](img/CMake%20Lists%20Hierarchy.png "F´ CMake File Organization")

## 5 CMake Architecture

As can be seen thus far, most of the F´ build magic is encapsulated in CMake utility functions. This
section will describe the primary functions and how they setup the F´ build. There are two primary
functions sets in this architecture. Each function set has the raw function, which performs the
generation and the other which wraps it. `register_*` functions are the API wrappers, and the
`generate_*` functions perform the work. The functions are:

1. register_fprime_module; generate_module: generates library/module build files and dependencies.
2. register_fprime_executable; generate_executable: generates executable build and dependencies.

![F´ CMake Architecture](img/CMake%20-%20Architecture.png "F´ CMake Architecture")

**Note:** colors are inherited from above diagrams. Red items are output products, and purple items
represent execution steps.

### 5.1 Module Functions: register_fprime_module and generate_module

Module functions are designed to create a library (static or shared) out of an F´ directory. The
module name is determined from the directory path relative to the root of the repository.
i.e. `Fw/Comp` becomes *Fw_Comp* and yields `libFw_Comp.a`. This library is added as an output of
the CMake system. This library is built from `SOURCES` and `AUTOCODER_INPUTS`. Specific dependencies can be supplied using
the `DEPENDS` directive, but is only required when the dependencies are not detected via the model
dependency tree. The autocoder generation steps are registered
for each of the autocoder input files. These autocoder input files are also used to detect all
dependencies of the given module. The normal source files are supplied to the build directory.

Once these steps have completed, CMake generates host-specific build files that encapsulate the
autocoder calls for the module, the dependencies of the module, and the sources for the module.
Running these build files will generate the module's library for use within the deployment linking
stage and register it with CMake so it can be used with the global dependency roll-up.

### 5.2 Executable Functions: register_fprime_executable and generate_executable

Executable functions are designed to create an executable. These executables have some concrete
sources and roll-up all dependencies into a global ordered list of dependencies automatically. The
executable name comes from the defined "${PROJECT_NAME}" when creating the CMake project, or a
separate name must be supplied. The executable may supply `SOURCES` and `DEPENDS`, use
autocoding, and otherwise acts similar to the above module with the exception that an executable
output is the result of the build.

**Note:** deployments specify one or more executables, and these executables become the root of the
dependency tree. Thus, only the needed executables, libraries, and outputs are generated and the
complete F´ system is not explicitly built. This makes the build more efficient.

### 5.3 Unit Test Functions: register_fprime_ut

Registering unit tests uses the same process as above with the exception that the variables
`SOURCES` and `DEPENDS`. This allows the same file to define both a module or
executable and unit test without overriding previously used variables.


**Example UT Build**

```
mkdir build_ut
cd build_ut
cmake .. -DBUILD_TESTING=ON
make -j32
```

### 5.4 Adding New Platforms

CMake allows you to specify the toolchain as part of the initial CMake step, so at its core,
compiling for a new target OS should be as simple as doing the following step.

```
cmake <path to deployment> -DCMAKE_TOOLCHAIN_FILE=path/to/toolchain_file
```

However, adding tool-chains this way neglects some F´ convenience setup for compiling platform-
specific code. Thus, a user must also register a platform file. A new platform in the
`fprime/cmake/platform/` directory is made by creating a `<platform>.cmake` file there. More
specific steps will be described in `fprime/cmake/platform/README.md`.

Toolchain files can also be added to `fprime/cmake/toolchain/` if they need to be provided with F´
in order to make compiling with new toolchains easier.

