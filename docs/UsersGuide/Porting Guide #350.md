**Outline**

Porting Guide

> i. Porting F′ to a New Hardware Platform
> 
> TBD
> 
> ii. Building Components and Deployments
> 
> a. Build SignalGen Component
> 
> b. Build Ref Deployment

**Porting Guide**

**i. Porting F′ to a New Hardware Platform**

Porting F′ to a new hardware platform is often required for using F´ on
new missions and projects. Fortunately, F´ is designed to be mostly
independent of hardware. There are four parts of F´ that need to be
developed to run on a new platform:

1.  CMake Toolchain and F´ platform files. These files specify the
    location of compilers and other build tools for compiling F´ for a
    given platform.

2.  Fw::Types and configuration. A Fw::Types header may be needed and
    configuration set if the hardware needs different settings from the
    default.

3.  Hardware drivers. Any drivers that are outside the provided Linux
    Driver package would need to be developed for the hardware specific
    interfaces to be supported.

4.  OSAL support. Should a non-Linux operating system be needed to run
    on the platform, OSAL support files must be developed.

The F′ build system provides targets for the standard build … TBD

The F′ build system also provides targets for building and running
component unit tests. The system compiles unit tests with code coverage
analysis enabled through gcov.6. Running the unit tests automatically
generates the code coverage results and stores them to files in a form
that can be examined. These are independent configuration files that
generate native makefiles and workspaces that can be used in a preferred
compiler environment.

Build automation involves scripting or automating the process of
compiling computer source code into binary code. The build directory is
created by the fprime-util. It is required to build CMake projects. It
stores all by-products, generated code, and build products from an F′
build.

CMake is an open-source, cross-platform software tool designed to build,
test and package software. It is currently used as the F′ build system
for building cross-platform binaries. CMake builds code in a separate
directory from the source, then new ports, components, and deployments
can be added.

To generate a working build cache use the F′ utility with the “generate”
command. This command takes a single argument of the F′ toolchain the
user selects to build. The default is read from the deployment
CMakeLists.txt and is typically “native.” The utility creates two
builds: one for the standard build, and one for the unit test build.
This enables the user to take advantage of all F′ parts without
generating their own CMake build caches. If the user chooses to edit or
test individual F′ built-in components, the user should also generate
builds at the F′ library's root.

**ii. Building Components and Deployments**

In order to build a component or deployment with F′, the user runs the
“build” command as part of the fprime-utility to build the current
directory. This means that should the user change into a deployment
directory then the build command builds the full deployment. However, if
the user desires to build a component, navigate to the directory and run
the build command.

**a. To Build SignalGen Component**

cd fprime/Ref/SignalGen

fprime-util build

The user can also build a deployment by navigating to the deployment
directory and running the build command.

**b. To Build Ref Deployment**

cd fprime/Ref

fprime-util build

The user must run the install command on deployments. This builds the
binary “build,” but also copies the binary to the deployments bin
directory. i.e., fprime/Ref/bin.

This process also builds the dictionaries for the project and places the
dictionary in the top folder of the deployment. This happens when the
install or build command are run on a deployment.

The user can also build a component or deployment for a cross-compile by
specifying the toolchain. However, first run a generate for that
toolchain. For deployments, the user must run install.

cd fprime/Ref/SignalGen

fprime-util build raspberrypi

cd fprime/Ref

fprime-util build raspberrypi
