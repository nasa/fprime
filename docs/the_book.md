# The F´ Book

F´ has a lot of documentation. This page is a guided reading order: each chapter introduces a set of concepts in a
paragraph, then points to the existing documents that cover them. Read the chapters in order the first time through;
come back later and use it as a map.

**Assumed background:** working knowledge of C++ and Python. No prior knowledge of CMake, flight software, or F´ is
assumed.

**How to read this book:** Parts I–III are the core path and are meant for everyone, in order. Part IV is a set of
independent chapters: pick the ones that match what you are building, in any order. Each Part IV chapter starts with
who it is for. Links marked *(reference)* are for looking things up later; skim them the first time.

**Keep the [F´ Cheatsheet](https://fprime.jpl.nasa.gov/cheatsheet) open:** it summarizes the core terms, component and
port kinds, the `fprime-util` commands, and the day-to-day development workflow on two pages.

> [!TIP]
> On the F´ website, use the documentation tree on the left to browse all pages, the table of contents on the right to
> jump to any chapter of this page, and the search bar at the top to search all of the documentation.

---

## Part I: Getting Oriented

### Chapter 1: What F´ Is

F´ is a component-based framework for flight and embedded software. A system is a set of reusable **components** that
talk to each other only through typed **ports**, wired together into a **topology**. F´ provides the framework (threads,
queues, an OS abstraction layer), a modeling language (FPP) that generates most of the boilerplate code, a library of
ready-to-use flight components, and a ground data system (GDS) to command and monitor the software. Start by reading
why F´ is designed this way; the rest of the book builds on that.

- [Introduction to F´](user-manual/overview/01-full-intro.md): origins, design goals, and how F´ deployments are organized
- [F´ Features](getting-started/features.md): what the framework provides
- [F´ Terminology](../tutorials-hello-world/docs/hello-world.md#f-terminology): short definitions of the core terms
- [F´ Translation Guide](reference/fprime-translations.md): common software concepts and their F´ equivalents

### Chapter 2: Installing F´ and Your First Component

The fastest way to learn is to build something. The Hello World tutorial walks through installing the tools, creating a
project, writing a small component, adding it to a deployment, and sending it a command from the GDS. Don't worry about
understanding every step; the next chapters explain each piece.

- [Installing F´](getting-started/installing-fprime.md): installation and troubleshooting
- [Hello World Tutorial](../tutorials-hello-world/docs/hello-world.md)

---

## Part II: The Building Blocks

### Chapter 3: Ports, Components, and Topologies

These are the three constructs every F´ system is made of. Ports define typed interfaces; components implement
behavior behind those interfaces; topologies instantiate components and connect their ports. Components come in three
kinds (passive, queued, active) and ports can be synchronous or asynchronous. That choice determines which thread does
the work, and it is one of the most important design decisions you will make.

- [Core Constructs: Ports, Components, and Topologies](user-manual/overview/03-port-comp-top.md)
- [Selecting Component, Port, and Command Kinds](user-manual/framework/component-and-port-selection.md)

### Chapter 4: Commands, Events, Telemetry, Parameters, and Types

Flight software is operated from the ground: **commands** go up, **events** (log messages) and **telemetry channels**
(periodic measurements) come down, and **parameters** are persisted settings that can be updated in flight. F´ supports
all four directly in the component model and handles the routing for you. The data they carry is described with F´
types: fixed-width primitives plus enums, arrays, and structs defined in FPP.

- [Data Constructs: Commands, Events, Channels, and Parameters](user-manual/overview/04-cmd-evt-chn-prm.md)
- [Data Structures and Types](user-manual/overview/05-enum-arr-ser.md)
- [F´ Numerical Types](reference/numerical-types.md)

### Chapter 5: Modeling with FPP

FPP (F Prime Prime) is the language used to describe types, ports, components, and topologies. The FPP compiler
generates the C++ base classes, the ground dictionary, and the topology setup code, so you only write the component
behavior. The Math Component tutorial is a practical introduction: it defines custom types and ports, builds two
components that talk to each other, adds telemetry and error handling, and writes unit tests. Afterwards, use the
User's Guide to learn the language in depth and keep the spec for reference.

- [Math Component Tutorial](../tutorials-math-component/docs/math-component.md)
- [FPP User's Guide](https://nasa.github.io/fpp/fpp-users-guide.html): after the tutorial, read sections 6–14
  ([Defining Types](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Types) through
  [Defining Topologies](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Topologies))
- [FPP Language Specification](https://nasa.github.io/fpp/fpp-spec.html) *(reference)*
- [Autocoded Functions and Component Classes](user-manual/framework/autocoded-functions.md): what the generated C++ gives you

### Chapter 6: Projects, Deployments, and the Development Process

You have now built components; this chapter zooms out. A **project** holds your code and configuration, and a
**deployment** is one executable built from a topology. F´ has a recommended development process: requirements, then
FPP design, implementation, unit testing, topology integration, and integration testing. The LED Blinker tutorial
follows that process end to end on a more realistic component. For now, do Steps 1–7 and 9 on your development
machine; Step 8 (running on hardware) is covered in Chapter 12.

- [Projects and Deployments](user-manual/overview/proj-dep.md)
- [F´ Development Process](user-manual/overview/development-practice.md)
- [LED Blinker Tutorial](../tutorials-led-blinker/docs/led-blinker.md)

---

## Part III: Building a Complete System

### Chapter 7: The Build System

F´ builds with CMake, plus a thin tool (`fprime-util`) that drives it. You don't need to be a CMake expert: most
modules only call a single `register_fprime_*` function in their `CMakeLists.txt`. Learn how modules are registered
and how `settings.ini` configures a project. The cheatsheet lists the `fprime-util` commands you will use every day,
and `fprime-util --help` lists the rest. Toolchains and platforms come later (Chapter 13).

- [F´ CMake Build System](user-manual/build-system/01-cmake-intro.md): start here
- [F´ Cheatsheet](https://fprime.jpl.nasa.gov/cheatsheet): `fprime-util` commands and the development workflow
- [`settings.ini`: Build Settings Configuration](user-manual/build-system/settings.md)
- [CMake API Reference](user-manual/build-system/cmake-api.md) *(reference)*, [Targets](user-manual/build-system/cmake-targets.md) *(reference)*

### Chapter 8: Topologies and Subtopologies

A real deployment has dozens of component instances. F´ ships **subtopologies**, pre-wired groups of standard
components, for command and data handling (CdhCore), communications (ComFprime / ComCcsds), and file handling.
Most projects import these and connect their own components to them. Learn how instances and topologies are
defined in FPP, then how to use and create subtopologies.

- FPP User's Guide: [Defining Component Instances](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Component-Instances) and [Defining Topologies](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Topologies)
- [Subtopologies](user-manual/design-patterns/subtopologies.md)
- [CDH Core Subtopology](reference/system-functional/subtopology-cdh-core.md), [ComFprime Subtopology](reference/system-functional/subtopology-com-fprime.md), [File Handling Subtopology](reference/system-functional/subtopology-file-handling.md)
- [Develop a Subtopology](how-to/develop/develop-subtopologies.md)

### Chapter 9: Scheduling, Threads, and Design Patterns

Flight software must run on time. F´ uses **rate groups** to call components periodically at fixed rates, and active
components to move work onto their own threads. These building blocks combine into a few well-known design patterns;
knowing them will make both the standard components and other projects much easier to read.

- [Rate Groups and Timeliness](user-manual/design-patterns/rate-group.md)
- [Rate Group Scheduling Functionality](reference/system-functional/rate-group-scheduling.md)
- [LED Blinker extension: Timeliness and Deadline-Driven Components](../tutorials-led-blinker/docs/timeliness.md)
- [Common Port Design Patterns](user-manual/design-patterns/common-port-patterns.md)
- [The Manager/Worker Pattern](user-manual/design-patterns/manager-worker.md)
- [Asserts in F´](user-manual/framework/assert.md)

### Chapter 10: The Ground Data System

The GDS is how you interact with running software during development and test: it sends commands, displays events and
telemetry, transfers files, and runs sequences. It works from a browser or from the command line. The dictionary
generated from your FPP model is what connects the two sides.

- [The F´ Ground Data System](user-manual/overview/gds-introduction.md)
- [The F´ GDS CLI](user-manual/gds/gds-cli.md)
- [The GDS Dashboard](user-manual/gds/gds-custom-dashboards.md)
- [Sequencing in F´](user-manual/gds/seqgen.md): command sequences with CmdSequencer
- [FpySequencer](../Svc/FpySequencer/docs/sdd.md) and [Advanced Sequencing with Rust](user-manual/gds/wasm-rust.md): sequencers
  that run programs (with branching, telemetry checks, and waits) rather than fixed command lists

### Chapter 11: Testing

F´ generates a test harness for every component, so unit tests can send commands, invoke ports, and check events and
telemetry without a running deployment. Rule-based testing extends this to randomized sequences of actions. Integration
tests then drive a full running deployment through the GDS from Python (pytest). You have already written some unit
tests in the tutorials; this chapter covers the full toolset.

- [Unit Testing in F´](user-manual/overview/unit-testing.md)
- [Test-Driven Development in F´](how-to/test/test-driven-development.md)
- [Write Rule-Based Tests](how-to/test/rule-based-testing.md)
- [LED Blinker Step 9: System Testing](../tutorials-led-blinker/docs/led-blinker.md#9-system-testing): a first integration test
- [GDS Integration Test API](user-manual/gds/gds-test-api-guide.md)
- [Reusable Integration Tests](user-manual/gds/reusable-integration-tests.md)

At this point you can design, build, test, and operate an F´ deployment on your development machine. Part IV covers
specialized topics.

---

## Part IV: Specialized Topics

### Chapter 12: Running on Hardware

*For: anyone deploying to an embedded target.*

Running on a target means cross-compiling with the right toolchain and talking to hardware through driver components.
F´ separates hardware access from application logic with the Application-Manager-Driver pattern, so most of your code
stays portable and testable. Finish the hardware sections of the LED Blinker tutorial, then learn how drivers are
structured. The two tutorials take different routes: LED Blinker runs on embedded Linux (such as a Raspberry Pi),
while Arduino LED Blinker runs on a microcontroller without a full OS.

- [Cross-Compilation Setup Tutorial](tutorials/cross-compilation.md)
- [LED Blinker Step 8: Running on Hardware](../tutorials-led-blinker/docs/led-blinker.md#8-led-blinker-running-on-hardware) (embedded Linux) or [Arduino LED Blinker](../tutorials-arduino-led-blinker/docs/arduino-led-blinker.md) (microcontroller)
- [Supported Platforms](user-manual/framework/supported-platforms.md): existing platforms and reference projects
- [Application-Manager-Driver Architecture](user-manual/design-patterns/app-man-drv.md)
- [ISR Device Driver Pattern](user-manual/design-patterns/isr-driver.md)
- [Develop a Device Driver](how-to/develop/develop-device-driver.md)
- [Hardware Driver Functionality](reference/system-functional/hardware-drivers.md)

### Chapter 13: Porting to a New Platform

*For: anyone bringing F´ to an OS, RTOS, or board that isn't supported yet.*

Porting F´ to a new platform involves four pieces: CMake toolchain and platform files (the compiler, build settings,
and which implementations to use), platform types and configuration, drivers for the hardware, and an implementation of
the OS Abstraction Layer (tasks, mutexes, files, and so on). F´ can also run without an OS.

- [Porting to New Platforms](how-to/integrate/porting-guide.md): start here
- [CMake Toolchain Files](user-manual/build-system/cmake-toolchains.md), [F´ and CMake Platforms](user-manual/build-system/cmake-platforms.md), [CMake Implementations](user-manual/build-system/cmake-implementations.md)
- [Operating System Abstraction Layer (OSAL)](reference/system-functional/osal.md), [Implement an OS Abstraction Layer](how-to/integrate/implement-osal.md)
- [F´ on Baremetal Systems](user-manual/framework/run-baremetal.md), [F´ on Multi-Core Systems](user-manual/framework/run-multi-core.md)

### Chapter 14: Communications

*For: anyone connecting F´ to a radio, a different ground system, or a custom protocol.*

Between the flight software and the ground sits a communications stack: framing and deframing, routing, queueing, and
a driver for the physical link. F´ ships with an F´ protocol and a CCSDS protocol, and every layer can be replaced.

- [Ground Interface Architecture and Customization](user-manual/framework/ground-interface.md)
- [Communication Stack Functionality](reference/system-functional/communication.md), [CCSDS Protocol Functionality](reference/system-functional/ccsds-protocol.md), [ComCcsds Subtopology](reference/system-functional/subtopology-com-ccsds.md)
- [Communication Adapter Interface](reference/communication-adapter-interface.md)
- [Implement a Framing Protocol](how-to/integrate/custom-framing.md), [Implement a Radio Manager Component](how-to/develop/implement-radio-manager.md), [Add Custom Uplink and Downlink Data Types](how-to/develop/custom-uplink-downlink-data.md)
- [The Hub Pattern](user-manual/design-patterns/hub-pattern.md): connecting multiple F´ deployments

### Chapter 15: Flight Services

*For: anyone preparing a deployment for real operations.*

The standard components cover command dispatch, event and telemetry management, parameters, time, file transfer,
sequencing, and health monitoring. Most missions use them as-is. Understanding what they do (and their requirements)
tells you what you get for free and what you still need to build.

- [System Functional Documentation](reference/system-functional/index.md): index of all capabilities, start here
- [Command Dispatch](reference/system-functional/command-dispatch.md), [Event Management](reference/system-functional/event-management.md), [Telemetry Channel Storage](reference/system-functional/telemetry-chan.md), [Telemetry Packetizer](reference/system-functional/telemetry-packetizer.md)
- [Parameter Management](reference/system-functional/parameters.md), [Load Parameters in Batch](how-to/operate/prm-write-how-to.md)
- [Time Services](reference/system-functional/time-services.md), [File Management](reference/system-functional/file-management.md), [System Services](reference/system-functional/system-services.md)
- [Sequencing](reference/system-functional/sequencing.md), [Advanced Sequencing](reference/system-functional/advanced-sequencing.md)
- [Health Monitoring](reference/system-functional/health-monitoring.md), [Health Checking Pattern](user-manual/design-patterns/health-checking.md)

### Chapter 16: Memory Management and Data Products

*For: anyone handling large data or needing strict memory control.*

Flight software typically forbids dynamic allocation after startup. F´ allocates memory at initialization and passes
large data around as buffers drawn from pre-allocated pools. Data products build on this to store and downlink bulk
science or engineering data, with prioritization.

- [Memory Management](user-manual/framework/memory-management/index.md), [Memory Allocation](user-manual/framework/memory-management/memory-allocation.md), [Buffer Pools](user-manual/framework/memory-management/buffer-pool.md)
- [Buffer Management Functionality](reference/system-functional/buffer-management.md)
- [Data Products](user-manual/framework/data-products.md), [Generate Data Products](how-to/develop/data-products.md), [Data Products Subtopology](reference/system-functional/subtopology-data-products.md)

### Chapter 17: Configuring and Extending F´

*For: anyone tuning F´ for their mission or reusing code across projects.*

Framework behavior (sizes, limits, feature flags) is set by configuration files that projects can override. Code can be
packaged as F´ libraries and shared between projects, and third-party libraries can be pulled into the build. FPP also
supports state machines, and components can be written in Python.

- [Configuring F´](user-manual/framework/configuring-fprime.md), [Configuration Modules](user-manual/build-system/configuration.md), [CMake Customization](user-manual/build-system/cmake-customization.md)
- [Develop an F´ Library](how-to/develop/develop-fprime-libraries.md), [Integrate a Third-Party Library](how-to/integrate/integrate-external-libraries.md)
- [State Machines](user-manual/framework/state-machines.md), [Define State Machines](how-to/develop/define-state-machines.md)
- [Develop Components in Python](how-to/develop/python-development.md)
- [Software Bill of Materials Generation](user-manual/security/software-bill-of-materials.md)

### Chapter 18: Extending the Ground Data System

*For: anyone customizing ground tooling or integrating another ground system.*

The GDS is extensible through plugins (communication, framing, data handling, apps). The JSON dictionary generated by
FPP is the interface for any other ground system.

- [Develop a GDS Plugin](how-to/operate/develop-gds-plugins.md): start here; [GDS Plugins Reference](reference/gds-plugins/index.md) *(reference)*
- [Create Ground-Derived Channels](how-to/operate/derive-channels-on-ground.md)
- [Dictionary Capabilities](reference/system-functional/dictionary.md), [FPP JSON Dictionary Specification](reference/fpp-json-dict.md) *(reference)*
- [GDS Developer's Guide](user-manual/gds/gds-dev-guide.md): internals of the GDS command-line tools, for maintainers

---

## Appendix: Where to Go Next

- [F´ Reference Projects](user-manual/framework/reference-projects.md): working examples of specific features and integrations
- [F´ Examples](https://github.com/nasa/fprime-examples): small, focused examples
- [F´ Community GitHub](https://github.com/fprime-community): tutorials, workshops, and platform support packages
- [Reference](reference/index.md): C++ and CMake APIs, component SDDs, specifications
- [F´ Course Materials](user-manual/overview/02-fprime-architecture.md): slide decks on F´ and flight software architecture
- [F´ Cheatsheet](https://fprime.jpl.nasa.gov/cheatsheet): the two-page summary, handy to print
- [GitHub Discussions](https://github.com/nasa/fprime/discussions): ask questions and get help
