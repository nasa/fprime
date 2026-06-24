---
hide:
  - toc
---

# User Manual

The User Manual dives into F Prime concepts and usage, providing a deep understanding of how the framework operates. The different chapters are listed below.

> [!TIP]
> **← Navigation pane**  
> Use the navigation pane on the left to explore the different chapters of the User Manual. If the navigation pane is not visible, click on the menu icon (three horizontal lines) at the top left corner of the page. The navigation pane is hidden on narrow screens or if zoomed in.

## Table of Contents

<details markdown="1">
<summary><strong>Build System</strong> — Learn about the F´ build system and how to customize it.</summary>

- [F´ CMake Build System](build-system/01-cmake-intro.md)
- [CMake API Reference](build-system/cmake-api.md)
- [CMake Customization](build-system/cmake-customization.md)
- [CMake Implementations](build-system/cmake-implementations.md)
- [F´ and CMake Platforms](build-system/cmake-platforms.md)
- [Targets](build-system/cmake-targets.md)
- [CMake Toolchain Files](build-system/cmake-toolchains.md)
- [CMake Build System Unit Tests](build-system/cmake-uts.md)
- [`settings.ini`: Build Settings Configuration](build-system/settings.md)

</details>

<details markdown="1">
<summary><strong>Design Patterns</strong> — Learn about common design patterns used in F´ applications.</summary>

- [Application-Manager-Driver Architecture](design-patterns/app-man-drv.md)
- [Common Port Design Patterns](design-patterns/common-port-patterns.md)
- [Health Checking Pattern](design-patterns/health-checking.md)
- [A Quick Look at the Hub Pattern](design-patterns/hub-pattern.md)
- [The Manager/Worker Pattern](design-patterns/manager-worker.md)
- [Rate Groups and Timeliness](design-patterns/rate-group.md)
- [Subtopologies](design-patterns/subtopologies.md)

</details>

<details markdown="1">
<summary><strong>Framework</strong> — Learn concepts and mechanisms needed to build and use an F´ application.</summary>

- [Asserts in F](framework/assert.md)
- [F´ Autocoded Functions and Component Classes](framework/autocoded-functions.md)
- [Constructing the F´ Topology](framework/building-topology.md)
- [Selecting Component, Port, and Command Kinds](framework/component-and-port-selection.md)
- [Configuring F´](framework/configuring-fprime.md)
- [Data Products](framework/data-products.md)
- [Ground Interface Architecture and Customization](framework/ground-interface.md)
- [Memory Management](framework/memory-management/index.md)
- [F´ on Baremetal Systems](framework/run-baremetal.md)
- [F´ on Multi-Core Systems](framework/run-multi-core.md)
- [State Machines](framework/state-machines.md)
- [Supported Platforms](framework/supported-platforms.md)

</details>

<details markdown="1">
<summary><strong>GDS</strong> — Learn how to use the F Prime Ground Data System, and how it can be used to test F´ applications.</summary>

- [The F´ GDS CLI](gds/gds-cli.md)
- [The GDS Dashboard](gds/gds-custom-dashboards.md)
- [GDS Dashboard Component Reference](gds/gds-dashboard-reference.md)
- [GDS Developer's Guide](gds/gds-dev-guide.md)
- [GDS Integration Test API](gds/gds-test-api-guide.md)
- [Reusable Integration Tests](gds/reusable-integration-tests.md)
- [Sequencing In F´](gds/seqgen.md)

</details>

<details markdown="1">
<summary><strong>Overview</strong> — Technical overview of the F´ ecosystem.</summary>

- [Introduction To F´](overview/01-full-intro.md)
- [F´ Software Architecture](overview/02-fprime-architecture.md)
- [Core Constructs: Ports, Components, and Topologies](overview/03-port-comp-top.md)
- [Data Constructs: Commands, Events, Channels, and Parameters](overview/04-cmd-evt-chn-prm.md)
- [Data Structures and Types](overview/05-enum-arr-ser.md)
- [F´ Development Process](overview/development-practice.md)
- [The F´ Ground Data System](overview/gds-introduction.md)
- [Projects and Deployments](overview/proj-dep.md)
- [A Tour of the Source Tree](overview/source-tree.md)
- [Unit Testing in F´](overview/unit-testing.md)

</details>

<details markdown="1">
<summary><strong>Security</strong> — Security considerations when designing and developing F´ applications.</summary>

- [Software Bill Of Materials Generation](security/software-bill-of-materials.md)

</details>

