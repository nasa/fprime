# A More Complete Introduction To F´

This document will reintroduce F´ giving more context and detail for users who desire the "full story", or at least a
more complete "story". This document contains the following sections:

- [The Origins of F´](#the-origins-of-f)
- [A (Brief) Introduction to Space Systems](#a-brief-introduction-to-space-systems)
- [The Organization of F´ Deployments](#the-organization-of-f-deployments)
- [Conclusion](#conclusion)


## The Origins of F´

F´ is an embedded systems framework developed build to meet certain goals expanded upon below.
These goals it particularly well suited for small embedded missions (and projects) that need
reliable components and infrastructure provided by the framework in
order to minimize development cost, schedule, or effort. These missions
include CubeSats, small-sats, and deployables. Despite being built for NASA missions, F´ can be used for any
embedded system regardless of the project size or field.


F´ was built to meet the following goals:

  - Capture the reusability of embedded projects into a Framework
  - Ease the separating and reassembling of shareable system components
  - Isolate components for ease of testing
  - Adapt easily to new contexts
  - Port to new architectures and platforms
  - Be easy to use
  - Scale and configure to meet new use cases
  - Perform well in resource-constrained context

F´ was developed at NASA's Jet Propulsion Laboratory for small-scale embedded flight software. These small missions
desperately needed a Flight-Quality framework to expedite development while reducing cost.

## A (Brief) Introduction to Space Systems

Although F´ can be used to model any embedded system, it has its origin in the stars. It was built at NASA's Jet
Propulsion Laboratory for use in small-scale space flight missions and thus to understand the basics of these systems
is to more completely understand F´ itself.

What is a space system? For the purposes of this guide, it is any embedded system in space. Typically these systems
compose the complete control and operation of a spacecraft to accomplish some mission.  At JPL these systems are
typically conducting some aspect of scientific research.

Since F´ is usually the complete software for one of these systems, F´ decomposes its projects into discrete
**Components** that each manage one part of the system. e.g. a Radio Component may control the radio hardware to
facilitate communication. **Components** are connected to one another via **Ports**. **Ports** allow communication
between **Components**.  The complete graph or network of **Components** connected via **Ports** is called a
**Topology**, which encapsulates the full system.

F´ was built to support **Command and Data Handling** (C&DH) of these space systems as well as instruments running as
part of these space systems. This means F´ out-of-the-box is designed to handle commands sent from the ground, and
respond with telemetry to the ground. In F´ this telemetry is broken into **Events** representing the history of actions
taken by the system, and channels representing the current state of the system broken into named channels that each
contain a portion of the state. e.g. an **Event** might be "Established Communications" and a **Channel** might be
"Current Temperature: 3C".

All projects using F´ are composed of **Components**, **Ports**, and **Topologies**. Although F´ does not require the
the user to control the system using **Commands**, **Events**, and **Channels**, these constructs represent the typical
use case for F´projects and such constructs are built-in. Thus helpful to understand both **Components**, **Ports**, and
**Topologies**, as well as **Command**, **Events**, and **Channels** in order to fully understand the power of the F´
framework.

## The Organization of F´ Deployments

The core F′ software framework allows projects to be decomposed into a set of **Components** that are interconnected by
**Ports**. Each **Component** represents one discrete piece of the system. e.g. the Command Dispatcher is a framework
component used to dispatch incoming commands to be handled by another component in the system. Its job is to receive a
ground communication and translate that into an action, dispatch the action to another component, and await the
completion of this action. It emits **Events** to signify when the action is dispatched, and when it has been completed. It
has **Channels** counting the number of commands dispatched.

From this example, we can see the organization of an F´ project. **Components** are the key to system
modularization and each component has a set of **Ports** to communicate from. In addition, each **Component** may
define a set of **Commands** it may handle (these are the actions seen dispatched above), and each **Component** may
define a set of **Events** it can report as well as a set of **Channels** that it will send out. When a system is built
from these modules, the functionality is distributed amongst the **Components** and the **Topology** sets up the
communication such that the system can function.

## Threads, Multi-Core Architectures and F´

F´ was built for use on platforms running an Operating System (OS) and executing on a single core. Notably, these systems
come with a thread scheduler. That being said it is entirely possible to use F´ on a baremetal system, or a multi-core
system, however; some care should be taken when designing for such systems an understanding of execution context is
required.  See: [F´ On Baremetal and Multi-Core Systems](../dev/baremetal-multicore.md)

## Conclusion

The F′ software framework is released as open source and has been ported to Linux, MacOS, Windows (WSL), VxWorks, ARINC
 653, RTEMS, Baremetal(No OS), PPC, Leon3, x86, ARM (A15/A7), and MSP430. Mature sets of CD&H components are available
following flight process, such as code inspections, static analysis, and full-coverage unit testing.