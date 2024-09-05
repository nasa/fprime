# F´ Implementation Classes

**Note:** for a hands-on walk-through of build topologies, please see: [Tutorials](../../Tutorials/README.md)

The code generator takes the XML definitions in the previous section and
generates C++ base classes. The developer writes classes that derive
from those base classes and implements the project-specific logic. For
input ports and commands, the base classes declare pure virtual methods
for the derived class to implement. If a developer forgets to implement
these functions, the compilation of the code will fail. For output
ports, telemetry channels, events, and parameters, the base class
provides methods for the base class to call.

Depending on the kind of the component, the virtual calls will be made
on the thread of the component itself or the thread of a component
calling a synchronous or guarded port.

This guide walks through how to use the autocoded setup for:
- [Ports](#ports)
- [Commands](#commands)
- [Channels/Telemetry](#telemetry)
- [Events](#events)
- [Parameters](#parameters)

Advanced topics:
- [Internal Interfaces](#internal-interfaces)
- [Message Pre-Hooks](#message-pre-hooks)
- [Initialization Code](#initialization-code)


## Ports

### Input port calls

The pure virtual function to implement a port call is derived from the
name of the port declaration in the component XML. The function is
declared in the protected section of the class and has the following
naming scheme:

> \<port name\>\_handler(NATIVE\_INT\_TYPE portNum, \<argument list\>) =
> 0;

where

> \<port name\> = The name given to the port in the name= tag in the
> port section of the XML.
>
> portNum = If the XML writer has defined multiple ports, this allows
> the developer to know which port was invoked. The value is the port
> instance indexed to zero. In the event the “max\_number” attribute is
> not specified (i.e., a single input port), this value will be zero.
>
> \<argument\_list\> = The list of arguments specified in the args
> section of the port definition XML.

### Output port calls

The base class function for outgoing port calls is derived from the name
and type of the port declaration in the component XML. The function is
declared in the protected section of the class and has the following
naming scheme:

> \<port name\>\_out(NATIVE\_INT\_TYPE portNum, \<argument list\>);

where

> \<port name\> = The name given to the port in the name= tag in the
> port section of the XML.
>
> portNum = If XML writer has defined multiple ports, this allows the
> developer to specify which port to invoke. The value is the port
> instance indexed to zero. In the event the “max\_number” attribute is
> not specified (i.e., a single output port), this value should be set
> to zero.
>
> \<argument\_list\> = The list of arguments specified in the args
> section of the port definition XML.

The call will invoke the port methods defined on whatever component the
component in consideration is interconnected with. If those ports are defined as
synchronous or guarded, the other component’s logic will execute on the
thread of the call.

If the port is not connected and is called, the code will assert. If the
design calls for ports that are optionally connected, the connection
status can be checked before calling via this function:

> isConnected \_\<port name\>\_OutputPort(NATIVE\_INT\_TYPE portNum);

### Port number calls

A method in the base class can be called to get the number of ports
available. The method has the following naming scheme:

> NATIVE\_INT\_TYPE getNum\_\<port name\>\_\<direction\>Ports();

where

> \<port name\> = The name given to the port in the name= tag in the
> port section of the XML.
>
> \<direction\> = The direction of the port, Input or Output.

The developer can use this to automatically scale the code to the number
of ports specified in the XML. If the port output function is called
with a portNum value greater than the number of ports minus one, the
code will assert.

## Commands

The pure virtual function to implement a command is derived from the
mnemonic in the command declaration in the component XML. The function
is declared in the protected section of the class and has the following
naming scheme:

> \<mnemonic\>\_cmdHandler(FwOpcodeType opcode, U32 cmdSeq, \<argument
> list\>) = 0;

where

> \<mnemonic\> = The mnemonic string of the command given in the
> mnemonic= tag in the command section of the XML.
>
> \<argument\_list\> = The list of arguments specified in the args
> section of the command definition XML.

When the command has been completed, a command response method must be
called in the base class to inform the dispatcher of the command that it
has completed. That function call is as follows:

> void cmdResponse\_out(FwOpcodeType opCode, U32 cmdSeq,
> Fw::CommandResponse response);

The opcode and cmdSeq values passed by the function should be passed to
the command response function as well as a status indicating the success
or failure of a command. The opcode is specified in the XML, and cmdSeq
will be set by the command dispatcher to track where the command is in a
sequence of commands. If more information about a failure is needed, an
event should be specified and called with the additional information
(see Section 6.7.4). If a command takes a number of steps and the call
to the command dispatch function does not complete the command, the
opcode and command sequence should be stored for a later call to the
command response function.

## Telemetry

A telemetry channel is intended to be used for periodically measured
data. It is a snapshot in time, and all values may not be permanently
recorded and sent to the command and data handling software. The code
generator generates a base class function for each telemetry channel
defined in the XML. The developer calls this to write a new value of the
telemetry being stored. The function is declared in the protected
section of the class and has the following naming scheme:

> tlmWrite\_\<channel name\>(\<type\>& arg);

where

> \<channel name\> = The name given to the port in the name= tag in the
> channel section of the XML.
>
> \<type\> = The non-namespace qualified type of the channel as
> specified in the data\_type= tag in the XML.

The argument is always passed by reference to avoid copying. The call
internally adds a timestamp to the value. There is a method getTime() in
the base class if the developer wishes to use a time value for other
purposes.

## Events

Events are intermittent and are all recorded to reconstruct a series of
actions or events after the fact. The code generator generates a base
class function for each event defined in the XML. The developer calls
whenever the event to be recorded happens. The function is declared in
the protected section of the class and has the following naming scheme:

> log\_\<severity\>\_\<event name\>(\<event arguments\>);

where

> \<severity\> = The value of the severity attribute in the XML for the
> event.
>
> \<event name\> = The name of the event given in the name attribute in
> the XML.
>
> \<event arguments\> = The argument list of the event.

The call internally adds a timestamp to the event. There is a method
getTime() in the base class if the developer wishes to use a time value
for other purposes.

## Parameters

Parameters are values that are stored non-volatilely and are ways to
influence the behavior of the software without requiring software
updates. During initialization, the parameters are retrieved and stored
in the component base class for later use be the developer’s derived
class. If for some reason the parameters cannot be retrieved, the
default value specified in the XML is returned. The function is declared
in the protected section of the class and has the following naming
scheme:

> \<parameter type\> paramGet\_\<parameter name\>(Fw::ParamValid&
> valid);

where

> \<parameter type\> = The type of the parameter specified by the
> data\_type tag in the XML.
>
> \<parameter name\> = The name of the parameter given in the name
> attribute in the XML.

The parameter value is returned by reference to avoid copying the data.
The valid value should be checked after the call to see what the status
of the parameter value is. Table 21 provides the possible values of the
status and their meanings.

**Table 21.** Parameter retrieval status
values.

| Value              | Meaning                                                                                                                                                                                               |
| ------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Fw::PARAM\_UNINIT  | The code to attempt to retrieve the value was never called. This is most likely an error in forgetting to call the loadParameters() public function for the component during software initialization. |
| Fw::PARAM\_VALID   | The parameter was successfully retrieved.                                                                                                                                                             |
| Fw::PARAM\_INVALID | The parameter was not successfully retrieved, and no default was specified.                                                                                                                           |
| Fw::PARAM\_DEFAULT | The parameter was not successfully retrieved, but a default was provided.                                                                                                                             |

A virtual method is defined in the base class:

> void parameterUpdated(FwPrmIdType id);

By default this method does nothing, but the developer can override the
method if the implementation needs a notification of when a parameter
value is updated. It is called each time a parameter is updated.

## Internal Interfaces

When internal interfaces are specified in the component XML, a function
is generated that can be called by implementation code to dispatch a
message for a message loop. The function has the following name:

> \<internal interface name\>\_internalInterfaceInvoke(\<arguments\>);

A handler function definition is also defined for the function that will
be called when the internal interface message is dispatched. The
function has the following name:

> \<internal interface name\>\_internalInterfaceHandler(\<arguments\>);

The function is defined as a pure virtual to make sure it is
implemented.

## Message Pre-hooks

When asynchronous ports or commands are specified, the code generator
defines functions that can be called prior to dispatching the message.
This provides a lightweight mechanism to do some work before the message
is dispatched. The function is defined as a virtual (not pure) function
with a default implementation that is empty. The implementer can
override the function with an alternate version.

The function name for ports is as follows:

> void \<port name\>\_preMsgHook(NATIVE\_INT\_TYPE portNum, \<port
> arguments\>);

The values of the port arguments are passed to the function.

The function name for commands is as follows:

> void \<command mnemonic\>\_preMsgHook(FswOpcodeType opcode, U32
> cmdSeq);

It does not provide the arguments for the command since they are not
extracted until the command message is processed.

## Initialization Code

### Constructor

The component framework has the option of storing component names for
component interconnection testing and tracing. This is enabled or
disabled via the class naming configuration discussed in Section 9.2.
The macro that indicates whether or not the naming is enabled is
FW\_OBJECT\_NAMES*.* The developer should define and implement two
alternate constructors, one that takes a name argument and one that does
not. As seen in the example, the only difference between the two
constructor implementations is that the base class constructor needs to
be passed the name argument. The user can add any custom constructor
code as well, but at this stage, the component base class is not
initialized so no port calls should be made.

### Initialization

Each component base class has an init() function that must be called
before interconnecting components. If the component is queued or active,
a queue depth argument must be provided. In addition, there is an
optional instance argument if the component is going to be instanced
more than once. This function can be called from a parallel init()
function in the derived class.

### Task Preamble/Finalizer

Active components provide a preamble prototype for code that can be run
once before the thread blocks waiting for port invocations and a
finalizer prototype for code that runs when the component exits the
message loop. These two functions are called on the thread of the active
component. They are declared as virtual functions in C++, so they are
not required. The preamble function is named preamble(void) and the
finalizer is named finalizer(void). They can be used to do one-time activities
such as data structure initialization and cleanup.