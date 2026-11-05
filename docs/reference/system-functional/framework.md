
# Framework Functionality

## References

- [Fw::Types SDD](https://github.com/nasa/fprime/blob/devel/Fw/Types/docs/sdd.md)
- [Fw::Obj SDD](https://github.com/nasa/fprime/blob/devel/Fw/Obj/docs/sdd.md)
- [Fw::Port SDD](https://github.com/nasa/fprime/blob/devel/Fw/Port/docs/sdd.md)
- [Fw::Cmd SDD](https://github.com/nasa/fprime/blob/devel/Fw/Cmd/docs/sdd.md)
- [Fw::Tlm SDD](https://github.com/nasa/fprime/blob/devel/Fw/Tlm/docs/sdd.md)
- [Fw::Log SDD](https://github.com/nasa/fprime/blob/devel/Fw/Log/docs/sdd.md)
- [Fw::Prm SDD](https://github.com/nasa/fprime/blob/devel/Fw/Prm/docs/sdd.md)
- [Fw::Time SDD](https://github.com/nasa/fprime/blob/devel/Fw/Time/docs/sdd.md)
- [Fw::Buffer SDD](https://github.com/nasa/fprime/blob/devel/Fw/Buffer/docs/sdd.md)
- [Fw::Com SDD](https://github.com/nasa/fprime/blob/devel/Fw/Com/docs/sdd.md)
- [Fw::Dp SDD](https://github.com/nasa/fprime/blob/devel/Fw/Dp/docs/sdd.md)
- [Fw::FilePacket SDD](https://github.com/nasa/fprime/blob/devel/Fw/FilePacket/docs/sdd.md)
- [Fw::DataStructures SDD](https://github.com/nasa/fprime/blob/devel/Fw/DataStructures/docs/sdd.md)
- [FPP User Guide](https://nasa.github.io/fpp/fpp-users-guide.html)

## Overview

The F Prime Framework (Fw) provides the foundational types, base classes, port interfaces, and serialization infrastructure upon which all F Prime components are built. It defines the core abstractions of the component architecture — objects, ports, components, commands, events, telemetry, parameters, time, buffers, and data products — along with the serialization mechanisms that allow these abstractions to communicate across port boundaries.

### Objects and Components

All objects in F Prime derive from a common base class that provides identity (name) and optional registration with an object registry. Components are the primary unit of software in F Prime and come in three kinds:

- **Active Components** — Have their own thread and a message queue. Incoming port calls are placed on the queue and processed asynchronously.
- **Passive Components** — Execute in the caller's thread context. Port calls are handled immediately and synchronously.
- **Queued Components** — Have a message queue but do not own a thread. They process queued messages when dispatched by an external caller.

Components interact exclusively through ports. The framework provides base classes that the autocoder uses to generate component infrastructure from FPP models.

### Ports

Ports define typed interfaces for inter-component communication. Each port specifies a set of arguments and an optional return type. The framework provides base classes for input and output ports. At system startup, output ports are connected to input ports as defined by the topology. Port calls are the only mechanism for inter-component communication in F Prime.

### Serialization

All data passed between components is serialized into byte buffers. The framework provides a serialization infrastructure that supports:

- Primitive types (integers, floats, booleans)
- Strings
- Enumerations
- Arrays and structures
- Polymorphic types (PolyType) that can hold any primitive type

Serialization enables data to be passed across port boundaries and transmitted to or from a ground system.

### Commands

The command infrastructure provides the mechanism for external systems (ground or sequencers) to invoke operations on components. Commands are defined in FPP models and consist of:

- An opcode that uniquely identifies the command
- An optional argument list
- A kind (async, sync, or guarded) that determines dispatch behavior

When a command is received, it is deserialized from a command buffer, dispatched to the implementing component, and a completion status is returned to the command source. See [Command Dispatch](command-dispatch.md) for details on command routing and status tracking.

### Events

Events are time-tagged log entries that record system activities and anomalies. Each event has:

- A unique identifier
- A severity level (FATAL, WARNING_HI, WARNING_LO, COMMAND, ACTIVITY_HI, ACTIVITY_LO, DIAGNOSTIC)
- A format string for human-readable display
- Optional arguments that provide context

Events are serialized and sent to event management services for distribution and downlink. A separate text event port provides pre-formatted human-readable strings for console logging. See [Event Management](event-management.md) for details on event collection and handling.

### Telemetry

Telemetry channels provide a mechanism for components to publish named data values representing system state. Each telemetry channel has:

- A unique identifier
- A typed value
- A time tag indicating when the value was written

Telemetry values are serialized and sent to telemetry storage services, which collect them into packets for downlink. Telemetry packets can hold multiple channel values and are formatted with channel IDs and timestamps. See [Telemetry (Channel-based)](telemetry-chan.md) and [Telemetry (Packetized)](telemetry-packetizer.md) for storage and downlink options.

### Parameters

Parameters are named configuration values stored in non-volatile memory that can be read at startup and updated during operation. The framework provides ports for:

- **Getting** a parameter value from storage, with status indicating validity (valid, invalid, default, or uninitialized)
- **Setting** a parameter value in storage

Parameters are defined in FPP models and managed by a parameter database service. See [Parameters](parameters.md) for details.

### Time

The framework provides a time representation that includes a time base (project-specific), a time context, seconds, and microseconds. Time values are used throughout the system to tag events and telemetry. A separate time interval type represents durations rather than absolute timestamps. Time source components implement the time port to provide the current system time to all components that need it. See [Time Services](time-services.md) for available implementations.

### Buffers

The framework provides a buffer type that wraps a pointer to allocated memory along with its size and an origin-tracking context value. Buffers avoid copying data by passing references to allocated memory between components. The framework defines ports for requesting buffers from a buffer manager and for sending buffers between components. Buffers must be checked for validity before use, as allocation may fail. See [Buffer Management](buffer-management.md) for allocation strategies.

### Communication Buffers

Communication buffers provide the serialization containers for data that is transmitted between the flight software and external systems. The framework defines a base packet class with type identification and a communication buffer class for holding serialized packet data. Command packets, telemetry packets, and other data types extend this base for their specific needs. See [Communication Stack](communication.md) for the data path between flight software and external systems.

### Data Products

Data products are structured file data intended for downlink. The framework defines:

- **Containers** — Named, prioritized wrappers that hold data product records. Each container has an ID, priority, time tag, processing type flags, user-configurable header data, and a state.
- **Records** — Individual data items within a container, either single-value or array-type.

The serialized container format includes a header, a header hash for integrity, data records, and a data hash. Ports are provided for requesting, getting, sending, and responding to data product buffer operations. See [Data Products](data-products.md) for the full lifecycle.

### File Packets

File packets provide the serialization format for transferring files between the flight software and ground system. This supports both uplink (ground to flight) and downlink (flight to ground) file transfers. See [File Management](file-management.md) for file transfer operations.

### Data Structures

The framework provides reusable data structure implementations including maps and other collections used internally by framework services and components.

### State Machines

The framework includes support for hierarchical state machines that can be attached to components. State machines are defined in FPP models and auto-generated into C++ implementations. They provide a structured way to manage complex component behavior with well-defined states, transitions, and actions.
