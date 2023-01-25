# F´ CMake Build System

Historical versions of F´ shipped with a bespoke make system ensure that building is done correctly and in the correct
order. However, using and maintaining this build system presents a steep learning curve to new
users of F´. The new CMake system is intended as a replacement for the old make-based
build system that should be easier to learn and use. In addition, the use of cmake puts F´more in line with standard C++ development.

Since this CMake system is designed to follow CMake norms, certain caveats must be
understood before beginning to use CMake. These are described below:

1. CMake should not be used in tandem with the original make system.  If it is needed to switch
between make systems, perform a `git clean -xdf` command or otherwise remove **all** generated files.
2. CMake in-source builds are dangerous. Use CMake out-of-source builds.


Installation guides for CMake can be found here: [https://cmake.org/install/](https://cmake.org/install/).

A Basic CMake tutorial can be found here: [https://cmake.org/cmake/help/latest/guide/tutorial/index.html](https://cmake.org/cmake/help/latest/guide/tutorial/index.html).
Although fprime tries to simplify CMake usage for fprime-specific tasks, an understanding of basic CMake is useful.

## Getting Started with CMake and F´

CMake as a system auto-generates OS-specific make files for building F´. Once these file are generated, standard make tools can be run to perform the compiling, assembling, linking, etc. In other words, CMake is a high-level build system that defers low-level build systems to build. It generates the inputs to these low-level systems in a straightforward way.

fprime sets up CMake in such a way that adding a module (component, port, deployment) is easy and automatically takes
advantage of the autocoding capabilities of fprime. To add new modules to the CMake system, users need to perform the following steps:

1. Define a `CMakeLists.txt` file to define the module's source files and dependencies
2. Ensure that `register_fprime_module` or `register_fprime_executable` is called in that `CMakeLists.txt`
3. Make sure this new directory defining the `CMakeLists.txt` is added to the deployment `CMakeLists.txt` using
   `add_fprime_subdirectory`.

Each of these steps are described in detail below. Further usage documentation on the functions used to perform these
steps can be found in [API](./cmake-api.md). This document will explain the usage of core F´ CMake functions.

Further discussion regarding the transition from the former make system can be found here:
[Migration.md](Migration.md)

## Step 1, Step 2, and Step 3: Define A CMakeList.txt File

The CMakeList.txt file defines the steps needed to build **something** in CMake.  In fprime, we use this file to define the source, autocoder, and module dependencies for modules in fprime. A `register_` function is called to tie into the fprime autocoder environment. This keeps fprime modules simple, although all of CMake's power can be used when needed.

Users need only set the `SOURCE_FILES` variable to a list of autocoder and code sources and then call
`register_fprime_module` to setup a module for fprime (Port/Component). Deployments are done similarly, but since these
`CMakeLists.txt` files are the entry point to a build, more setup is needed. Deployments should also call
`register_fprime_executable` as a binary output is desired. `add_fprime_subdirectory` is also used in deployments to
link to all the other modules they use.

`add_fprime_subdirectory`, `register_fprime_module`, `register_fprime_executable` docs are here: [API](./cmake-api.md).

A template module `CMakeLists.txt` is documented: [module-CMakeLists.txt-template.md](../api/cmake/module-CMakeLists.txt-template.md)
and the template is available here:
[https://github.com/nasa/fprime/tree/master/cmake/module-CMakeLists.txt.template](https://github.com/nasa/fprime/tree/master/cmake/module-CMakeLists.txt.template).
Remember it should be renamed to `CMakeLists.txt` in your module's folder.

A template deployment `CMakeLists.txt` is documented: [deployment-CMakeLists.txt-template.md](../api/cmake/deployment-CMakeLists.txt-template.md)
and available here:
[https://github.com/nasa/fprime/tree/master/cmake/deployment-CMakeLists.txt.template]([https://github.com/nasa/fprime/tree/master/cmake/deployment-CMakeLists.txt.template]).
Remember it should be renamed to `CMakeLists.txt` in your deployments folder.

When building a module, ensure it at least calls `register_fprime_module`. Deployments may call
`register_fprime_executable` in the deployment `CMakeLists.txt` or in any child (usually Top/CMakeLists.txt).

When building a module, remember to add it to the deployment by adding a line `add_fprime_subdirectory(path/module/dir)`
to the deployment `CMakeLists.txt`.

## API Information

The CMake automatically documented API describes the above steps with all details.  The index for this documentation can
be found here: [CMake API](./cmake-api.md).

## Toolchains and Platforms

To integrate with new hardware platforms users need to build or acquire a CMake toolchain file, and add a platform
support file to the F´ CMake system. These steps can be reviewed here:

[CMake Toolchains](./cmake-toolchains.md): F´ CMake toolchain file usage
[F´ Platforms](./cmake-platforms.md): F´ CMake platform files

## Advance CMake Usage

Most users don't need the CMAke advanced usage, as they use `fprime-util`.  However, to use CMake without `fprime-util`
please see: [CMake Advanced Usage](./cmake-advanced.md)

Should a user want to run the unit tests that check CMake, see [CMake UTs](./cmake-uts.md).