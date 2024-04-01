# Constructing the F´ Topology

**Note:** For a hands-on walk-through of build topologies, please see: [Tutorials](../../Tutorials/README.md)

The executing software consists of a set of interconnected components
executing on the threads of the active components or driven by other
events in the system such as hardware interrupts or timing sources. This
section will describe the steps necessary to get the software up and
running.

This guide discusses:

-[Instantiating the Components](#instantiating-the-components)
-[Initializing the Components](#initializing-the-components)
-[Interconnecting the Components](#interconnecting-the-components)
-[Registering Commands](#registering-commands)
-[Loading Parameters](#loading-parameters)
-[Starting Active Components](#starting-active-components)


## Instantiating the Components

The constructors in the component base classes have been designed so
that the components can be instantiated using whatever memory model the
developer wishes. They can be created statically, on the heap, or on the
stack. As described in Section 6.7.8.1, the constructor has either a
name argument or none at all. The developer’s derived class constructors
may have extra arguments that are particular to that application. If
classes are declared statically, the developer should keep in mind
uncertainties about execution order.

## Initializing the Components

As discussed in Section 6.8.2, each component has an init() call that
initializes the component base classes. This call should be made after
instantiating the components. For queued and active components, the
queue size is passed. The queue should be sized based on an
understanding of task priorities and message traffic between the
components.

## Interconnecting the Components

The components in the software are interconnected by connecting the
ports of the components together. Ports are connected by passing
pointers to input ports to the output ports that are calling them.
Methods are generated in the component base classes to get input port
pointers and pass them to output ports. The following sections describe
the connections of different port types.

### Interface Ports

Interface ports are the regular ports that are used to connect
components together. For each port type and name on a component, the
method naming scheme is as follows:

Get input port pointer:

> \<PortType\>\* get\_\<port name\>\_InputPort(NATIVE\_INT\_TYPE
> portNum);

where

> \<PortType\> = The full port type specified in the data\_type
> attribute in the definition of the port in the component XML.
>
> \<port name\> = The name of the port in the name attribute of the port
> definition.

The portNum argument to the method should be set to zero if there is
only one instance of the port.

Set output port pointer:

The value of the input pointer retrieved via the method in the last
section is given to an output port of the same type by calling:

> void set\_\<port name\>\_OutputPort(NATIVE\_INT\_TYPE portNum,
> \<PortType\>\*port);

where

> \<PortType\> = The full port type specified in the data\_type
> attribute in the definition of the port in the component XML.
>
> \<port name\> = The name of the port in the name attribute of the port
> definition.

The portNum argument to the method should be set to zero if there is
only one instance of the port.

There is a second overloaded version of the method to set an output port
when the input port being passed to it is a serialized port:

> void set\_\<port name\>\_OutputPort(NATIVE\_INT\_TYPE portNum,
> Fw::InputSerializePort \*port);

### Command Ports

As discussed in Section 6.6.4, the code generator will create the
correct set of command-related ports for a component that has commands
defined. For that component, the functions used to get or set
command-related port pointers have standard names. The names are as
follows:

Get command input port:

> Fw::InputFwCmdPort\* get\_CmdDisp\_InputPort();

Set command status port:

> void set\_CmdStatus\_OutputPort(Fw::InputCmdResponse\_Port\* port);

Set command registration port:

> void set\_CmdReg\_OutputPort(Fw::InputCmdRegPort\* port);

For the component(s) that are connected to those ports, they would use
the normal methods for accessing the port pointers as described in the
last section.

### Telemetry Ports

The standard port accessor functions for telemetry are as follows:

Set telemetry output ports:

> void set\_Tlm\_OutputPort(Fw::InputTlmPort\* port);

Set time output ports:

> void set\_Time\_OutputPort(Fw::InputTimePort\* port);

### Event Logging Ports

The standard port accessor functions for logging events are as follows:

Set logging output ports:

> void set\_Log\_OutputPort(Fw::InputLogPort\* port);
>
> void set\_TextLog\_OutputPort(Fw::InputFwLogTextPort\* port);

Set time output ports:

> void set\_Time\_OutputPort(Fw::InputFwTimePort\* port);

Note that the set\_Time\_OutputPort() call is shared with the telemetry
ports. It can be called once for both.

### Parameter Ports

The standard port accessor functions for parameters is as follows:

> void set\_ParamGet\_OutputPort(Fw::InputFwPrmGetPort\* port);
>
> void set\_ParamSet\_OutputPort(Fw::InputFwPrmSetPort\* port);

## Registering Commands

The pattern for dispatching commands is for a user-implemented command
dispatch component to connect an output command port to the input
command port for each component servicing commands. Internally, the
dispatcher would map the set of opcodes for a particular component to
the port that is connected to that component. To aid that process, the
code generator creates a command registration function when there are
commands specified for a component. It takes as an argument the port
number on the dispatcher component that is connected to the component’s
command port. The registration port should be connected to the
dispatcher’s registration port as described in Section 6.8.3.2. Then the
regCommands() method can be called in the component, which will invoke
the registration port for each of the opcodes defined. This method
should be called for each component that has commands.

## Loading Parameters

Section 6.8.3.5 describes how to connect a parameter output port to a
component providing parameter storage. After the two components are
connected, the base class method loadParameters() can be called. That
method will request the values of all parameters for that component via
the parameter port. From then on, they will be available to the
implementation class for use. Although it is common to only read
parameters at software initialization, there is nothing that prevents a
re-read after the software has started running by invoking the
loadParameters() call again in the event that the parameter storage was
updated.

## Starting Active Components

The last action in constructing the topology is to start the tasks for
any active components. The start() method is found in the
ActiveComponentBase base class in Fw/Comp/FwActiveComponentBase.hpp.
Table 22 provides the arguments and their meanings.

**Table 22.** Active component start()
arguments.

| Argument   | Meaning                                                                                                       |
| ---------- | ------------------------------------------------------------------------------------------------------------- |
| identifier | A thread-independent value that is used to identify activities of the thread. Should be unique in the system. |
| priority   | The execution priority of the task: 0 = low priority, 255 = high priority.                                    |
| stackSize  | The size of the stack given to the task.                                                                      |

As mentioned in Section 6.7.8.3, the functions preamble() and
finalizer() will be run once before and after the loop waiting for port
invocations.