# CMake Build System

## 1 Introduction

This document describes a new build system based on the popular open source tool cmake
![https://cmake.org/](https://cmake.org/).  The software design document will present a set of
requirements for the build system, some operational concepts, and the candidate build system design.
This build system is intended to replace the legacy F´ build system which is difficult to enhance
and maintain.  The legacy F´ build system had was inherited from the JPL Mars Science Laboratory
mission and created over 10 years ago.

## 1.1 Definitions

These terms have specific meaning in this SDD. This table gives a quick reference for the reader.

| Term | Meaning |
|---|---|
| **Host** | Machine or architecture used to build the code. |
| **Target** | Machine or architecture for which the code is built. |
| **Build Commands** | Commands run through the make system. |
| **In-Source Build** | Generate build artifacts along-side source code. |
| **Out-Of-Source Build** | Generate build artifacts in a separate directory source code. |
| **Build Configurations** | Different build setups, like different targets, debug flags, different deployments. Typically these are isolated from one another. |
| **F´ Module** | Super set of F´ components and F´ ports. |
| **Deployments** | F´ binary/executable containing framework intended to run as F´. |
| **Executable** | Binary built with the build system, not intended to run as F´ deployment |

## 2 Requirements

Requirement | Description | Rationale | Verification Method | Status |
---- | ---- | ---- | ---- | ----
BUILD-01 | The build system shall support native F´ builds and installations on Linux, and Mac OS. | F´ development at JPL takes place on Linux and Mac OS machines | Inspection | Done |
BUILD-02 | The build system shall provide templates for supporting other host OSes. | Templates make adding hosts easier. | Inspection | Done |
BUILD-03 | The build system shall provide templates for supporting other targets. | Templates make adding new targets easier. | Inspection | Done |
BUILD-05 | The build system shall provide a cross-compiled target example. | Cross-compilation is common at JPL and must be provided as an example. | Inspection | Done |
BUILD-06 | The build system shall support custom build commands. | Custom build commands allow for extension of the build system. | Inspection | Done |
BUILD-07 | The build system shall support individual component, port, and topology builds. | Compiling a specific component can speed-up development. | Unit Test | Done |
BUILD-08 | The build system shall support unit test building and running system checks. | Unit testing is critical for proper development. | Unit Test | Done |
BUILD-09 | The build system shall support building deployments. | Deployments must build properly | Unit Test | Done |
BUILD-10 | The build system shall support integration with other build systems. | Some libraries come with other make systems. | Included within CMake | Done |
BUILD-11 | The build system shall not require specific ordering of all modules to build properly. | Ordering of all F´ is difficult when it must be explicit | Inspection | Done |
BUILD-12 | The build system shall support separate out-of-source building of F´ | Build artifacts are commonly kept separate from source code. | Inspection | Done |
BUILD-13 | The build system shall support executable and tool building | Not all of F´ is a deployment | Inspection | Done |
BUILD-14 | The build system shall support installation and distribution of outputs, headers, and libraries | Shipping of binary outputs is important for projects. | Inspection | **Needed** |
BUILD-15 | The build system shall support user-configurable builds i.e. debug, release, etc. | F´ may need access to different build variants for debugging purposes | Inspection | **Work-Around** |
BUILD-16 | The build system shall be easy to use including adding new components | F´'s current build system has a steep learning curve | Inspection | Done |
BUILD-17 | The build system shall not be explicitly slow. | Compilation times are non-trivial | Inspection | Done |
BUILD_18 | Deployments shall configure dependencies independently. | Current F´ has issues with global make config directory | Inspection | Done |
BUILD_19 | The build system shall not be difficult to set up and configure. | Porting existing F´ deployments to the new make system should not require massive efforts | Inspection | Done |
BUILD_20 | The build system shall support treating F´ as a library, sub-repo, and sub-directory even if F is read-only | Future F´ usage should treat core as an input | Inspection | Done |
BUILD_21 | The build system shall support Windows hosts. | Windows build are desired to be supported in the future. | Inspection | **Needed** |
~~BUILD_22~~ | ~~The build system shall support building sub topologies.~~ | ~~Sub topologies are desired in the future.~~ | ~~Inspection~~ | **Removed** Note: Autocoder function support needed |
BUILD_23 | The build system shall support building F´ core as a set of shared libraries. | Some future missions may benefit from shared F´ core. | Inspection | Done |
BUILD_24 | The build system shall support UT and validation stage hooks. | Validation and additions to Unit Testing support better project development. | Inspection | **Needed** |
BUILD_26 | The build system shall support execution of individual, sets, or all gtest based unit tests. | | | Done |
BUILD_27 | The build system shall support explicit and implicit execution of the F´ Autocoder. | | | Done |
BUILD_28 | The build system shall verify that required compilers, linkers, libraries, etc. are installed on host where build is being executed. | | |  DONE |
BUILD_29 | The build system shall implement all user targets of the legacy F´ build system. | | | **Deferred** separate build commands needed. |
~~BUILD_30~~ | ~~The build system shall support execution of individual, sets, or all GSE based integration tests.~~ | | | Note: this is a GDS/GSE process, not a build process. Easily accomplished w/ fprime-gds |
BUILD_31 | The build system shall support execution of individual, sets, or all F´ Autocoder and associated tooling tests. | | | Done |

## 3 Operations Concepts

The CMake can be run in three different ways. These represent different paradigms of using F´.
Primarily, there are two choices of where to place build artifacts (objects, libraries, archives,
makefiles, and sometimes auto-generated code). *Out-of-source* builds place the artifacts into a
separate directory from the source files. *In-source* builds place these same files along-side
source. Modern build systems either strongly discourage or forbid *in-source* builds, as content
management, build configuration, version control, and development all become more difficult when
using *in-source* builds.

The CMake system will allow *in-source* builds, but only when operating precisely as the old make
system is running. Thus there are three paradigms of operation for the CMake system. These
paradigms are described below. They are:

1. (Recommended) Treating F´ as a library. Builds are performed out-of-source.
2. Adding project code directly to F´. Builds are still performed out-of-source.
3. (Highly Discouraged) Adding code to F´, building is done in-source.  **Note:** this will not
work when running with the old make system in-place.

Separate build configurations (different targets, different cmake options set, different
deployments, debug builds, etc.) would normally be placed in separate build directories (i.e.
build_raspi_debug). This keeps all of that configuration, and all of the outputs fully isolated.
There is not a change of linking confusion, or build collisions. Nor is there a chance that the
state is forgotten and something "wrong" is built. This is easily supported by options 1 and 2 as
they use *out-of-source* builds.  Options 3, performing *in-source* builds becomes very difficult
as any and all build artifacts must be purged. Thus, this option specifically is discouraged.

### 3.1 (Recommended) Treating F´As a Library w/ Out-Of-Source Builds

In this design, F´is included as a sub directory to the project. All adaptations are kept out of
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

For building individual components, please see section "3.3, *out-of-source* building of individual
components".

### 3.2 Traditional F´Organization w/ Out-Of-Source Builds

In this design, F´ core and adaptations are kept in the F´ directory. This is the traditional
way of using F´. It is less easy to use F´ as a library, and can be somewhat difficult to update
F´, if needed, but is otherwise a stable approach. Here a user uses a named directory to build
into, and then supplies cmake the configuration arguments to setup the build properly, just like
in section 3.1.  The only difference is that all code is in the F´ checkout. This forces a full
fork of F´.

![Combined F´ and Adaptations](img/CMake%20Ops%20-%20Traditional.png "Combined F´ and Adaptations")

As can be seen here, the adaptations and core code live in the same directory. The builds go into
separate build directories, one for each configuration. This can be setup with the following
commands. Setup needs to be done once and only once for each build type. After that, one can call
make over-and-over.

```
cd <fprime>
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

For building individual components, please see section "3.3, *out-of-source* building of individual
components".

### 3.3 *Out-of-Source* Building of Individual Components

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

### 3.4 (Highly Discouraged) Old Style *In-Source* Builds

This style is strongly discouraged. It is supported simply to not break the old F´ workflow style.
This paradigm comes with some caveats:

1. It cannot be run alongside the old make system. It *will* clobber the old make system.
2. Reconfiguring build settings *will* require removal of all build and CMake artifacts
3. Linking and building errors can arise when switching build configurations.

If at all possible, *do not* use this paradigm without understanding the above caveats and having
safety mechanisms to combat the negative consequences of this pattern.

![Old Style F´Build](img/CMake%20Ops%20-%20Old%20Style.png "Old Style F´Build")

Here the adaptations are combined and the builds land in the same directory as the source. To use
this system, the following commands can be run:

```
cd <fprime>
git clean -xdf . #WARNING, this clobbers *all* untracked files
cmake ../<path-to-deployment> -D<configuration settings>
make
```

Each time the build it run it, the above commands **must** be run to prevent errors and
unpredictable builds. However, it is heavy handed to purge all artifacts of the build system.
Configurations are lost or pollute new builds.  Hence the discouraging tone used in this section.

### 3.5 Adding in New Components and Topologies

New components can be added by creating a `CMakeLists.txt` file in the directory of that component.
This file is required to set two CMake variables:

1. **SOURCE_FILES**: a list of files that act as source or autocoding source files.
2. **MOD_DEPS**: (optional) a list of module and link dependencies.

These are set using the CMake set() function, as shown below. Finally, this file must call
`register_fprime_module`, a F´ CMake support function used to setup the auto coding step and ensure
this component is built as an F´ module.  This call passes in the above variables.

**Module CMakeList.txt**

```
# Default module cmake file
# SOURCE_FILES: Handcoded C++ source files

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/PingReceiverComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/PingReceiverComponentImpl.cpp"
)

