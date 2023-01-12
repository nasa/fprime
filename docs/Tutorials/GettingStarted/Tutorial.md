# Getting Started with F´

Prerequisites: [INSTALL.md](../../INSTALL.md)

This tutorial will walk the user through the basic usage of the F´ development suite. This assumes that the user has run
through the above installation guide, and has a working F´ installation.

The goal of this Tutorial is to build familiarity with the F´environment and tools. It does not walk the user through
creation of new F´ items, but rather use the existing F´ `Ref` example as a place to operate the tools, learn the
environment, and learn F´.

The full development process is covered in the [Math Component Tutorial](../MathComponent/Tutorial.md), which will build
on the knowledge learned here.

## Tutorial Goals

The goal of this tutorial is to demonstrate the F´ tool suite as it applies to the F´ development process.  Users will
first gain some understanding of F´ terminology followed by the F´ development process. Then they will use the tools to
walk the F´ development process steps against existing reference goals.

## F´ Terminology

This section will cover basic terminology used in this and other tutorials with respect to F´.  It may be used as a
reference for keywords that have specific meaning in an F´ project.

**Port:** a port represents a connection between *Components*. These act as communication channels in F´. This allows
*Components* to interact with each other. A set of *Ports* acts as a defined interface to a *Component*.

**Component:** the location of program behavior. Components execute the software logic of the system and typically
define an interface of *Ports* that act allow them to interact with other *Components*. Components additionally
may define *Commands*, *Events*, *Channels*, and *Parameters*, which define standard data patterns in F´.

**Topology**: a topology represents the full F´ system. It is a set of *Components* and the connections between those
*Components*. The *Topology* specifies the full system and all the communication channels.

**Event:** an event, also known as an EVR, is a type of downlinked data that represents a single event in the system.
An event is composed of a name, format string, and set of arguments that described what happened. Events represent a
history of the system. Events are defined per-*component*.

**Channel:** a channel, also known as a telemetry item, is a single value read and downlinked. Channels consist of
a type, a format specifier, and a value. Channels represent the current system state. Channels are defined per-*component*.

**Command:** a command is uplinked data items that instruct the system to perform an action. *Commands* consist of an
opcode, a mnemonic, and a list of arguments to the command. Commands are defined per-*component*.

**Deployment:** a deployment is a single instance of F´. Although a single F´ project can use/define multiple
deployments, a deployment represents one build/executable.

**Build Directory/Build Cache:** this directory, created by the `fprime-util` described below, is required to build
CMake projects. It stores all by-products, generated code, and build products from an F´ build.

**Ground System:** the computer system running to collect the data from the running F´ deployment. There is a basic
ground system supplied with F´ for development purposes.

**Toolchain:** a set of build tools to build for a specific architecture. i.e. the "raspberrypi" toolchain will build F´
deployments for the arm/Linux architecture of the raspberry pi.

## F´ Development Process

F´ development typically starts with creating new Ports and Components as these can be built and tested as units and
then the full F´ system deployment topology can be assembled for integrated systems testing and release/operations. This
can be done iteratively by creating a few Components then integrating them into the system topology and then repeating
with the next most critical components.

F´ development is a design-first approach. To start, a developer should design any unique ports that the Component will
need. If desired, the port can be built and compiled to ensure that the design is correct. Since the port will be
entirely auto-generated, developers usually refrain from unit testing the port and defer unit testing to the Component.

Next, developers design components. Components are designed by specifying the ports that represent the Component's
interface and the Commands, Events and Channels that the Component handles. This tutorial focuses on the tool suite, so
the design is already in-place.  Users can see the design by inspecting Components in the `Ref` directory. Creation of
new components from design through test is demonstrated in the [Math Component Tutorial](../MathComponent/Tutorial.md).

The next step for developing a Component is to implement the code. To do this, one can generate template stubs to fill
with the developer's stubs. To do this, a user runs the F´ tool suite to generate these stubs. Then implements, builds,
and tests from there. Unit test template stubs are generated in the same way and should test-driven development be
preferred, these test stubs could be generated and implemented before the Component's code.

Finally, the Component is integrated into the deployment and the entire deployment may be built and installed.

This tutorial will walk developers through generating the templates, building example Components, and
building/installing the full deployment. It will use the `Ref` application as an example for this.

## Working the F´ Development Process with `fprime-util`

In order to ease in development, the F´ team has created a small wrapper for the CMake build system. This will recover the
advantages of previous F´ development patterns, while allowing for the power and configurability of CMake as an underlying
build tool.

### Build Cache Generation

In order to run CMake, a build cache needs to be generated.  This is typically done once for each platform the user wishes
to compile against. It is usually done right after F´ is checked-out and repeated for each platform as they are added. It
is a setup step and isn't formally part of the F´ development process.

