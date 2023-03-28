---
layout: default
title: "F´ Tutorials"
---

These tutorials will walk the user from a basic getting-started guide through designing a full system for use with F´. This will help new
users to learn F´ and walk through the most basic steps in developing an F´ application.

## Tutorials Index

0. [GettingStarted](HelloWorld/Tutorial.md)
1. [MathComponent](MathComponent/Tutorial.md)
2. [Cross-Compilation Tutorial](CrossCompilation/Tutorial.md)
3. [GpsTutorial](GpsTutorial/Tutorial.md)

## [Getting Started](HelloWorld/NewProject.md)

The getting started tutorial walks a new user through creating a new project, their first F´ component, and testing that
component through an F´ deployment. This tutorial has the following subsections:

1. [Introduction and F´ Terminology](./HelloWorld/Tutorial.md)
2. [Creating an F´ Project](./HelloWorld/NewProject.md)
3. [Creating an F´ Hello World Component](./HelloWorld/HelloWorld.md)
4. [Integration and Testing With F´ Deployments](./HelloWorld/Deployments.md)

## [Math Component](MathComponent/Tutorial.md)

A tutorial to walk through the full development process of an F´ application. It does this by creating two components,
one to request the results of a math operation, and the other to compute the result. This allows users to see F´
components communicate and run and F´ project without the need for embedded devices nor hardware. This tutorial runs entirely
on a user's *nix (Linux, Mac, BSD, Windows w/ WSL) machine. It uses the existing `Ref` application as a host for the components
allowing a quick development. Users should first review the [Getting Started Tutorial](HelloWorld/Tutorial.md) for
understanding the tools.

## [Cross-Compilation Tutorial](CrossCompilation/Tutorial.md)

A tutorial that walks the user through the process of cross-compiling a
complete Math Component application to the Raspberry Pi. This allows users to
gain a basic understanding of how to cross-compile an F´ application and run
the deployment on the Raspberry Pi. To get the most from this tutorial the user
should possess a Raspberry Pi. Users should first review the 
[Getting Started Tutorial](HelloWorld/Tutorial.md) for understanding the
tools and complete the [Math Component Tutorial](MathComponent/Tutorial.md) to
build familiarity with F´ and to have a complete Ref application.

## [Gps Tutorial](GpsTutorial/Tutorial.md)

A tutorial covering the interaction with a real GPS receiver. This will cover how to design driver components, application components,
and cross-compiling onto a Raspberry Pi. The purpose of this tutorial is to expand from F´ running on *nix machines into the world
of embedded hardware. To get the most from this tutorial, the user should possess a Raspberry Pi, have access to a Linux machine or
a Linux VM, and have a small UART/USB GPS receiver. The ones with a USB mode work best.  Users should first review the
[Getting Started Tutorial](HelloWorld/Tutorial.md) for understanding the tools and would benefit from running through the
[Math Component Tutorial](MathComponent/Tutorial.md) to build familiarity with F´.
