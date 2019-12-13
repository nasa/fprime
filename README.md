# F´: A Flight-Proven, Multi-Platform, Open-Source Flight Software Framework

F´ (F Prime) is a component-driven framework that enables rapid development and deployment of spaceflight and other embedded software applications. Originally developed at the Jet Propulsion Laboratory, F´ has been successfully deployed on several space applications. It is tailored, but not limited, to small-scale spaceflight systems such as CubeSats, SmallSats, and instruments. 

F´ comprises several elements: 

* An architecture that decomposes flight software into discrete components with well-defined interfaces
* A C++ framework that provides core capabilities such as message queues and threads
* Modeling tools for specifying components and connections and automatically generating code
* A growing collection of ready-to-use components
* Testing tools for testing flight software at the unit and integration levels.

## Quick Installation Guide

F´ can be quickly install using the following instructions. F´ requires that the following utilities be installed: cmake, git, and Python 3 with pip. Once these have been installed, users are 
recommended to install F´ python dependencies. This is usually done in a Python virtual environment as this prevents issues at the system level, but is not required. Full installation instructions
including virtual environment creation, installation verification, and support for Python 2 is available: [INSTALL.md](INSTALL.md). The following are the most basic steps for convenience.

```
git clone https://github.com/nasa/fprime.git
cd fprime
pip install Fw/Python Gds
```

## Tutorials

F´ provides several tutorials in order to help understand and develop within the framework. These tutorials cover basic component creation, system and topology design, tooling, and more. These 
tutorials are available here: https://github.com/nasa/fprime/tree/master/docs/Tutorials

## User Guide, Architecture Specification, and Further Documentation

The Reference application is shipped as part of F'. Documentation for this reference application can be found [here.](Ref/docs/sdd.md)

The full F' User's guide can be found [here.](docs/UsersGuide/FprimeUserGuide.pdf) In addition, the F' architectural overview can be found [here.](docs/Architecture/FPrimeArchitectureShort.pdf)

### Continuous Integration

The continuous integration system performs builds and unit-test check on any pull-requests created on the F´ core. Thus ensuring that F´ core is stable and well maintained.

### Cosmos Integration

More information can be found here: [Autocoders/Python/src/fprime_ac/utils/cosmos/README.md](Autocoders/Python/src/fprime_ac/utils/cosmos/README.md)


## F´ Community

As F´ becomes a community centered product line, there are more items available from the community at large. This information
may be found at: [https://github.com/fprime-community](https://github.com/fprime-community).

The F´ community contains course materials on flight quality software, component libraries that may be of use, and more.


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
* Prototype CMake build system. See: [CMake Readme](cmake/README.md)
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

