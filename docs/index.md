---
title: F' - A Component-Base Flight Software Framework
layout: default
---
# F'- A component-based Flight Software Framework

F' is a component-driven framework used to quickly develop and deploy flight and embedded software applications. Originally developed at the Jet Propulsion Laboratory this framework has been successfully deployed on several space applications.


### Component Driven

F' is composed of components and ports used to interact between components. This enables users to quickly develop
individual components and ports. These individual components can then be composed together into full a full system architecture called a Topology. This Topology is compiled and runs as the system.

### Reusable Standard Components

F' contains a number of standard components that can be reused to supply generic applications of embedded and
flight applications. These components include functions to downlink Telemetry values, send out log-events, and
dispatch commands. F' also supports driving components at set rates allowing them to carry out actions on
a set schedule.

### Auto-Generated Code

F' uses code generators to create the standard glue code used to connect components, ports, and Topologies. This
saves the user time freeing them to implement the custom actions responding to port calls.
