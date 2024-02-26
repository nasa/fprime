<p align="center">
<img width="230em" src="docs/img/fprime-logo.png"><br/>
<h2 align="center">A Flight-Proven, Multi-Platform, Open-Source Flight Software Framework</h2>
</p>

#
F´ (F Prime) is a component-driven framework that enables rapid development and deployment of spaceflight and other embedded software applications. Originally developed at the [Jet Propulsion Laboratory](https://www.jpl.nasa.gov/), F´ has been successfully deployed on [several space applications](https://nasa.github.io/fprime/projects.html). It is tailored but not limited to small-scale spaceflight systems such as CubeSats, SmallSats, and instruments.

**Please Visit the F´ Website:** [https://nasa.github.io/fprime](https://nasa.github.io/fprime/) for more information.


## What does F´ provide

- An architecture that decomposes flight software into discrete components with well-defined interfaces
- A C++ framework that provides core capabilities such as message queues and threads
- Modeling tools for specifying components and connections and automatically generating code
- A growing collection of ready-to-use components
- Testing tools for testing flight software at the unit and integration levels.

Learn more about F' key features [here](https://nasa.github.io/fprime/features.html).


## System Requirements

1. Linux, Windows with WSL, or macOS operating system
2. git
3. [CMake 3.16+](https://cmake.org/download/). CLI tool must be available on the system path.
4. CLang or GNU C and C++ compilers (e.g. gcc and g++)
5. [Python 3.8+](https://www.python.org/downloads/), virtual environments, and PIP


## Getting Started

To get started with F´, install the F´ bootstrapping tool with:
```
pip install fprime-bootstrap
```

Then, create a new project with:
```
fprime-bootstrap project
```

See the [HelloWorld Tutorial](https://fprime-community.github.io/fprime-tutorial-hello-world/) to guide you through all the steps of developing an F´ project.

New users are encouraged to read through the [User Guide](https://nasa.github.io/fprime/UsersGuide/guide.html) and explore the [other tutorials](https://nasa.github.io/fprime/Tutorials/README.html).


## Getting Help

### Discussions
To ask questions, discuss improvements, and ask for help please use the project's [GitHub Discussions](https://github.com/nasa/fprime/discussions).
### Bug reports
To report bugs and issues, [open an issue here](https://github.com/nasa/fprime/issues).
### Community
The [F´ Community](https://github.com/fprime-community) GitHub Organization contains third party contributions, more documentation of flight software development, and additional resources.


## Resources
- [User Guide](https://nasa.github.io/fprime/UsersGuide/guide.html)
- [Tutorials](https://nasa.github.io/fprime/Tutorials/README.html)
- [Discussions](https://github.com/nasa/fprime/discussions)
- [Submit an Issue](https://github.com/nasa/fprime/issues)
- [F´ Community](https://github.com/fprime-community)


## Release Notes

The version history and artifacts associated with the project can be found at [Releases](https://github.com/nasa/fprime/releases).