# F´: A Flight-Proven, Multi-Platform, Open-Source Flight Software Framework

[![Language grade: C++](https://img.shields.io/lgtm/grade/cpp/g/nasa/fprime.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/nasa/fprime/context:cpp)
[![Language grade: Python](https://img.shields.io/lgtm/grade/python/g/nasa/fprime.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/nasa/fprime/context:python)
[![Language grade: JavaScript](https://img.shields.io/lgtm/grade/javascript/g/nasa/fprime.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/nasa/fprime/context:javascript)

F´ (F Prime) is a component-driven framework that enables rapid development and deployment of spaceflight and other embedded software applications. Originally developed at the Jet Propulsion Laboratory, F´ has been successfully deployed on several space applications. It is tailored, but not limited, to small-scale spaceflight systems such as CubeSats, SmallSats, and instruments.

F´ comprises several elements:

* An architecture that decomposes flight software into discrete components with well-defined interfaces
* A C++ framework that provides core capabilities such as message queues and threads
* Modeling tools for specifying components and connections and automatically generating code
* A growing collection of ready-to-use components
* Testing tools for testing flight software at the unit and integration levels.

## Quick Installation Guide

F´ can be quickly install using the following instructions. F´ requires that the following utilities be installed: cmake, git, and Python 3.5+ with pip. Once these have been installed, users are
recommended to install F´ python dependencies. This is usually done in a Python virtual environment as this prevents issues at the system level, but is not required. Full installation instructions
including virtual environment creation, and installation verification: [INSTALL.md](./docs/INSTALL.md). The following are the most basic steps for convenience.

```
git clone https://github.com/nasa/fprime.git
cd fprime
pip install --upgrade wheel setuptools pip
pip install Fw/Python Gds/
```

## Example Deployments

F´ comes with two example deployments. The deployments represent working F´ applications for help understanding F´. These examples may be used
as reference, or cloned to start a new project. More step-by-step tutorials are linked in the next section, but as these are complete applications
it is recommended that the user at least build and run the first one to ensure that F´ is installed correctly.

[Ref](./Ref/README.md): The standard reference application demonstrates how most of the system components should be wired together. In addition,
the reference application can be built on Linux or Mac OSX allowing the user to get started immediately without the need for embedded hardware.

[RPI](./RPI/README.md): The Raspberry PI application shows how to run F´ in an embedded context by running on the Raspberry PI (a $35 embedded
Linux computer). This application shows the user how to get started in the embedded world with cross-compiling, dirvers, and more. The Raspberry
Pi was chosen as it is comercially available for a low price and runs Linux.

## Tutorials

F´ provides several tutorials in order to help understand and develop within the framework. These tutorials cover basic component creation, system
 and topology design, tooling, and more. These tutorials are available here: [docs/Tutorials/README.md](./docs/Tutorials/README.md).

## Getting Help with F´

As F´ becomes a community centered product line, there are more items available from the community at large. A mailing list is available
at [https://groups.google.com/d/forum/fprime-community](https://groups.google.com/d/forum/fprime-community).

In addition we have a community GitHub Organization containing third party contributions, further documentation of flight
software development and more! [https://github.com/fprime-community](https://github.com/fprime-community).

Issues can be submitted to: [https://github.com/nasa/fprime/issues](https://github.com/nasa/fprime/issues)

## F´ Features

F´ has the following key features that enable robust embedded system design.

### Reusability

F´'s component-based architecture enables a high degree of modularity and software reuse.

### Rapid Deployment

F´ provides a complete development ecosystem, including modeling tools, testing tools, and a ground data system. Developers use the modeling tools to write high-level specifications, automatically generate implementations in C++, and fill in the implementations with domain-specific code. The framework and the code generators provide all the boilerplate code required in an F´ deployment, including code for thread management, code for communication between components, and code for handling commands, telemetry, and parameters. The testing tools and the ground data system simplify software testing, both on workstations and on flight hardware in the lab.

### Portability

F´ runs on a wide range of processors, from microcontrollers to multicore computers, and on several operating systems. Porting F´ to new operating systems is straightforward.

### High Performance

F´ utilizes a point-to-point architecture. The architecture minimizes the use of computational resources and is well suited for smaller processors.

### Adaptability

F´is tailored to the level of complexity required for small missions. This makes it accessible and easy to use, while still supporting a wide variety of missions.

### Analyzability

The typed port connections provide strong compile-time guarantees of correctness.

## F´ Release Notes

#### Release 1.0:

 * This is the initial release of the software to open source. See the license file for terms of use.

#### Release 1.01

 * Updated contributor list. No code changes.

#### Release 1.1

 * Created a Raspberry Pi demo. Read about it [here.](RPI/README.md)
 * Added a tutorial [here.](docs/Tutorials/README.md)
 * Updated Svc/BufferManager with bug fix
 * Fixed a bunch of shell permissions
 
#### Release 1.2

* Better MagicDraw Plugin
* Prototype CMake build system. See: [CMake Documentation](./docs/UsersGuide/cmake/cmake-intro.md)
* Mars Helicopter Project fixes migrated in
* Python 3 support added
* Gse refactored and renamed to Gds
* Wx frontend to Gds
* UdpSender and UdpReceiver components added
* Purged inaccurate ITAR and Copyright notices
* Misc. bug fixes

#### Release 1.3

* New prototype HTML GUI
* Python packages Fw/Python and Gds
* Refined CMake and fprime-util helper script
* Better ground interface component
* Integration test API
* Baremetal components

#### Release 1.4

* Ref app no longer hangs on Linux exit
* GDS improvements:
  * File Uplink and Downlink implemented
  * GDS supports multiple active windows
  * Usability improvements for EVRs and commands
* CMake improvements:
  * Baremetal compilation supported
  * Random rebuilding fixed
  * Missing Cheetah templates properly rebuild
  * Separate projects supported without additional tweaks
* Updated MemAllocator to have:
  * "recoverable" flag to indicate if memory was recoverable across boots
  * size variable is now modifiable by allocator to indicate actual size
  * This will break existing code that uses MemAllocator
* Updated CmdSequencer
  * Uses new MemAllocator interface  

#### Release 1.5

* Documentation improvements
  * New user's guide containing considerable content: [https://nasa.github.io/fprime/UsersGuide/guide.html](https://nasa.github.io/fprime/UsersGuide/guide.html)
  * Auto-generated API documentation
  * Rewrites, edits, improvements across the board
* F´ Project restructuing
  * Projects may now link to F´ and F´ library packges, without needing to keep the framework code in the same source tree
  * Usage of framework can be out-of-source
  * `settings.ini` Introduced
  * Example: [https://github.com/fprime-community/fprime-arduino](https://github.com/fprime-community/fprime-arduino)
* Refactored `fprim-util`
  * Replaced redundant targets with flags e.g. build-ut is now build --ut
  * Added `info` command
  * Bug and usability fixes
* GDS Improvments
  * Prototype GDS CLI tool
  * Project custom dashboard support
* Array, Enum type support and examples
* Code linting and bug fixes
