<title>CMake Build System</title>

# CMake Build System

## 1 Introduction

The CMake build system is a rewrite of the F´ make system used to streamline the build process, expand features, and bring F´ more in line with the greater C++ community by using
a standardized tool rather than bespoke Makefiles.

This document seeks to describe the requirments, operations concept, and architecture of this make systm in a formal way.

## 1.1 Defitions

**Host:** machine or architecture used to build the code.
**Target:** machine or architecture for which the code is built.
**Build Commands:** commands run through the make system.
**In-Source Build:** generate build artifacts along-side source code.
**Out-Of-Source Build:** generate build artifacts in a separate directory source code.
**Build Configurations:** different build setups, like different targets, debug flags, different deployments. Typically these are isolated from one another.


## 2 Requirements

Requirement | Description | Rationale | Verification Method
---- | ---- | ---- | ----
BUILD-01 | The build system shall use standardized tools to build F´. | Building and maintaining make systems requires community supported tools. | Inspection | 
BUILD-02 | The build system shall run on Linux, and Mac OS. | F´ development at JPL takes place on Linux and Mac OS machines | Inspection |
BUILD-03 | The build system shall provide templates for supporting other host OSes. | Templates make adding hosts easier. | Inspection |
BUILD-04 | The build system shall provide templates for supporting other targets. | Templates make adding new targets easier. | Inspection |
BUILD-05 | The build system shall provide a VxWorks target example. | VxWorks is common at JPL and makes a fantastic example. | Inspection |
BUILD-06 | The build system shall support custom build commands. | Custom build commands allow for extension of the build system. | Inspection |
BUILD-07 | The build system shall support custom build commands. | Custom build commands allow for extension of the build system. | Inspection |
BUILD-08 | The build system shall support individual component build. | Compiling a specific component can speed-up development. | Unit Test |
BUILD-09 | The build system shall support unit test building and running system checks. | Unit testing is critical for proper development. | Unit Test |
BUILD-10 | The build system shall support building deployments. | Deployments must build properly | Unit Test |
BUILD-11 | The build system shall support integration with other build systems. | Some libraries come with other make systems. | Inspection |
BUILD-12 | The build system shall not require specific ordering of all modules to build properly. | Ordering of all F´ is difficult when it must be explicit | Inspection |
BUILD-13 | The build system shall support separate out-of-source building of F´ | Build artifacts are commonly kept separate from source code. | Inspection |
BUILD-14 | The build system shall support executable and tool building | Not all of F´ is a deployment | Inspection |
BUILD-15 | The build system shall support installation and distribution of outputs, headers, and libraries | Shipping of binary outputs is important for projects. | Inspection |
BUILD-16 | The build system shall support user-configurable builds i.e. debug, release, etc. | F´ may need access to different build variants for debugging purposes | Inspection |
BUILD-17 | The build system shall be easy to use including adding new components | F´'s current build system has a steep learning curve | Inspection |
BUILD-18 | The build system shall not be explicitly slow. | Compilation times are non-trivial | Inspection |
BUILD_19 | The build system shall isolation dependencies and builds per deployment-specific configuration | Current F´ has issues with global make config directory | Inspection |
BUILD_20 | The build system shall not be difficult to set up | Porting to the new make system should not require massive efforts | Inspection |
BUILD_21 | The build system shall support treating F´ as a library, sub-repo, and sub-directory even if F is read-only | Future F´ usage should treat core as an input | Inspection |
BUILD_22 | The build system shall not preclude building on Windows hosts. | Windows build are desired to be supported in the future. | Inspection |
BUILD_23 | The build system shall not preclude building sub topologies. | Sub topologies are desired in the future. | Inspection |
BUILD_24 | The build system shall not preclude building F´ core as a set of shared libraries. | Some future missions may benefit from shared F´ core. | Inspection |
BUILD_25 | The build system shall support UT and validation stage hooks. | Validation and additions to Unit Testing support better project development. | Inspection |

## 3 Operations Concepts

The CMake can be run in three different ways. These represent different paradigms of using F´. Primarily, there are two choices of where to place build artifacts (objects, libraries,
archives, makefiles, and sometimes auto-generated code). *Out-of-source* builds place the artifacts into a separate directory from the source files. *In-source* builds place these same
files along-side source. Modern build systems either strongly discourage or forbid *in-source* builds, as content management, build configuration, version control, and development all
become more difficult when using *in-source* builds.

The CMake system will allow *in-source* builds, but only when operating precisely as the old make system is running. Thus there are three paradigms of operation for the CMake system. 
These paradigms are described below. They are:

