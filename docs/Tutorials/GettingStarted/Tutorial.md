# Getting Started with F´

Prerequisites: [https://github.com/nasa/fprime/blob/master/INSTALL.md](https://github.com/nasa/fprime/blob/master/INSTALL.md)

This tutorial will walk the user through the basic usage of the F´ development suite. This assumes that the user has run
through the above installation guide, and has a working F´ installation.

## F´ Terminology


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
fprime-utility generate
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
fprime-utility build
```

**Build Ref Deployment**
```
cd fprime/Ref
fprime-utility build
```

Now users should be able to change components' C++, build the component to ensure that the changes compile, and then continue
to build the full deployment to yield a runnable F´ exectuable. The F´ exectuable is currently stored at
`fprime/Ref/build*/bin/*/Ref`.

This process also build the Dictionaries for the project.

## Installing the F´ Executable and Dictionaries

Once the deployment is built, it would nice to be able to install the binary and dictionaries. This will enable the users to
quickly find and run the deployment. This installation can be run using the following command. Everything will be install to
the directory defining the deployment.

**Installing the Ref Deployment**
```
cd fprime/Ref
fprime-utility install
./bin/*/Ref # Run the deployment
CTRL-C # Exit the application
```
Running the application as part of the development ground data system is shown below.