To run this tool, the developer will use the `generate` subcommand.  It takes one optional argument: the toolchain file
used in CMake to compile for a specific platform. If not supplied, the `native` toolchain will be used and F´ will be
set up to run on the current platform (typically Mac OS, or Linux depending on the developer's chosen OS).

**Generate the Ref Application for Native Compilation**

To generate a native build, follow the commands shown below. It is assumed that the user has checked out the fprime repository
and is performing commands relative to the location of that checkout.

```
cd fprime/Ref
fprime-util generate
```

Here the utility will create two standard builds: one for the standard build, and one for the unit test build. This will enable
the user to take advantage of all parts of F´ without generating their own CMake build caches. **Note:** should a developer
be developing or improving F´ provided infrastructure components, then the `generate` command should be run in the F´
root directory. However, most developers do not need this functionality.

**Generate Cross-Compile of the Ref Application for Raspberry PI Platform**

Most developers wish to run F´ on embedded hardware. This is done by generating a cross-compile using a different CMake
toolchain by providing the toolchain argument. The above invocations assume the default "native" toolchain.

Here, the "raspberrypi" toolchain supplied by F´ is used. This will generate a cross-compile for the raspberry pi
architecture.  The user should ensure to follow the Raspberry PI setup steps found at: [RPI](https://github.com/nasa/fprime/blob/master/RPI/README.md)
```
cd fprime/Ref
fprime-util generate raspberrypi
```

## Creating Implementation Stubs

Once the build system cache is generated, developers will need to generate the stubbed implementations of a Component's C++
files. This can be done using the `impl` command of the F´ utility. These commands assume a default build has been generated.
Typically passing in a toolchain is unnecessary as these templates are platform agnostic.

**Generating Implementation Stubs of SignalGen**
```
cd fprime/Ref/SignalGen
fprime-util impl
```

This creates two files for the component. These are `<Component>Impl.cpp-template` and `<Component>Impl.hpp-template`.
The user can then rename these files to remove `-template`. The file is then ready for C++ development. By generating
`-template` files, we won't accidentally overwrite any existing implementation should the developer need to repeat this
step.

## Creating Unit Test Implementations

Once the build system cache is generated, developers will need to generate the stubbed implementations for creating
unit tests for a Component's C++. This can be done using the `impl --ut` command of the F´ utility. These commands
assume a default build has been generated. They can be run passing in a toolchain, but this is typically not done because
they are toolchain agnostic.

**Generating Unit Test Stubs of SignalGen**
```
cd fprime/Ref/SignalGen
fprime-util impl --ut
```

This creates the following files, which are typically moved to a subfolder called `test/ut`. The files created are
placed in the current directory and named:
```
Tester.cpp
Tester.hpp
GTestBase.cpp
GTestBase.hpp
TesterBase.cpp
TesterBase.hpp
TestMain.cpp
```

### Building Components and Deployments

Once a developer has implemented a component, it is time to build that component and test that component. In order to build
a component, the `build` subcommand needs to be run. This will build the "current directory" that the developer is in.
That means that should the user change into a deployment directory then the build command will build the full deployment.
Should the user desire to build a component, navigate into the Component's directory and run the build command.

**Build SignalGen Component**
```
cd fprime/Ref/SignalGen
fprime-util build
```

The user can also build a deployment by navigating to the deployment directory and running the `build` command. This is
shown below:

**Build Ref Deployment**
```
cd fprime/Ref
fprime-util build
```
**Note:** the user almost always wants to run the "install" command on deployments. This builds the binary like "build"
but also copies the binary to the deployments "bin" directory, i.e. `fprime/Ref/bin`.  Install is described below.

This process also built the Dictionaries for the project and places the dictionary in the "Top" folder of the deployment.
This happens any time the "install" or "build" command are run on a deployment.

The user can also build a component or deployment for a cross-compile by specifying the toolchain.  A previous generate
for that toolchain should have been run. Again for deployments, the user typically should run "install", see below. These
steps require the setup described here: [RPI](https://github.com/nasa/fprime/blob/master/RPI/README.md)

```
cd fprime/Ref/SignalGen
fprime-util build raspberrypi
cd fprime/Ref
fprime-util build raspberrypi
```

**Running the Binary Assuming Linux**
```
cd fprime/Ref
fprime-util build
./build-artifacts/Linux/bin/Ref # Run the deployment
CTRL-C # Exit the application
```
Running the application as part of the development ground data system is shown below.

The user can also install a cross-compile.
```
cd fprime/Ref
fprime-util build raspberrypi
```

## Building and Running Unit Tests

Unit tests can be built using the `build --ut` command of the `fprime-util`. This will allow us to build the unit tests
in preparation to run them.  The user can also just run `check` to build and run the unit tests.

Before building unit tests, the unit test build cache must be generated:

```
cd fprime/Ref
fprime-util generate --ut
```

**Building Unit Test of SignalGen**
```
cd fprime/Ref/SignalGen
fprime-util build --ut
```

Once built, the unit test can be run using the following command.  **Note: this will produce a message warning of no
tests run because SigGen has no unit tests defined.**
```
cd fprime/Ref/SignalGen
fprime-util check
```
The user can also build the unit test, but must copy it to the hardware and run it here. This can be done with a cross-
compile by running the following commands.

**Cross-Compile Unit Test of SignalGen**
```
cd fprime/Ref/SignalGen
fprime-util build raspberrypi --ut
```

## Conclusion

The user should now be familiar with F´ terminology and with the `fprime-util` tool used to build and develop F´
applications. The next step is to follow the full `MathComponent` tutorial to create new *Ports*, *Components*, and
*Topologies*. This will walk the user through the entire development process, using the tool commands we learned here.

**Next:** [Math Component Tutorial](../MathComponent/Tutorial.md)
**Also See:** [GPS Component Tutorial](../GpsTutorial/Tutorial.md) for a cross-compiling tutorial.