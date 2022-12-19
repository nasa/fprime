# F´: A Flight-Proven, Multi-Platform, Open-Source Flight Software Framework

**F´ (F Prime)** is a component-driven framework that enables rapid development and deployment of spaceflight and other embedded software applications. Originally developed at the [Jet Propulsion Laboratory](https://www.jpl.nasa.gov/), F´ has been successfully deployed on several space applications. It is tailored but not limited to small-scale spaceflight systems such as CubeSats, SmallSats, and instruments.

**Please Visit the F´ Website:** [https://nasa.github.io/fprime/](https://nasa.github.io/fprime/).  This website contains project information, user guides, documentation, tutorials, and more!

F´ comprises several elements:

* An architecture that decomposes flight software into discrete components with well-defined interfaces
* A C++ framework that provides core capabilities such as message queues and threads
* Modeling tools for specifying components and connections and automatically generating code
* A growing collection of ready-to-use components
* Testing tools for testing flight software at the unit and integration levels.

For installation instructions, including virtual environment creation and installation verification, see [INSTALL.md](./docs/INSTALL.md).

## Example Deployments

F´ comes with two example deployments. The deployments represent working F´ applications to help you understand F´. You can use these examples for reference, or clone them to start a new project.

The next section links to more step-by-step tutorials, but it's a good idea to build and run at least the first example deployment to ensure that F´ is installed correctly.

**Example one:** [Ref](./Ref/README.md)

   The standard reference application demonstrates how most of the system components should be wired together. The reference application can build on Linux or macOS, allowing you to get started immediately without the need for embedded hardware.

**Example two:** [RPI](./RPI/README.md)

This Raspberry PI application shows how to run F´ in an embedded context by running on the Raspberry PI (a $35 embedded Linux computer). This application shows you how to get started on embedded projects with cross-compiling, drivers, and more. The Raspberry Pi was chosen because it is commercially available for a low price and runs Linux.

## Tutorials

F´ provides several tutorials in order to help understand and develop within the framework. These tutorials cover basic component creation, system and topology design, tooling, and more. These tutorials are available at [docs/Tutorials/README.md](./docs/Tutorials/README.md).

## Getting Help with F´

As F´ becomes a community centered product line, there are more items available from the community at large.

To ask questions, discuss improvements, ask for help, please use the project's GitHub Discussions at: [https://github.com/nasa/fprime/discussions](https://github.com/nasa/fprime/discussions).

The F´ community GitHub Organization contains third party contributions, more documentation of flight software development, and more! [https://github.com/fprime-community](https://github.com/fprime-community).

You can open issues with this repository at: [https://github.com/nasa/fprime/issues](https://github.com/nasa/fprime/issues)

## F´ Features

F´ has the following key features that enable robust embedded system design.

### Reusability

F´'s component-based architecture enables a high degree of modularity and software reusability.

### Rapid Deployment

F´ provides a complete development ecosystem, including modeling tools, testing tools, and a ground data system. Developers use the modeling tools to write high-level specifications, automatically generate implementations in C++, and fill in the implementations with domain-specific code. The framework and the code generators provide all the boilerplate code required in an F´ deployment, including code for thread management, code for communication between components, and code for handling commands, telemetry, and parameters. The testing tools and the ground data system simplify software testing, both on workstations and on flight hardware in the lab.

### Portability

F´ runs on a wide range of processors, from microcontrollers to multicore computers, and on several operating systems. Porting F´ to new operating systems is straightforward.

### High Performance

F´ utilizes a point-to-point architecture. The architecture minimizes the use of computational resources and is well suited for smaller processors.

### Adaptability

F´ is tailored to the level of complexity required for small missions. This makes F´ accessible and easy to use while still supporting a wide variety of missions.

### Analyzability

The typed port connections provide strong compile-time guarantees of correctness.

## Release Notes

The version history and artifacts associated with the project can be found at [Releases](https://github.com/nasa/fprime/releases) and [CHANGELOG.md](./CHANGELOG.md).
