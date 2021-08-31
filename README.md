# F´: A Flight-Proven, Multi-Platform, Open-Source Flight Software Framework

[![Language grade: C++](https://img.shields.io/lgtm/grade/cpp/g/nasa/fprime.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/nasa/fprime/context:cpp)
[![Language grade: Python](https://img.shields.io/lgtm/grade/python/g/nasa/fprime.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/nasa/fprime/context:python)

**F´ (F Prime)** is a component-driven framework that enables rapid development and deployment of spaceflight and other embedded software applications. Originally developed at the [Jet Propulsion Laboratory](https://www.jpl.nasa.gov/), F´ has been successfully deployed on several space applications. It is tailored but not limited to small-scale spaceflight systems such as CubeSats, SmallSats, and instruments.

**Please Visit the F´ Website:** [https://nasa.github.io/fprime/](https://nasa.github.io/fprime/).  This website contains project information, user guides, documentation, tutorials, and more!

F´ comprises several elements:

* An architecture that decomposes flight software into discrete components with well-defined interfaces
* A C++ framework that provides core capabilities such as message queues and threads
* Modeling tools for specifying components and connections and automatically generating code
* A growing collection of ready-to-use components
* Testing tools for testing flight software at the unit and integration levels.

## Quick Installation Guide

The following utilities are prerequisites to installing F´:

* [cmake](https://cmake.org/)
* [git](https://git-scm.com/)
* [Python](https://www.python.org/) 3.6+ with pip

Once these utilities are installed, you can install F´ Python dependencies. Installing dependencies in a Python virtual environment prevents issues at the system level, but installing in a virtual environment is not required.

To install F´ quickly, enter:

```shell
git clone https://github.com/nasa/fprime.git
pip install --upgrade fprime-tools fprime-gds
```

For full installation instructions, including virtual environment creation and installation verification, see [INSTALL.md](./docs/INSTALL.md).

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

A former source of information, now depreciated, is available on the F´ Community Group [https://groups.google.com/d/forum/fprime-community](https://groups.google.com/g/fprime-community).

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

## F´ Release Notes

### Release 2.0: Release and Migration Notes

Version 2.0.0 of F´ represents major improvements across the F´ framework. As such, some work may be required to migrate from other versions of F´ to the new
functionality. This section will offer recommendations to migrate to version 2.0.0 of F´.

Features and Functionality:

* New ground interface change improves stability and flexibility
  * `Svc::Framer` and `Svc::Deframer` components may be used in place of `Svc::GroundInterface`
  * `Svc::Framer` and `Svc::Deframer` delegate to a user instantiated framing class allowing use of non-fprime framing protocols
* `Drv::ByteStreamDriverModel` allows implementing drivers reading/writing streams of bytes using a single model
* New IPv4 drivers implement `Drv::ByteStreamDriverModel` allowing choice or combination of uplink and downlink communications
  * `Drv::TcpClient` is a tcp client that connects to a remote server
  * `Drv::TcpServer` is a tcp server that allows connections from remote clients
  * `Drv::Udp` allows UDP communications
  * `Drv::SocketIpDriver` may be replaced using a choice of an above component.
* `Svc::FileDownlink` now supports a queue of files to downlink and a port to trigger file downlinks
* `Svc::FileDownlink` may now be configured to turn off certain errors
* `Svc/GenericHub` is a basic instantiation of the hub pattern
* Bug fixes and stability improvements

Migration considerations:

* F´ tooling (fprime-util and fprime-gds) should be installed using `pip install fprime-tools fprime-gds`
* `Os::File::open` with the mode CREATE will now properly respect O_EXCL and error if the file exists. Pass in `false` as the final argument to override.
* Revise uses of `Fw::Buffer` to correct usage of member functions using camel case.  E.g. `Fw::Buffer::getsize` is now `Fw::Buffer::getSize`
* The ground interface chain has been refactored. Projects may switch to using `Svc::Framer`, `Svc::Deframer`, and any implementor of `Drv::ByteStreamDriverModel` to supply the data.  To continue using the old interface with the GDS run `fprime-gds --comm-checksum-type fixed`.
* `Svc::BufferManager` has been reworked to remove errors. When instantiating it please supply a memory allocator as shown in `Ref`.
* Dictionaries, binaries, and other build outputs now are written to a deployments `build_artifacts` folder.

**Deprecated Functionality:** The following features are or will be deprecated soon and may be removed in future releases.

* `Svc::GroundInterface` and `Drv::SocketIpDriver` should be replaced by the new ground system components.
* Inline enumerations (enumerations defined inside the definition of a command/event/channel) should be replaced by EnumAi.xml implementations
* `fprime-util generate --ut -DFPRIME_ENABLE_FRAMEWORK_UTS=OFF` will be removed in favor of future `fprime-util check` variants
* `Autocoders/MagicDrawCompPlugin` will be removed in a near-term release

### Release 1.0

* This is the initial release of the software to open source. See the license file for terms of use.

### Release 1.01

* Updated contributor list. No code changes.

### Release 1.1

* Created a Raspberry Pi demo. Read about it [here](RPI/README.md)
* Added a tutorial [here](docs/Tutorials/README.md)
* Updated Svc/BufferManager with bug fix
* Fixed a bunch of shell permissions

### Release 1.2

* Better MagicDraw Plugin
* Prototype CMake build system. See: [CMake Documentation](./docs/UsersGuide/cmake/cmake-intro.md)
* Mars Helicopter Project fixes migrated in
* Python 3 support added
* Gse refactored and renamed to Gds
* Wx frontend to Gds
* UdpSender and UdpReceiver components added
* Purged inaccurate ITAR and Copyright notices
* Misc. bug fixes

### Release 1.3

* New prototype HTML GUI
* Python packages Fw/Python and Gds
* Refined CMake and fprime-util helper script
* Better ground interface component
* Integration test API
* Baremetal components

### Release 1.4

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

### Release 1.5

* Documentation improvements
  * New user's guide containing considerable content: [https://nasa.github.io/fprime/UsersGuide/guide.html](https://nasa.github.io/fprime/UsersGuide/guide.html)
  * Auto-generated API documentation
  * Rewrites, edits, improvements across the board
* F´ Project restructuring
  * Projects may now link to F´ and F´ library packages, without needing to keep the framework code in the same source tree
  * Usage of framework can be out-of-source
  * `settings.ini` Introduced
  * Example: [https://github.com/fprime-community/fprime-arduino](https://github.com/fprime-community/fprime-arduino)
* Refactored `fprim-util`
  * Replaced redundant targets with flags e.g. build-ut is now build --ut
  * Added `info` command
  * Bug and usability fixes
* GDS Improvements
  * Prototype GDS CLI tool
  * Project custom dashboard support
* Array, Enum type support and examples
* Code linting and bug fixes
