# Introduction

This tutorial shows how to develop, test, and deploy a simple topology
consisting of two components:

1. `MathSender`: A component that receives commands and forwards work to
   `MathReceiver`.

1. `MathReceiver`: A component that carries out arithmetic operations and
   returns the results to `MathSender`.

See the diagram below.

![A simple topology for arithmetic computation](png/top.png)

**What is covered:** The tutorial covers the following concepts:

1. Using the [FPP modeling language](https://fprime-community.github.io/fpp) to
   specify the types and ports used by the components.

1. Using the F Prime build system to build the types and ports.

1. Developing the `MathSender` component: Specifying the component, building
   the component, completing the C++ component implementation, and writing
   component unit tests.

1. Developing the `MathReceiver` component.

1. Adding the new components and connections to the F Prime `Ref` application.

1. Using the F Prime Ground Data System (GDS) to run the updated `Ref`
   application.

**Prerequisites:** This tutorial assumes the following:

1. Basic knowledge of Unix: How to navigate in a shell and execute programs.

1. Basic knowledge of git: How to create a branch.

1. Basic knowledge of C++, including class declarations, inheritance,
and virtual functions.

If you have not yet installed F Prime on your system, do so now.
Follow the installation guide at `INSTALL.md`
in the [F Prime git repository](https://github.com/nasa/fprime).
You may also wish to work through the Getting Started tutorial at
`docs/GettingStarted/Tutorial.md`.

**Git branch:** This tutorial is designed to work on the branch `release/v3.0.0`.

Working on this tutorial will modify some files under version control in the
F Prime git repository.
Therefore it is a good idea to do this work on a new branch.
For example:

```bash
git checkout release/v3.0.0
git checkout -b math-tutorial
```

If you wish, you can save your work by committing to this branch.

# The MathOp Type

In F Prime, a **type definition** defines a kind of data that you can pass
between components or use in commands and telemetry.

For this tutorial, we need one type definition.
It defines an enumeration called `MathOp`, which
represents a mathematical operation.

We will add the specification for the `MathOp` type to the
`Ref` topology.
We will do this in three stages:

1. Construct the FPP model.

1. Add the model to the project.

1. Build the model.

## Construct the FPP Model

**Create the MathTypes directory:**
Go to the directory `Ref` at the top-level of the
F Prime repository and run `mkdir MathTypes`.
This step creates a new directory `Ref/MathTypes`.
This directory will contain our new type.

**Create the FPP model file:**
Now go into the directory `Ref/MathTypes`.
In that directory, create a file `MathTypes.fpp` with the following contents:

```fpp
!include ../MathTypes/MathTypes.fpp
```

You can do this by typing, or by copy-paste.

This file defines an enumeration or **enum** with enumerated constants `ADD`,
`SUB`, `MUL`, and `DIV`.
These four constants represent the operations of addition, subtraction,
multiplication, and division.
The enum also defines a type `MathOp`; the enumerated constants are the values
of this type.
For more information on enums, see [_The FPP User's 
Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Defining-Enums).

The enum `MathTypes` resides in an FPP module `Ref`.

An FPP module is like a C++ namespace: it encloses several definitions, each of
which is qualified with the name of the module.
For more information on FPP modules, see [_The FPP User's
Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Defining-Modules).

The text following a symbol `@` or `@<` is called an **annotation**.
These annotations are carried through the parsing and become comments in the
generated code.
For more information, see [_The FPP User's
Guide_](https://fprime-community.github.io/fpp/fpp-users-guide.html#Writing-Comments-and-Annotations).

<a name="types_add"></a>
## Add the Model to the Project

**Create Ref/MathTypes/CMakeLists.txt:**
Create a file `Ref/MathTypes/CMakeLists.txt` with the following contents:

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathTypes.fpp"
)

register_fprime_module()
```

This code will tell the build system how to build the FPP model.

**Update Ref/CMakeLists.txt:**
Now we need to add the new directory to the `Ref` project.
To do that, open the file `Ref/CMakeLists.txt`.
This file should already exist; it was put there by the developers
of the `Ref` topology.
In this file, you should see several lines starting with `add_fprime_subdirectory`.
Immediately after the last of those lines, add the following new line:

```cmake
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MathTypes/")
```

<a name="types_build"></a>
## Build the Model

**Run the build:**
Do the following:

1. Go to the directory `Ref/MathTypes`.

1. If you have not already run `fprime-util generate`, then do so now.

1. Run the command `fprime-util build`.

The output should indicate that the model built without any errors.
If not, try to identify and correct what is wrong,
either by deciphering the error output, or by going over the steps again.
If you get stuck, you can look at the
<a href="#types_ref">reference implementation</a>.

**Inspect the generated code:**
Now go to the directory `Ref/build-fprime-automatic-native/Ref/MathTypes`
(you may want to use `pushd`, or do this in a separate shell,
so you don't lose your current working directory).
The directory `build-fprime-automatic-native` is where all the
generated code lives for the "automatic native" build of the `Ref`
project.
Within that directory is a directory tree that mirrors the project
structure.
In particular, `Ref/build-fprime-automatic-native/Ref/MathTypes`
contains the generated code for `Ref/MathTypes`.

Run `ls`.
You should see something like this:

```bash
CMakeFiles            MathOpEnumAc.cpp      MathOpEnumAi.xml.prev cmake_install.cmake
Makefile              MathOpEnumAc.hpp      autocoder
```

The files `MathOpEnumAc.hpp` and
`MathOpEnumAc.cpp` are the auto-generated {cpp} files
corresponding to the `MathOp` enum.
You may wish to study the file `MathOpEnumAc.hpp`.
This file gives the interface to the {cpp} class `Ref::MathOp`.
All enum types have a similar auto-generated class
interface.

<a name="types_ref"></a>
## Reference Implementation

A reference implementation for this section is available at
`docs/Tutorials/MathComponent/MathTypes`.
To build this implementation from a clean repository,
do the following:

1. Go to the `Ref` directory.

1. Run `cp -R ../docs/Tutorials/MathComponent/MathTypes .`

1. Update `Ref/CMakeLists.txt` as stated <a href="#types_add">above</a>.

1. Follow the steps for <a href="#types_build">building the model</a>.

If you have modified the repo, revise the steps accordingly.
For example, switch git branches, use `git stash` to stash
your changes, or move `MathTypes` to another directory such
as `MathTypes-saved`.

