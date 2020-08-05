**i. Commands, Telemetry, Events, and Parameters**

The code generator provides a method of implementing commands,
telemetry, events (EVRs), and parameters. It parses arguments, types,
and generates code to convert arguments to serialized data and
serialized data to arguments. Calls implementation functions with
deserialized arguments (commands), or provides base class functions to
implement calls (telemetry, events, and parameters). The code generator
uses port types that are specified in XML. These ports are then used in
components that provide interfaces for transporting or storing data for
those services. Further description of commands, telemetry, events, and
parameters are discussed directly below, and the component types are
defined in the section entitled component types.

**Commands:** component command XML specifies opcode, mnemonic,
arguments, and synchronization attributes. Arguments can be any built-in
type or external XML complex type, which can be a single argument, and
defines enumerations. Synchronization attributes are sync, async, or
guarded (similar to the ports). Async specifies message priority.

Implementation class implements the function for each command; and the
autocoder automatically adds ports for registering and receiving
commands, and reporting an execution status.

![](./media/image4.emf)

**Figure 4. Example of command dispatcher.** The command dispatcher
receives the raw buffer containing the command and arguments. The
command opcode is extracted, and lookup is made using table maps to
capture the opcode to send to the port allowing multiple opcode entries
per component. The argument buffer is then passed to the component, and
the command dispatcher becomes a passive component.

![](./media/image5.emf)

**Figure 5. Example of command sequence.** The command sequencer loads
the file from the file system, and sends the command and waits for the
response for each command file. A failed response terminates the
sequence, while a passed response moves to the next command creating an
active component.

**Events:** component event XML specifies ID, name, severity, arguments,
and format specifier string. Arguments can be any built-in type or
external XML complex type, which can be a single argument, and defines
enumerations. Format specifier strings follow the C format specifier
syntax, and are used by ground software and optional on-board consoles
to display a message with argument values.

Code generated base class provides the function to call for each event
with typed arguments; and the code generator automatically adds ports
for retrieving time tag and sending events. There are two independent
ports for sending events: i) binary, and ii) console. The binary port
has serialized arguments for transports to ground software. The console
port has the capability for a text port to send a string of events using
the format specifier.

![](./media/image6.emf)

**Figure 6. Example of event log.** The component implementation calls
function to generate the event. The base class function (code-generated)
retrieves the time tag from the time source component
(project-specific). The component sends the event to the event log
component, then the event log component places the event on the message
queue. The thread of the component sends the downlink with the event,
and the event log becomes an active component.

**Telemetry:** component telemetry XML specifies channels that have ID,
name, data type, and format specifier strings. Data type is any built-in
type or external XML complex type and defines enumerations. Format
specifier strings are used by ground software and optional on-board
consoles to display a message with argument values.

Code generated base class provides a function to call for each channel
with typed arguments, known as implementation class; and the code
generator automatically adds ports for retrieving time tag and sending
channelized data.

![](./media/image7.emf)

**Figure 7. Example of telemetry database.** The telemetry database has
a double-buffered array of telemetry buffers. Implementation class calls
base class function with telemetry channel update. The base class
function retrieves the time tag from the time source component, and then
writes the updated value to the telemetry database component. The
telemetry database writes the value to the active buffer, as the run
port is called periodically by the rate group. This then swaps the
active buffer and the run call copies the updated values to the downlink
creating a passive component.

A rate group is a container of *run()* ports that calls ports in order.
The list of run ports does not know which destinations are in active
components; however, the rate group is an active component.

**Parameters:** are traditional means of storing non-volatile states.
The framework provides code generation to manage; however, the user must
write a specific storage component. Component XML specifies parameters
that have ID, name, data type, and optional default value. Data type can
be any built-in type or external XML complex type and defines
enumerations. Default values are assigned in the event the parameter
cannot be retrieved.

The code generator automatically adds ports for retrieving parameters.
During initialization, a public method in the class is called which
retrieves the parameters and stores copies locally. Calls can reoccur if
the parameter is updated. The code generated base class provides
function to call for each parameter to retrieve the stored copy; and an
implementation class can call whenever the parameter value is needed.

![](./media/image8.emf)

**Figure 8. Example of parameter manager.** The parameter manager loads
the file containing parameters from the file system during
initialization. The initialization subsequently calls *loadParameters()*
on all components with parameters (can be called after). The uplink
parameter updates the stored value of the parameter, and the component
then refreshes the parameters by implementing the command to reload. The
parameter manager saves the updated values to the file system via the
command.

**ii. Multi-node Hub Pattern**

A hub is a component with multiple serialization input and output ports.
Typed ports on calling components are connected to serialized ports, as
previously shown in Figure 3. A single point of connection to a remote
node is essential for a central point of configuration transport, as
shown in Figure 9.

![](./media/image9.emf)

**Figure 9. Hub pattern.** Each hub instance is responsible for
connecting to a remote node. Input port calls are repeated to
corresponding output ports on a remote hub. These hubs have been
demonstrated on Sockets, ARINC 653 Channels, High-speed hardware bus
between nodes, and UARTs between nodes in an embedded system.


### A Note On Serialized Ports
that are specified in the XML. These include commands, events, telemetry, and parameters; and
should be designed so the small-scale projects can handle sufficient
implementations straight away. Facility interfaces are defined to
support existing component implementations for uplink/downlink packet
types that provide input/output of ground systems and C\&DH components,
and uplink/downlink ports that provide projects with specific
uplink/downlink hardware.