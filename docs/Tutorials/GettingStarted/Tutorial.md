# Getting Started with F´

Prerequisites: [https://github.com/nasa/fprime/blob/master/INSTALL.md](https://github.com/nasa/fprime/blob/master/INSTALL.md)

This tutorial will walk the user through the basic usage of the F´ development suite. This assumes that the user has run
through the above installation guide, and has a working F´ installation.

The goal of this Tutorial is to build familiarity with the F´environment and tools. It does not walk the user through
creation of new F´ items, but rather users the existing F´ `Ref` example as a place to operate the tools, learn the
environment, and learn F´.

The full development process is covered in the [Math Component Tutorial](../MathComponent/Tutorial.md), which will build
on the knowledge learned here.

## F´ Terminology

This section will cover basic terminology used in this and other tutorial with respect to F´.  It may be used as a
reference for keywords that have specific meaning in an F´ project.

**Port:** a port represents a connection between *Components*. These act as communication channels in F´. This allows
*Components* to interact with each other. A set of *Ports* acts as a defined interface to a *Component*. 

**Component:** a single unit in F´. This represents one conceptual portion of an F´ deployment.  Typically encapsulates
core logic inside. *Components* define a set of *Ports* that act as the interface to the *Component*, and additionally
may define *Commands*, *Events*, *Channels*, and *Parameters*. These items represent a standard data pattern for F´.

**Topology**: a topology represents the full F´ system. It is a set of *Components* and the connections between those
*Components*. The *Topology* specifies the full system and all the communication channels.

**Event:** an event, also known as an EVR, is a type of downlinked data that represents a single event in the system.
An event is composed of a name, format string, and set of arguments that described what happened. Events represent a
history of the system. Events are defined per-*component*.

**Channel:** a channel, also known as a telemetry item, is a single value read and downlinked. Channels are consist of
a type, a format specifier, and a value. Channels represent current system state. Channels are defined per-*component*.

**Commands:** commands are uplinked data items that instruct the system to perform an action. *Commands* consist of an
opcode, a mnemonic, and a list of arguments to the command. Commands are defined per-*component*.

**Deployment:** a deployment is a single instance of F´. Although a single F´ project can use/define multiple
deployments, a deployment represents one build/executable.

**Build Directory/Build Cache:** this directory, created by the `fprime-util` described below, is required to build
CMake projects. It stores all by-products, generated code, and build products from an F´ build.

## F´ Utility

In order to ease in development, the F´ team has created a small wrapper for the CMake build system. This will recover the
advantages of the previous development pattern, while allowing the power and configurability of CMake as an underlying
build architecture.

### Build Cache Generation

The first step to building and developing an F´ application is to generate a working build cache. This can easily be done
using the F´ utility with the "generate" command. This command takes a single argument of the F´ of the F´ deployment one
wishes to build.  If not supplied, the current directory will be considered the deployment to use.

**Generate the Ref Application**
```
cd fprime/Ref
fprime-util generate
```

Here the utility will create two standard builds: one for the standard build, and one for the unit test build. This will enable
the user to take advantage of all parts of F´ without generating their own CMake build caches.

If the user wishes to edit or test individual F´built-in components, the user should also generate builds at the F´ library's
root. This is shown below.

**Generate the F´ Framework**
```
cd fprime
fprime-generate
```

### Building Components and Deployments

In order to build a component or deployment with F´, a user may run the "build" command as part of the fprime-utility. This
will build the "current directory". That means that should the user change into a deployment directory then the build command
will build the full deployment. Should the user desire to build a component, navigate into the directory and run the build
command.

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

Now users should be able to change components' C++, build the component to ensure that the changes compile, and then continue
to build the full deployment to yield a runnable F´ executable. The F´ executable is currently stored at
`fprime/Ref/build*/bin/*/Ref`.

This process also build the Dictionaries for the project and places the dictionary in the "Top" folder of the deployment.

## Installing the F´ Executable and Dictionaries

Once the deployment is built, it would nice to be able to install the binary and dictionaries. This will enable the users to
quickly find and run the deployment. This installation can be run using the following command. Everything will be install to
the directory defining the deployment.

**Installing the Ref Deployment**
```
cd fprime/Ref
fprime-util install
./bin/*/Ref # Run the deployment
CTRL-C # Exit the application
```
Running the application as part of the development ground data system is shown below.

## Creating Implementation Stubs

Once the build system is up and running, developers will desire to generate the stubbed implementations of the Component's C++
files. This can be done using the "impl" command of the F´ utility.

**Generating Implementation Stubs of SignalGen**
```
cd fprime/Ref/SignalGen
fprime-util impl
```

This creates two files for the component. These are `<Component>Impl.cpp-template` and `<Component>Impl.hpp-template`.
The user can then rename these files to remove `-template`. The file is then ready for C++ development. By generating 
`-template` files, we won't accidentally overwrite any existing implementation.

## Creating Unit Test Implementations

Once the build system is up and running, developers will desire to generate the stubbed implementations and other files
needed for building UTs for a Component's C++. This can be done using the "impl" command of the F´ utility.

**Generating Unit Test Stubs of SignalGen**
```
cd fprime/Ref/SignalGen
fprime-util impl-ut
```

This creates the following files, that are typically moved to a sub folder called `test/ut`.  The files created are 
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

## Building and Running Unit Tests

Unit tests can be build using the the `build-ut` command of the `fprime-util`.


**Building Unit Test of SignalGen**
```
cd fprime/Ref/SignalGen
fprime-util build-ut
```

Once built, the unit test can be run using the following command.
```
cd fprime/Ref/SignalGen
fprime-util check
```

The output of the unit test should be displayed in the console.

## Conclusion

The user should now be familiar with F´ terminology and with the `fprime-util` tool used to build and develop F´
applications. The next step is to follow the full `MathComponent` tutorial to create new *Ports*, *Components*, and 
*Topologies*. This will walk the user through the entire development process, using the tool commands we learned here.

**Next:** [Math Component Tutorial](../MathComponent/Tutorial.md)
