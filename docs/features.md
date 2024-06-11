---
layout: default
title: "F´ Features"
---

F´ (or F Prime) has the following key features that enable robust embedded system design. These features enable projects to quickly
develop embedded systems applications from design through test.

### Reusability

F´'s component-based architecture enables a high degree of modularity and software reuse. This means that components
written for one application may be reused in other applications seamlessly.

### Rapid Deployment

F´ provides a complete development ecosystem, including modeling tools, testing tools, and a ground data system.
Developers use the modeling tools to write high-level specifications, automatically generate implementations in C++,
and fill in the implementations with domain-specific code. The framework and the code generators provide all the
boilerplate code required in an F´ deployment, including code for thread management, code for communication between
components, and code for handling commands, telemetry, and parameters. The testing tools and the ground data system
simplify software testing, both on workstations and flight hardware in the lab.

### Portability

F´ runs on a wide range of processors, from microcontrollers to multi-core computers. It runs on several
operating systems (or no operating system at all). Porting F´ to new operating systems and architectures is
straightforward enabling use with just about any embedded system.

### High Performance

F´ utilizes a point-to-point architecture. The architecture minimizes the use of computational resources and is well
suited for smaller processors.

### Adaptability

F´is tailored to the level of complexity required for small missions. This makes it accessible and easy to use, while
still supporting a wide variety of missions.

### Analyzability

The typed port connections provide strong compile-time guarantees of correctness.

### Standard Component Library

F´ ships with a good number of flight-proven components ready for use. These include most standard functions of an
embedded system such as commanding, telemetry capture, system event capture, and ground interaction. Often this
means that most of an application is completed.  Developers often only need to develop components for their
specific use-case and standard components are inherited from the framework itself.

### Standard Tooling

F´ provides several standard tools to ease development:

1. Autocoder for stamping out boilerplate code
2. CMake-based build system for building cross-platform binaries
3. Development ground system enabling developers to quickly test with real hardware
4. Unit Test framework to enable quick generation of unit tests tailored to F´ code
5. Integration test framework to test F´ at the integrated system level