# Note: no MOD_DEPS needed.

register_fprime_module()
```

This file must be added to some parent CMakeLists.txt file using the add_subdirectory file. If it
is a new F´ component this is typically added to `CMakeLists.txt` in the top-level directory this
component goes in. i.e. `fprime/Svc/CMakeLists.txt`. These sub-CMakeLists.txt are there as a
convenience and are included themselves in FPrime.cmake. If there is no convenient sub list file,
the component may be added directly to `fprime/cmake/FPrime.cmake`. Non-core components must be
added to the deployment `CMakeLists.txt` file, or to some child included from there.

**Example Add Subdirectory**

```
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
```

Topology `CMakeLists.txt` follow the same format as the Module files with two deviations. First,
`MOD_DEPS` is usually defined as some dependencies cannot be auto-detected must be chosen and
`register_fprime_executable` is called as an executable will be generated.

**Topology CMakeList.txt**

```
# Default deployment cmake file
# SOURCE_FILES: Handcoded C++ source files
# MOD_DEPS: Modules needed by this deployment

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/RefTopologyAppAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/Topology.cpp"
)

set(MOD_DEPS
  Svc/PosixTime
  Svc/PassiveConsoleTextLogger
)

register_fprime_executable()
```

These files must also be added to entry-point `CMakeList.txt` or a child therein.

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
cmake_minimum_required(VERSION 3.5)
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
automate the auto-code function, module dependencies, and various other utilities are included to.
Thus deployments and executables can follow the same pattern as core F´ components when adding
custom components of their own.

### 4.3 F´Core and Adaptation CMakeLists.txt Files

These files are used to specify the build layout of the F´ components, ports, and topologies. They
are composed in a hierarchy as shown below. These files call the F´ module and deployment functions
to generate the expected build files. The file format is described in section *3.5 Adding in New
Components and Topologies*. These files link source files, and auto-coder inputs to the generate
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
the CMake system. This library is built from `SOURCE_FILES`, which are split into inputs to the
autocoder (*.xml and *.txt) and normal source files. Specific dependencies can be supplied using
the `MOD_DEPS` variable, but is only required when the dependencies are not detected via a recursive
search of all *.xml includes in the autocoder files. The autocoder generation steps are registered
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
separate name must be supplied. The executable may supply `SOURCE_FILES` and `MOD_DEPS`, use
autocoding, and otherwise acts similar to the above module with the exception that an executable
output is the result of the build.

**Note:** deployments specify one or more executables, and these executables become the root of the
dependency tree. Thus, only the needed executables, libraries, and outputs are generated and the
complete F´ system is not explicitly built. This  makes the build more efficient.

### 5.3 Unit Test Functions: register_fprime_ut

Registering unit tests uses the same process as above with the exception that the variables
`UT_SOURCE_FILES` and `UT_MOD_DEPS`. This allows the same file to define both a module or
executable and unit test without overriding previously used variables.

Unit tests must be built with a cmake build type of "TESTING". This allows for the building of the
unit-tests and setting up the `make check` target. This prevents the
unit-tests from bogging down a normal build, and allows for specialized compilation flags for UTs.
They are registered with a `make check` target to allow them to be run at once. Individual UTs can
be run from the `bin` directory if needed. It is advised that the user build from top-level F´ as
this will pull in every unit-test. Building from a deployment is useful when running that
deployment's unit-tests.

**Example UT Build**

```
mkdir build_ut
cd build_ut
cmake .. -DBUILD_TESTING=ON
make -j32
```

### 5.3 Adding New Platforms

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

