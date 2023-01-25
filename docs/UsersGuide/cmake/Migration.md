# CMake Migration Guide

This document gives key pointers to users who wish to move away from the old make system and start
using the new CMake system. Notably, it provides links for each step in the process of moving from
the key features of the old make system to the features of the new CMake system.

## `make impl` and `make testcomp`

The commands `make impl` and `make testcomp` should be runnable outside of any make system setup.
These commands require an Ai.xml (or a list of them) and run the code generator directly. Thus,
requiring the user to navigate to the directory, generate the make system, and then run these
commands is less productive than running just the code generator directly.

This is now supported with the `ac_util.sh` wrapper. This can be run with the following commands:

```
Autocoders/Python/bin/ac_util.sh impl|testcomp <path/to/Ai.xml>
i.e.
Autocoders/Python/bin/ac_util.sh impl ./ComLogger/ComLoggerAi.xml
```

## Creating Modules: `mod.mk` Moves to CMakeLists.txt

For each place in the old make system where a user defines a `mod.mk`, the user must instead
specify a `CMakeLists.txt`. This file is used to specify source files and some module
dependencies. This is an **important** distinction from the original make system. Each component
auto-detects its dependencies, and from this CMake infers the dependency order.  Dependencies
now found in a walk of all *Ai.xml imports, must be specified manually. Each file should call one
or more of the following API calls:

**Difference from Old Make System:** the CMake system automatically detects dependencies for
everything imported in the Ai.xml files. Thus, only manual and link dependencies must be
maintained. This is done on a per-module basis. These dependencies are then rolled-up into the top-level binary and used for build-ordering and linking.

1. `add_fprime_subdirectory`: make directory available to the build. Done for **all** directories.
2. `register_fprime_module`: add library to the build. Done for **all** components and ports.
3. `register_fprime_executable`: add an executable output to the build. Done for **all** Topologies.

A guide to `CMakeLists.txt` for Modules is found here: [module](module.md)

A guide to the API function calls is found here: [API](API.md)

## Creating Deployments

Deployments in the new F prime system must now specify a `CMakeLists.txt` following a specific
format in order to act as a build entry point. This file includes the F prime make system, acts as
a target for the CMake command, and sets up basic project information. This file may also add other
subdirectories to pull in deployment-specific modules, and is placed at the top-level in the deployment
directory.

**Difference from Old Make System:** the CMake system uses `${PROJECT_NAME}` as the executable
name, not the key from the Topology Ai.xml. This makes naming cleaner and easier to maintain. It is
done in the deployment `CMakeLists.txt` with the CMake `project` command.

In one of the modules added in the Deployment with `add_fprime_subdirectory`, one or more executable
should be added with `register_fprime_executable`.

A guide to `CMakeLists.txt` for Deployments is found here: [deployment](deployment.md)

A guide to the API function calls is found here: [API](API.md)

## Cross-Compiling

Cross-compiling is set up by creating a pair of files. One is a standard CMake toolchain file. These
can be found online or written by the user. These files can be external to the project or kept in
the `cmake/toolchain` folder.

The other file is a platform file. These platform files add F prime specific cross-compilation
configuration in order to keep the toolchain files general-purpose, allowing any CMake toolchain
to be used. Project files setup standard header locations, compilation flags for the platform
being built for, and #defines. In addition, platform files are used to setup unit-test specific
compilations. i.e. unit tests are treated as a separate platform.

This is a **difference from the old make system**.

A guide to toolchains can be found here: [toolchain](toolchain.md)

A guide to platforms can be found here: [platform](platform.md)


## Building Deployments

Building deployments are done out-of-source. This is done to cleanly separate the outputs of the
build from the source of the build. For every variant of the build, the user should create a
separate build directory, which represents a single build. For example, consider the following
desired build variants: Linux, Embedded-Binary, and Unit Tests. Each would be managed with its own
build structure. This is a **difference from the old make system**.

### Setting Up and Building

Each directory is created, and run with the specific build-arguments needed for the specific build.
This includes specifying the toolchain used to build it. Toolchains are used to pick compiler and
build tool paths. They allow the CMake system to cross-compile and automatically link to the
parallel F prime project file, which contain F prime specific cross-compile configuration.

If needed, specific platforms may also be specified, but this is only needed if there are multiple
F prime cross-compile configurations for a given toolchain.

**Difference from Old Make System:** Each build is set up using a call to CMake, and then built with
the standard build call like `make`. Unlink the old system, if something changes in the build
configuration, CMake will automatically be rerun. There should be no need to call `make gen_make`.

A guide to full usage documentation is found here: [README](cmake-intro.md)

Consider three build variants: Linux, Embedded, and Unit Tests.

**Setup Linux Build:**

```
mkdir build_linux
cd build_linux
cmake ../Ref
cd ..
```
**Setup Embedded Build:**

```
mkdir build_arm
cd build_arm
cmake ../Ref -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain/arm-linux-gnueabihf.cmake
cd ..
```
**Setup Unit-Test Build:**

```
mkdir build_ut
cmake ../Ref -DCMAKE_BUILD_TYPE=TESTING
cd ..
```

**Building:**

```
cd build_linux
make
cd ../build_arm
make
cd ../build_ut
make check
```
## Global Build Options:
 - Options: [Options](Options.md) describes the CMake system options
 - Targets: [Targets](targets/Targets.md) describes built in support targets like `dict`