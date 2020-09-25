# F´ CMake Build System

Historical versions of F´ shipped with a bespoke make system ensure that building is done correctly and in the correct
order. However, using and maintaining this build system presents a steep learning curve to new
users of F´. The new CMake system is intended as a replacement to the old make based
build system that should be easier to learn and use. In addition, the use of cmake puts F´more in
line with standard C++ development.

Since this CMake system is designed to follow CMake norms, certain caveats must be
understood before beginning to use CMake. These are described below:

1. CMake should not be used in tandem with the original make system.  If it is needed to switch
between make systems, perform a `git clean -xdf` command or otherwise remove **all** generated files.
2. CMake in-source builds are dangerous. Use CMake out-of-source builds.


Installation guides for CMake can be found here: [https://cmake.org/install/](https://cmake.org/install/).

A Basic CMake tutorial can be found here: [https://cmake.org/cmake/help/latest/guide/tutorial/index.html](https://cmake.org/cmake/help/latest/guide/tutorial/index.html).
Although fprime tries to simplify CMake usage for fprime specific tasks, an understanding of basic CMake is useful.

## Getting Started Using with and F`

CMake as a system auto-generates OS-specific make files for building F´. Once these file are
generated, standard make tools can be run to perform the compiling, assembling, linking etc. In other words, CMake is a
high-level build system that defers to low-level build systems to build.  It generates the inputs to these low-level
systems in a straight forward way.

fprime sets up CMake in such a way that adding a module (component, port, deployment) is easy and automatically takes
advantage of the autocoding capabilities of fprime. To add new modules to the CMake system, users need to perform the
following steps:

1. Define a `CMakeLists.txt` file to define the module's source files and dependcies
2. Ensure that `register_fprime_module` or `register_fprime_executable` is called in that `CMakeLists.txt`
3. Make sure this new directory defining the `CMakeLists.txt` is added to the deployment `CMakeLists.txt` using
   `add_fprime_subdirectory`.

Each of these steps are described in detail below. Further usage documentation on the functions used to perform these
steps can be found in: [API.md](API.md). This document will explains the usage of core F´ CMake functions.

Further discussion regarding transition from the former make system can be found here:
[Migration.md](Migration.md)

## Step 1, Step 2, and Step 3: Define A CMakeList.txt File

THe CMakeList.txt file defines the steps needed to build **something** in CMake.  In fprime, we use this file to define
the source, autocoder, and module dependencies for modules in fprime. A `register_` function is called to tie into the
fprime autocoder environment. This keeps fprime modules simple, although all of CMake's power can be used when needed.

Users need only set the `SOURCE_FILES` variable to a list of autocoder and code sources and then call
`register_fprime_module` to setup a module for fprime (Port/Component). Deployments are done similarly, but since these
`CMakeLists.txt` files are the entry point to a build, more setup is needed. Deployments should also call
`register_fprime_executable` as a binary output is desired. `add_fprime_subdirectory` is also used in deployments to
link to all the other modules they use.

`add_fprime_subdirectory`, `register_fprime_module`, `register_fprime_executable` docs are here: [API.md](API.md).

A template module `CMakeLists.txt` is documented here: [module-template.md](module-template.md) and available here:
[https://github.com/nasa/fprime/cmake/module-CMakeLists.txt.template]. Remember it should be renamed to `CMakeLists.txt`
in your module's folder.

A template deployment `CMakeLists.txt` is documented here: [deployment-template.md](module-template.md) and available
here: [https://github.com/nasa/fprime/cmake/deployment-CMakeLists.txt.template]. Remember it should be renamed to
`CMakeLists.txt` in your deployments folder.

When building a module, ensure it at least calls `register_fprime_module`. Deployments may call
`register_fprime_executable` in the deployment `CMakeLists.txt` or in any child (usually Top/CMakeLists.txt).

When building a module, remember to add it to the deployment by adding a line `add_fprime_subdirecory(path/module/dir)`
to the deployment `CMakeLists.txt`.

## Advanced fprime CMake Usage

fprime allows for two different CMAKE_BUILD_TYPE settings (specified using -DCMAKE_BUILD_TYPE when generating a build
cache). These two types are: Release used to build a flight-like executable, and Testing enabling debugging and unit
test features.  These are split such that testing can be done on a more permissive executable.

**Note:** `fprime-util` was designed to ease use of the F´ cmake system by wrapping all of the developer level processes
           freeing users from running these steps directly. This automatically handles the two build types using the
           developer's action to choose the correct one.

Running CMake directly (by hand) amounts to generating a build cache and running make (assuming GNU make output):

1. Make and change to a directory to build in: `mkdir build_dir; cd build_dir`
2. Call CMake to generate make-files: `cmake <path to deployment CMakeLists.txt> -DCMAKE_BUILD_TYPE=<some type>`
3. Engage OS-specific make system: `make`
4. Run unit tests: `make check`. Note: `check` is only available in the `Testing` CMAKE_BUILD_TYPE

Further information on each step is provided below.

### Make Build Directory and Generate CMake Files (Run Once Per Configuration)

When building F´ using the CMake system, each build type should be separated into its own directory.
Any time a different platform, toolchain, or option set are used, a new directory should be created.
However, each of these directories are independent, and can be built independently without cleaning
or removing the others.

The following commands will create a new build directory and generate CMake files. Separate builds
should be isolated in their own build-directories.  These directories can be achieved as
build-artifacts, but are typically not added to source management (Git).

Below, a user-provided deployment directory could be substituted for `Ref` below in-order to build a
different deployment. More on deployments below.

**Build Setup Commands**

```
# Make a build directory and change directory into it
mkdir fprime/build-dir
cd fprime/build-dir
# Run CMake to generate CMake Files (Specifically for the Ref App)
cmake ../Ref/
```

### Building Deployments (Iterated On Change)

Once generated by CMake, the cmake files typically do not need to be re-generated. If new
configuration is needed, a separate (new) build directory should be used.  If changes occur to the
CMake system, running the following steps will rerun the CMake file generation. Thus, the above
step can be run one time.  Rebuilding and iteration can be done with the following simple steps:

**Build Commands(Linux, Mac OS X)**

``` 
# Build the application (Will regenerate CMake if necessary)
make
# Build and run the UTs (if desired)
make check # Only if CMAKE_BUILD_TYPE=Testing was used when generating the build cache
```

## CMake Options

There are several options that can be specified as part of the CMake system. These options are
documented in the following file: [Options.md](Options.md).

## Adding in New CMake Components and Deployments

The core of a cmake build is the `CMakeLists.txt` file. This file specifices the files needed to
build the current directory of the system. In F prime each Component, Port, and Topology get a
`CMakeList.txt` along with the top-level deployment directory.

Two templates for these CMakeLists.txt files are provided as part of the CMake system. One for
Modules that result in the creation of a library, executable, or both. The other for Deployments,
which setup the CMake for a deploment and include a number of Modules.

Components, Ports, and Topologies (`Top` folders) all use the Module template. These modules
provide libraries and executables to the system.

Top-level deployment directories (`Ref` folder) use the deployment templates. It is there to setup
the entry point to the build system.

For more on Modules, see: [module.md](module.md)
For more on Deployments, see: [deployment.md](deployment.md)


## Cross-Compiling With CMake

In order to cross compile F´ with the cmake system to some new target platform, two files are
required. These two files are a cmake toolchain file, and an F´ platform file. Once these files
have been created, a cross-compile can be setup and run with the following commands:

```
mkdir build-cross
cd build-cross
cmake -DCMAKE_TOOLCHAIN_FILE=<path/to/toolchain/file> <path/to/deployment>
make
```

### CMake Toolchain File

Toolchain files are used to setup the tools and packages used to cross-compile code for a separate
target platform. The cmake toolchain files are placed in [cmake/toolchain](../toolchain) and are
standard CMake toolchain files.
[https://cmake.org/cmake/help/v3.12/manual/cmake-toolchains.7.html](https://cmake.org/cmake/help/v3.12/manual/cmake-toolchains.7.html)
Alternatively, the user may specify a path to an external CMake toolchain file. This file specifies
the path to the tools used to perform the build (i.e. the compilers, libraries, and packages). A
sample template for setting up new toolchain files can be found at
[cmake/toolchain/toolchain.cmake.template](../toolchain/toolchain.cmake.template). Copy this file to
the new toolchain name, fill it out, and it can be then used with the above cross-compile
instructions.

**Note:** a parallel platform file must be created or an exisitng platform file specified with the
`-DPLATFORM` option before a toolchain file can be used.

More information can be found at: [template.md](docs/template.md).
More information can be found at: [Options.md](Options.md).

### CMake Platfrom File

Platform files are used to specify CMake options, compile flags, definitions, and other setting
used by F´ when compiling for a separate target. These files are F´ specific and are named after
the ${CMAKE_SYSTEM_NAME} defined in the toolchain file used to kick-off a cross compile. The
platform files are found in [cmake/platform](../platform). In order to create one of these files, copy
the platform template and fill it out. The template for setting up new platform files can be found
at [cmake/platform/platform.cmake.template](../platform/platform.cmake.template) Then follow the above
instructions to cross-compile.

**Note:** a parallel toolchain file must exist in order to trigger this platform file unless it is
manually specified with the `-DPLATFORM` option.

More information can be found at: [platform.md](platform.md).
More information can be found at: [Options.md](Options.md).

## CMake Customization

Sometimes users need to crack open the CMake structure in order to do things like adding a external
library/build system, adding custom make targets, building utilities, etc. These issues are described
here: [Customization](Customization.md)


Further documentation can be found in the SDD: [SDD.md](sdd.md)