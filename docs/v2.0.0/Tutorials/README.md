---
layout: default
title: "F´ Tutorials"
---

These tutorials will walk the user from a basic getting-started guide through designing a full system for use with F´. This will help new
users to learn F´ and walk through the most basic steps in developing an F´ application.

## Tutorials Index

0. [GettingStarted](GettingStarted/Tutorial.md)
1. [MathComponent](MathComponent/Tutorial.md)
2. [GpsTutorial](GpsTutorial/Tutorial.md)

## [Getting Started](GettingStarted/Tutorial.md)

A basic tutorial that will help the user start using F´, build and run a system, and learn the basic items in the system.
This will include looking through existing portions of the `Ref` app in order to learn the tools and terminologies of an
F´ project. This is the **best** place to start for learning terminology and tools.

## [Math Component](MathComponent/Tutorial.md)

A tutorial to walk through the full development process of an F´ application. It does this by creating two components,
one to request the results of a math operation, and the other to compute the result. This allows users to see F´
components communicate and run and F´ project without the need for embedded devices nor hardware. This tutorial runs entirely
on a user's *nix (Linux, Mac, BSD, Windows w/ WSL) machine. It uses the existing `Ref` application as a host for the components
allowing a quick development. Users should first review the [Getting Started Tutorial](GettingStarted/Tutorial.md) for
understanding the tools.

## [Gps Tutorial](GpsTutorial/Tutorial.md)

A tutorial covering the interaction with a real GPS receiver. This will cover how to design driver components, application components,
and cross-compiling onto a Raspberry Pi. The purpose of this tutorial is to expand from F´ running on *nix machines into the world
of embedded hardware. To get the most from this tutorial, the user should possess a Raspberry Pi, have access to a Linux machine or
a Linux VM, and have a small UART/USB GPS receiver. The ones with a USB mode work best.  Users should first review the
[Getting Started Tutorial](GettingStarted/Tutorial.md) for understanding the tools and would benefit from running through the
[Math Component Tutorial](MathComponent/Tutorial.md) to build familiarity with F´.
