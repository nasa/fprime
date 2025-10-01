# F´ CMake Build System

F´ leverages CMake as its underlying build system, adding an [API layer](../../reference/api/cmake/API.md) for ease of use.

Installation guides for CMake can be found here: [https://cmake.org/install/](https://cmake.org/install/).

A Basic CMake tutorial can be found here: [https://cmake.org/cmake/help/latest/guide/tutorial/index.html](https://cmake.org/cmake/help/latest/guide/tutorial/index.html).
Although fprime tries to simplify CMake usage for fprime-specific tasks, an understanding of basic CMake is useful.

## Getting Started with CMake and F´

CMake as a system auto-generates OS-specific build files for building F´. Once these files are generated, standard build tools can be run to perform the compiling, assembling, linking, etc. In other words, CMake is a high-level build system that defers low-level build systems to build. It generates the inputs to these low-level systems in a straightforward way.

F´ sets up CMake in such a way that adding a module (component, port, deployment) is easy and automatically takes
advantage of the autocoding capabilities of fprime. To add new modules to the CMake system, users need to perform the following steps:

1. Define a `CMakeLists.txt` file to define the module's source files and dependencies
2. Ensure that `register_fprime_module` or `register_fprime_deployment` is called in that `CMakeLists.txt`
3. Make sure this new directory defining the `CMakeLists.txt` is added to the deployment `CMakeLists.txt` using
   `add_fprime_subdirectory`.

Each of these steps are described in detail below. Further usage documentation on the functions used to perform these
steps can be found in [API](./cmake-api.md). This document will explain the usage of core F´ CMake functions.

## Step 1, Step 2, and Step 3: Define A CMakeList.txt File

The CMakeList.txt file defines the steps needed to build **something** in CMake.  In fprime, we use this file to define the source, autocoder, and module dependencies for modules in fprime. A `register_` function is called to tie into the fprime autocoder environment. This keeps fprime modules simple, although all of CMake's power can be used when needed.

Users need only set the `AUTOCODER_INPUTS` and `SOURCES` directives to a list of autocoder and code sources as part of the
`register_fprime_module` call to setup a module for fprime (Port/Component). Deployments are done similarly but use the
`register_fprime_deployment` call instead.

`add_fprime_subdirectory`, `register_fprime_module`, `register_fprime_executable` docs are here: [API](./cmake-api.md).


When defining a module, ensure it at least calls `register_fprime_module`, or `register_fprime_deployment`.

When building a module, remember to add it to the deployment by adding a line `add_fprime_subdirectory(path/module/dir)`
to the deployment `CMakeLists.txt`.

## API Information

The CMake automatically documented API describes the above steps with all details.  The index for this documentation can
be found here: [CMake API](./cmake-api.md).

## Build Options

Options describe the runtime options that the CMake system takes. Users wanting to alter the build should look here.
The list of all available options can be found here: [CMake Options](../../reference/api/cmake/options.md)

## Toolchains and Platforms

To integrate with new hardware platforms users need to build or acquire a CMake toolchain file, and add a platform
support file to the F´ CMake system. These steps can be reviewed here:

[CMake Toolchains](./cmake-toolchains.md): F´ CMake toolchain file usage
[F´ Platforms](./cmake-platforms.md): F´ CMake platform files

