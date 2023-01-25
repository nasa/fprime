# The Discerning User's Guide to F´

F´ is an embedded systems framework developed at NASA's Jet Propulsion Laboratory for use in space-based missions. Its
goal is simple: make the development of flight-quality embedded software simple, easy, and fun. F´ reduces the complexity
and overhead to develop such systems.

This guide is intended to expose the features of F´ and seek to allow users and developers to take advantage of all F´
has to offer. It is broken down into five documentation sections:
- Getting Started
- User's Manual 
- Best Practices
- Developer Documentation
- API

**The Getting Started** section points initiates to all the items needed to get up and running with F´. Since these are
often needed before a user searches for a user's guide, this section merely links to the stand-alone version of those
items.

**The User's Manual** section helps users of this framework understand the concepts of F´ and build more complex projects
using the Framework. It is where users should go to understand more about what is possible in F´.

**The Best Practices** section collects best practices and patterns many projects use while developing F´. Users who consult
this section will learn how to organize F´ projects and implementations to solve problems using standard and recommended
techniques.

**The Developer Documentation** is intended for developers who want to use some of the advanced features of F´, understand
how the framework is organized internally, and otherwise dig deep into F´ concepts and implementation. Anyone who wishes
to extend or change F´ should consult this section.
This section is divided into three sub-sections:
- Ground Data System Tools (GDS)
- Full Development Guides: technical details for full F´ implementations
- Advanced F´ Topics


**The API documentation** section contains the automatically generated documentation on F´.

**Note:** currently this documentation is being assembled. Please feel free to contribute!

## Table of Contents

- Getting Started with F´
    - [What is F´: a brief introduction](../index.md)
    - [Installing F´](../INSTALL.md)
    - [Installing F´ Console Autocomplete](./user/autocomplete.md)
    - [Tutorials: A Hands On Guide to F´](../Tutorials/README.md)
- F´ Users Manual: an in-depth description of F´ concepts
    - [A More Complete Introduction to F´](user/full-intro.md)
    - [Projects and Deployments](user/proj-dep.md)
    - [Core Constructs: Ports, Components, and Topologies](user/port-comp-top.md)
    - [Data Types and Data Structures: Primitive Types, Enums, Arrays, and Serializables](user/enum-arr-ser.md)
    - [Data Constructs: Commands, Events, Channels, and Parameters](user/cmd-evt-chn-prm.md)
    - [Unit Testing F´ Components](./user/unit-testing.md)
- F´ Best Practices: helpful patterns when developing F´ software
    - [F´ Development Process](./best/development-practice.md)
    - [Application, Manager, Driver Pattern](./best/app-man-drv.md)
    - [Ground Interface](./best/ground-interface.md)
    - [Rate Groups and Timeliness](./best/rate-group.md)
    - [Dynamic Memory and Buffer Management](./best/dynamic-memory.md)
    - [A Quick Look at the Hub Pattern](./best/hub-pattern.md)
    - [Documenting F´ Projects](./best/documentation.md)
    - [Code and Style Guidelines](./dev/code-style.md)
- F´ Ground Data System Tools (GDS)
    - [A Brief Guide to the F´ Ground Data System](./gds/gds-introduction.md)
    - [The Discerning User's Guide to the F´ GDS CLI](./gds/gds-cli.md)
    - [The GDS Dashboard](./gds/gds-custom-dashboards.md)
    - [Sequencing in F´](./gds/seqgen.md)
- Full Development Guides: technical details for full F´ implementations
    - [Configuring F´](./dev/configuring-fprime.md)
    - [v3 Migration Guide](./user/v3-migration-guide.md)
    - [A Tour of the Source Tree](./dev/source-tree.md)
    - [F´ XML Specifications](./dev/xml-specification.md):
    - [F´ Implementation Classes](./dev/implementation.md)
    - [Constructing the F´ Topology](./dev/building-topology.md)
    - [Asserts in F´](./dev/assert.md)
    - [GDS Dashboard Reference](./dev/gds-dashboard-reference.md)
    - [Integration Test API](./dev/testAPI/user_guide.md)
- Advanced F´ Topics:
    - [F´ Python Guidelines](./dev/py-dev.md)
    - [Porting F´ To a New Platform](./dev/porting-guide.md)
    - [F´ On Baremetal and Multi-Core Systems](./dev/baremetal-multicore.md)
    - [Configuring an IDE for Use With F´](./dev/configure-ide.md)
    - [OS Layer Description](./dev/os-docs.md)
- API Documentation
    - [GDS CLI Design](./dev/gds-cli-dev.md)
    - [C++ Documentation](./api/c++/html/index.html)



## F´ Source Tree F´ Out of the Box Items

The rest of this page will cover various items provided by F´ out-of-the-box.  Most of these items have more complete
documentation linked above.

## Framework and Service Components

F´ ships with a core framework and service code enabling the construction of F´ applications using standards.
Specifically, the service (Svc) components provide most of the basic functionality to command and communicate with
an embedded system such as a spacecraft.

## Autocoder

Autocoder automatically adds ports for registering commands, receiving
commands, and reporting an execution status. F′ defines an Extensible
Markup Language (XML) schema that users can use to specify a model of a
FSW application. The model describes the application at a high level in
terms of the components, ports, and topologies of the F′ architecture.
The F′ autocoder translates the model into the C++ classes to greatly reduce the boilerplate code required to build
an embedded system.

## Operating System Abstraction Layer

An operating system abstraction layer (OSAL) provides implementations of an application programming interface (API) for
real-time operating systems (RTOS) and non-real-time operating systems in a generic way such that the code using the
OSAL layer can easily swap to another OS without modification.

The F′ framework OSAL includes C++  classes that provide abstractions of common operating systems features. These
features include threads, mutual exclusion locks, message queues, files, timers, and clocks.

The open-source framework provides implementations of the OS layer for Linux and Mac OS. The Linux
implementation works in Windows using WSL. Some other operating systems are provided in extension libraries.

## Ground Data System (GDS)

See: [F´ GDS](./gds/gds-introduction.md)

The F′ user experience is intended to be an out-of-box ready-to-use ground data system (GDS) solution than can easily
run on Linux, Mac OSX, or Windows platforms without any mission-specific tailoring. The GDS provides an end-user
graphical user interface (GUI) tool and an integration test application programming interface (API) to enable
integration testing and quick-look telemetry monitoring.

Initially, the user defines the mission-specific components with commands and telemetry, and then the dictionaries
are automatically generated from this design. These dictionaries are read into the GDS to provide the user experience.


## F′ Utility Build Helper and CMake System

F´ ships with a build system configured to build the F´ code. This build system is implemented using CMake and handles
the dependencies needed to run the Autocoder and assemble it with user-written code. In order to support more standard
development patterns with F´, the `fprime-util` was built. This tool maps simple commands into the make system in order
to allow a developer to follow set patterns easily.

## Linux Drivers

F´ ships with some basic driver implementations for Linux systems. These act as example driver components and allow
users choosing the Linux OS to run standard hardware like UART, I2C, SPI, etc.