1. (Recommended) Treating F´ as a sub-repo/library. Builds are performed out-of-source.
2. Adding project code directly to F´. Builds are still performed out-of-source.
3. (Highly Discouraged) Adding code to F´, building is done in-source.  **Note:** this will not work when running with the old make system.

Separate build configurations (different targets, different cmake options set, different deployments, debug builds, etc.) would normally be placed in separate build directories (i.e. 
build_raspi_debug). This keeps all of that configuration, and all of the outputs fully isolated. There is not a change of linking confusion, or build collisions. Nor is there a chance
that the state is forgotten and something "wrong" is built. This is easily supported by options 1 and 2 as they use *out-of-source* builds.  Options 3, performing *in-source* builds
becomes very difficult as any and all build artifacts must be purged. Thus, this option specifically is discouraged.

### 3.1 (Recommended) Treating F´As a Sub-Repo or Library w/ Out-Of-Source Builds

In this design, F´is included as a sub-repository or sub directory to the project. All adaptations are kept out of the F´ directory, allowing for streamlined F´ update, patching, and 
freezing of the F´ core components. This could easily be extended to treat F´ as a library and link against it. Here builds are performed in user supplied directories. The user creates
a named directory to build into, and then supplies cmake the configuration arguments to setup the build properly.

![F´ As Sub Repository]("img/CMake Ops - Recommended.png" "F´ As Sub Repository")

As can be seen here, the adaptations and core top-level directory live alongside one another. The builds go into separate build directories, one for each configuration. This can be
setup with the following commands. Setup needs to be done once and only once for each build type. After that, one can call make over-and-over.

```
cd <project>
mkdir build_config1
cd build_config1
cmake ../<path-to-deployment> -D<configuration settings>
```

Then when code or make changes occur, this configuration can easily be rebuilt by performing the following commands (as many times as needed during development iteration).

```
cd <project>/build_config1
make
```

For building individual components, please see section "3.3, *out-of-source* building of individual components".

### 3.2 Traditional F´Organization w/ Out-Of-Source Builds

In this design, F´ core and adaptations are kept in of the F´ directory. This is the traditional way of using F´. It is less easy to use F´ as a library, and can be somewhat difficult
to update F´, if needed, but is otherwise a stable approach. Here a user uses a named directory to build into, and then supplies cmake the configuration arguments to setup the build
 properly, just like in section 3.1.  The only difference is that all code is in the F´ checkout. This forces a full fork of F´.

![Combined F´ and Adaptations]("img/CMake Ops - Traditional.png" "Combined F´ and Adaptations")

As can be seen here, the adaptations and core code live in the same directory. The builds go into separate build directories, one for each configuration. This can be setup with the
following commands. Setup needs to be done once and only once for each build type. After that, one can call make over-and-over.

```
cd <fprime>
mkdir build_config1
cd build_config1
cmake ../<path-to-deployment> -D<configuration settings>
```

Then when code or make changes occur, this configuration can easily be rebuilt by performing the following commands (as many times as needed during development iteration).

```
cd <project>/build_config1
make
```

For building individual components, please see section "3.3, *out-of-source* building of individual components".

### 3.3 *Out-of-Source* Building of Individual Components

In order to build individual components inside F´ when using out-of-source builds, one must change into the parallel build structure to find the components build directory. F´ core
components live in `<build>/F-Prime/<path-to-component>` and adaptations typically live in `<build>/<adaptation>/<path-to-component>`.  The user can then issue make in this directory
to build the component.  A referend app example can be seen in the commands below.

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

This style is strongly discouraged. It is supported simply to not break the old F´ workflow style. This paradigm comes with some caveats:

1. It cannot be run alongside the old make system. It *will* clobber the old make system.
2. Reconfiguring build settings *will* require removal of all build and CMake artifacts
3. Linking and building errors can arise when switching build configurations.

If at all possible, *do not* use this paradigm without understanding the above caveats and having safety mechanisms to combat the negative 
consequences of this pattern.

![Old Style F´Build]("img/CMake Ops - Old Style.png" "Old Style F´Build")

Here the adaptations are combined and the builds land in the same directory as the source. To use this system, the following commands can be 
run:

```
cd <fprime>
git clean -xdf . #WARNING, this clobbers *all* untracked files
cmake ../<path-to-deployment> -D<configuration settings>
make
```

Each time the build it run it, the above commands **must** be run to prevent errors and unpredictable builds. However, it is heavy handed to
purge all artifacts of the build system.  Configurations are lost or pollute new builds.  Hence the discouraging tone used in this section.

## 4 Design

**This is thundering your way...but has yet to arrive.**
