# Getting Started: Introduction and F´ Terminology

The getting started tutorial is designed to teach new users the basics of F´ usage, instruct existing users on new
command that help in F´ development, and act as the canonical "Hello World" example for F´.

This tutorial walks through the following steps:
1. [Creating an F´ Project](./NewProject.md)
2. [Creating an F´ Hello World Component](./HelloWorld.md)
3. [Integration and Testing With F´ Deployments](./Deployments.md)

Once finished, users should have a good understanding of the basic development mechanics for working with F´ and then
could dive deeper into concepts through the [Math Component Tutorial](../MathComponent/Tutorial.md).

## F´ Terminology

F´ uses specific terminology to refer to specific parts of the system. This section dives into the basic F´ terminology
used in this tutorial and an explanation of how the terminology is used.

#### Project

An F´ project is a collection of files and folders used to work with F´. At its core, a project is just a folder that
can be used to hold F´ code. Projects should specifically exclude the core F´ library to avoid the "clone and own"
problem. Rather projects should link-to a version of F´.

This tutorial will create a new `MyProject` project used to contain the other things created by the tutorial.

#### Component

An F´ component encapsulates a unit of system behavior. Components use ports to communicate with other components and
define commands, events, telemetry channels, and parameters.

This tutorial will create a new `HelloWorld` component that defines a `SAY_HELLO` command, `Hello` event, and
`GreetingCount` telemetry channel. The component only used built-in ports. It does not use custom ports nor parameters.

#### Port

A port is an interface used to communicate between components. This tutorial only uses built-in ports.

#### Command

Commands represent actions that a component can execute. Commands can be sent from the ground or via command sequences.
Command behavior is defined in a command handler defined in the component's implementation.

This tutorial defines one command `SAY_HELLO` with a single argument `greeting`. This command will be sent via the
ground system and will echo the greeting back via the `Hello` event.

#### Event

Events represent actions that a component has performed. Events are akin to software log messages. Events are received
and displayed by the ground system.

This tutorial defines one event `Hello` emitted in response to the `SAY_HELLO` command and containing the same greeting
that the command had.

#### Telemetry Channel

Telemetry channels provide the active state of the component. Each channel represents a single item of state that will
be sent to the ground system.

This tutorial defines a single channel `GreetingCount` that contains the count of the number of `SAY_HELLO` greeting
commands received.

#### Deployment

Deployments are a single build of F´ software. Each deployment results in one software executable that can be run along
with other associated files. Each deployment has a topology that defines the system.

This tutorial will create the `MyDeployment` deployment, run this deployment, and test it through the F´ GDS.

#### Topology

Topologies are networks of connected components that define the software. Multiple instances of a component may be added
to the topology.

This tutorial will use a standard command and data handling topology. A single `HelloWorld` component instance called
`helloWorld` will be added to the standard topology.

**Next:** [Getting Started: Creating an F´ Project](./NewProject.md)
