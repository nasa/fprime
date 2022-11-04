# How To Integrate a New Communication Component

F´ projects typically communicate with a ground system or some other remote node. In order to perform this communication
these projects need to integrate with communications hardware like a radio. This guide will walk projects through the
development of a communications (i.e. radio) component.

## Who Should Follow This Guide?

Projects who wish to use the standard F´ uplink and downlink model seen [here](TODO: FILL me) but wish to use project
specific communications hardware. This guide will not cover framing nor communication protocols but specifically covers
the interface for communication components.

### Assumptions and Definitions

This guide makes the following assumptions about the user following the guide and the project setup that will deploy the
component.

1. User is familiar with standard F´ concepts
2. User understands the [application-manager-drive](TODO: FILL me) pattern
3. User is familiar with the F´ development process
4. User understands how to integrate components into an F´ topology
5. User is using `Svc::Framer`, `Svc::Drframer`, and `Svc::ComQueue` in the topology

This guide also uses the following terminology:

**Ground System:** system designed to command a remotely operating system.
**Uplink:** data coming into this deployment from some remote system (e.g. ground system)
**Downlink:** data going from this deployment to some remote system (e.g. ground system)
**Telemetry:** state data downlinked from the system. Also known as channels or telemetry channels.

## Example Hardware Description

In this guide, we will integrate an XBee 3 radio using its UART interface and XBee command-mode. Data can be sent
directly to the UART interface. When the radio itself needs to be configured, special UART sequences can be sent to the
radio to make it enter command-mode allowing it to accept and process AT commands.

## Step 1: Requirements

The first step to integrating a communication component is to understand the basic components of the device. For this
guide, the requirements will be kept minimal. Projects should list out requirements as needed. These requirements should
capture:

1. Ports and Interfaces
2. Events, telemetry, and commands
3. Component behavior

When developing a communications component whose purpose is to replace the `Svc::ComStub` component and work alongside
the `Svc::ComQueue` component, the component must meet the following requirements:

1. Sending `Svc::ComStatus::Ready` must be done once when communication is initially ready.
2. `Svc::ComStatus::Ready` may be sent once and only once for each send buffer received. If a `Svc::ComStatus::Fail` is
sent instead then only one `Svc::ComStatus::Ready` may follow up when communication is restored.
3. The communications component must include at-least the ports defined in [Svc::ComStub](TODO).

The example requirements are shown below. These are:

| Requirement ID | Description                                                                              |
|----------------|------------------------------------------------------------------------------------------|
| COMXBEE-1      | The component shall indicate when the radio has connected via an `ACTIVITY_LO` event     |
| COMXBEE-2      | The component shall produce telemetry for bytes sent and received upon a rate group call |
| COMXBEE-3      | The component shall send data via XBee 3's UART interface                                |
| COMXBEE-4      | The component shall integrate with `Svc::Framer`, `Svc::Deframer`, and `Svc::ComQueue`   |
| COMXBEE-5      | The component shall command XBee 3 via the AT command-mode supporting NI and ED commands |

Here, we capture several events, channels, and commands. In addition, we capture the F´ interface requirements for
interfacing with standard uplink and downlink components, and the driver interface requirements for speaking to
hardware.  We also capture specific component behavior around the AT command interface.

## Step 2: Component Design

The second step to integrating the communication component is composing a component design. This section will walk
through each of the design decisions. Projects should implement design decisions to support the above requirements.

The final FPP model for the XBee 3 radio component can be found [here](). 

### Base Component Design

First, a project should choose the basic component properties. Namely, the component must be `active`, `passive` or
`queued`.  Most projects will select `active` for communications components as managing hardware often requires a
separate thread to handle effectively. The `Svc::ComStub` component is `passive` so choosing an `active` component is
by no means required.

For the XBee example, the AT command mode needs specifically timed commands on a quiescent UART bus to engage.  Thus,
this component needs to be an `active` component in order to control this specific timing without added complexity.

### Events Design

Component implementors are free to define whatever events are needed for their project. Typically, communications
components emit a connected event to indicate when uplink and downlink flows begin.

In this example, the communications component will define one event:

| Event     | Severity    | Description                   |
|-----------|-------------|-------------------------------|
| Connected | ACTIVITY_LO | Indicates radio has connected |

### Telemetry Design

Component implementors are free to define whatever channels are needed for their project. Typically, communications
components emit all sorts of hardware information on the radio. Bytes sent and received give indication as to how much
data is passing through the interface. Device state, operating mode, and link quality data can be useful to determine
the cause of any missed packets. Device temperature, voltage, and current can help understand general device health.
Projects may consider all of these channels where the data is available.

In this example, the communications component will define two telemetry channels:

| Telemetry Channel | Type | Description                         |
|-------------------|------|-------------------------------------|
| BytesSent         | U32  | Count of bytes sent to downlink     |
| BytesReceived     | U32  | Count of bytes received from uplink |

### Commands Design

TODO: defined commands


### Port Design

Primarily the communication component will implement the interface described in the
[`Svc::ComStub` SDD](../UsersGuide/...todo) because the component will replace `Svc::ComStub` in a projects' topology.
In addition, most communication components build on top of an underlying driver. Rate group and standard ports are also
often needed.

The example component uses [`Drv::LinuxUartDriver`](), which implements the [`Drv::ByteStreamDriverModel`]() interface.
It also included the `Svc::ComStub` ports, a rate group port, and standard F´ ports for events, channels, and telemetry.
Thus, our radio component is required to have the following ports:

One specific requirement of the port design is that 


| Kind          | Name           | Port Type             | Usage                                                       |
|---------------|----------------|-----------------------|-------------------------------------------------------------|
| `async input` | `comDataIn`    | `Drv.ByteStreamSend`  | Port receiving `Fw::Buffer` from `Svc::Framer` for downlink |
| `output`      | `comStatus`    | `Svc.ComStatus`       | Port informing `Svc::ComQueue` status of the last downlink  |
| `output`      | `comDataOut`   | `Drv.ByteStreamRecv`  | Port providing `Fw::Buffer` to `Svc::Deframer` for uplink   |
| `async input` | `drvConnected` | `Drv.ByteStreamReady` | Port indicating UART driver hardware is ready               |
| `async input` | `drvDataIn`    | `Drv.ByteStreamRecv`  | Port receiving `Fw::Buffer` from UART driver for uplink     |
| `output`      | `drvDataOut`   | `Drv.ByteStreamSend`  | Port providing `Fw::Buffer` to UART driver for downlink     |
| `async input` | `run`          | `Svc.Sched`           | Port called by rate group for emitting telemetry channels   |

**Note:** standard event, telemetry, and command ports are not listed above.

## Implementation and Testing

Projects will need to implement the port handlers and implementation for their communication component on their own.
Specific implementations will diverge based on hardware and design choices.

In order to help in this process, the example component implementation is available for [reference]().

## Topology Integration

Once the design and implementation is done, the component can be added to a projects' topology. This component will
replace the following two components in the `Ref` topology:

1. `Svc::ComStub`: replaced directly with implemented communication component
2. `Drv::TcpClient`: replaced with any driver supporting the communication component or removed

Project may attach additional support components as needed.

## Conclusion

In this guide, we have covered the design of new communications component and seen how to integrate it into the
topology. At this point, projects should be up and running with whatever communications hardware they deploy.
