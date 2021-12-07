# 1. Introduction

This tutorial shows how to develop, test, and deploy a simple topology
consisting of two components:

1. `MathSender`: A component that receives commands and forwards work to `MathReceiver`.

1. `MathReceiver`: A component that carries out arithmetic operations and 
returns the results to `MathSender`.

See the diagram below.

![A simple topology for arithmetic computation](png/top.png)

**What is covered:** The tutorial covers the following concepts:

1. Using the [FPP modeling language](https://fprime-community.github.io/fpp)
to specify the types and ports used by the components.

1. Using the F Prime build system to build the types and ports.

1. Developing the `MathSender` component: Specifying the component,
building the component,
completing the C++ component implementation, and writing
component unit tests.

1. Developing the `MathReceiver` component.

1. Adding the new components and connections to the F Prime
`Ref` application.

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

