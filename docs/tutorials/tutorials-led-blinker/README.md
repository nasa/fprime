# LED Blinker: An F´ Tutorial on Physical Hardware

This repository contains the source code and documentation for the F Prime MathComponent tutorial.

To run through the tutorial, please visit the F´ website and look for the tutorial section: https://fprime.jpl.nasa.gov

This is designed to be an extended introductory F´ tutorial taking the user through the basics of creating components,
using events, telemetry, commands, and parameters, and integrating topologies with the goal of running F´ on embedded
hardware. Users will be guided through the process of software development and testing on embedded Linux running on an
ARM processor (e.g. RaspberryPI, Odroid, etc).

## Prerequisites

In order to run through this tutorial, you must first do the following:

1. Meet the [F´ System Requirements](https://github.com/nasa/fprime#system-requirements)
2. Install an IDE or text editor supporting copy-paste. [VSCode](https://code.visualstudio.com/) has [plugins](https://marketplace.visualstudio.com/items?itemName=unlv-team5.fpptools) to work with FPP.
3. Attempt the [Hello World Tutorial](https://fprime.jpl.nasa.gov/latest/docs/tutorials/)

To run on hardware with cross-compiling, you must also:
1. Acquire and set up the appropriate [hardware](docs/led-blinker.md#appendix-optional-hardware-requirements) for this tutorial
2. Set up a [cross-compiling environment](https://github.com/nasa/fprime/blob/devel/docs/tutorials/cross-compilation.md) for their ARM processor

> [!NOTE]
> Attendees to an in-person F´ workshop will have access to 64-bit ARM hardware and should set up the 64-bit cross compiling environment.

## Contributing
If you would like to contribute to this tutorial, please open a pull request.

## Resources
- [Discussions](https://github.com/nasa/fprime/discussions)
- [Submit an Issue](https://github.com/nasa/fprime/issues)
- [F´ Community](https://github.com/fprime-community)
