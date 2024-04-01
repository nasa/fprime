# A Tour of the Source Tree

The following directories constitute the demonstration code. The
directories are a mix of tools, framework code, and demonstration code.
Details on each module can be seen in the docs/sdd.md (or html)
subdirectory. When the code generator is run in a particular directory,
it will generate files with the suffixes Ac.hpp and Ac.cpp. These files
are not described since they are considered build products. They are
automatically incorporated into the build by the build system. The
following files in Table 1 are produced by the code generator.

**Table 1.** Files produced by the code generator.

| Source                     | Generates                        | Description                  |
| -------------------------- | -------------------------------- | ---------------------------- |
| \<Name\>SerializableAi.xml | \<Name\>SerializableAc.hpp(.cpp) | Autocoded serializable files |
| \<Name\>PortAi.xml         | \<Name\>PortAc.hpp(.cpp)         | Autocoded port files         |
| \<Name\>ComponentAi.xml    | \<Name\>ComponentAc.hpp(.cpp)    | Autocoded component files    |
| \<Name\>TopologyAi.xml     | \<Name\>TopologyAc.hpp(.cpp)     | Autocoded topology files     |

## Autocoders

The Autocoders directory contains the scripts that are used to generate
source for the components. It is implemented as a set of Python 2.x
scripts that process the XML files used to describe the various entities
in the system. The directory also contains the C language version of the
hierarchical state machine (HSM) framework that has been flown on
several missions at JPL. The HSM is not needed by the framework or the
code generation, but is available for use by developers writing
component adaptations if they wish to use HSM for implementing state
machines. Developers do not need to study the implementation of the
autocoder in order to utilize it. The following directories are of
interest to developers:

### Templates

This directory has examples of the XML files that the developer would
write for each entity in the system. Each file will be covered in detail
in subsequent sections. Their function in the architecture is described
in the architecture document. The files are as follows:

  - Example2SerializableAi.xml—An example of a serializable type

  - ExampleSerializableAi.xml—An example of a more complicated
    serializable type

  - ExamplePortAi.xml—An example of a port type

  - AnotherPortAi.xml—A second port definition

  - ExampleComponentAi.xml—An example of a component type

  - ExampleType.hpp(.cpp)—Not XML, but an example of a user-written
    serializable

  - ExampleComponentImpl.hpp(.cpp)—An example of a user-written
    component class

The user can copy these files and use them as a basis for their own XML
files.

## Docs

The Docs directory contains documentation related to the design and
usage of the F′ framework.

## Fw

The Fw directory is the location of framework code and base classes.
This code should not be modified (with one exception, see Cfg in Section
5.3.1) by developers using the framework. The code generation relies on
the types declared to construct the entities in the architecture.

### Cfg

The Cfg directory contains a header file (Config.hpp) that is used to
configure various properties of the architecture. The developer can
modify the file to tune the architecture for the requirements of a
particular deployment environment. The contents are described in Section
9.

The file AcConstants.ini contains a set of values for variables used in
the code generator. This file follows the Python ConfigParser syntax,
which is based on Windows .ini files. Using this file allows component
features like opcodes and port numbers to be changed without modifying
the component XML itself. See the component XML specification in Section
6.6.3.

### Types

The Types directory contains basic types and other base classes used in
the architecture, as shown in Table 2.

**Table 2.** Types directory
files.

| File                              | Description                                                                                         |
| --------------------------------- | --------------------------------------------------------------------------------------------------- |
| BasicTypes.hpp                    | Defines portable built-in data types and common macros                                              |
| Assert.hpp(.cpp)                  | Defines macros to declare an assertion in C++ code                                                  |
| CAssert.hpp                       | Defines macros to declare an assertion in C code                                                    |
| StringType.hpp(.cpp)              | Declares a string base class                                                                        |
| Serializable.hpp(.cpp)            | Declares the serializable base classes and helper functions                                         |
| PolyType.hpp(.cpp)                | Describes a serializable polymorphic type class that can be used to uniformly store different types |
| String.hpp(.cpp)       | A fixed length string available for general usage if the developer does not wish to write one       |
| InternalInterfaceString.hpp(.cpp) | A string class used by internal interfaces when a string argument is specified                      |

### Obj

The Obj directory contains class declarations and implementations for
the root base class in the architectural framework. FwObjBase.hpp
contains the declaration for the object base class. In addition, it
contains a declaration for an object registry class. An object registry
is an optional feature that allows all objects to be registered as they
are created. It is a way to keep track of which objects have been
created as well as perform some common actions such as printing a string
representation of each object. Components and ports use the object class
as a base class. Table 3 lists the files and their descriptions.


**Table 3.** Obj directory files.

| File                        | Description                                                                                                                                                  |
| --------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| ObjBase.hpp(.cpp)           | Declaration for object base class                                                                                                                            |
| SimpleObjRegistry.hpp(.cpp) | An implementation of a simple object registry: This registry simply stores created object pointers in an array and calls their toString() method when asked. |

### Port

The Port directory contains the base classes for ports. Table 4 lists
the files and their descriptions.

**Table 4.** Port directory files.

| File                          | Description                                                                                                                                                                                                           |
| ----------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| PortBase.hpp(.cpp)            | Port base class; contains methods and attributes common to all ports                                                                                                                                                  |
| InputPort.hpp(.cpp)           | Input port base class that is derived from port base class; contains methods and attributes common to all input ports                                                                                                 |
| OutputPort.hpp(.cpp)          | Output port base class that is derived from port base class; contains methods and attributes common to all output ports                                                                                               |
| InputSerializePort.hpp(.cpp)  | Input serialize port class: Typed output ports can be connected to input serialize ports. The typed output port will serialize its arguments prior to invoking the port.                                              |
| OutputSerializePort.hpp(.cpp) | Output serialize port class: Output serialize ports can be connected to typed input port. The serialize port passes the typed port a buffer representing the serialized arguments, which the typed port deserializes. |

### Comp

The Comp directory contains the class declarations for the various kinds
of components. These classes act as base classes for components created
by the code generation and are not directly used by developers. Table 5
lists the files and their descriptions.

**Table 5.** Comp directory files.

| File                           | Description                                                                                                                                                         |
| ------------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| PassiveComponentBase.hpp(.cpp) | The base class for unthreaded passive components: These components have no thread of execution associated with them. This class derives from the object base class. |
| QueuedComponentBase.hpp(.cpp)  | The base class for queued components: These components have a message queue but no thread. It is derived from the passive component base class.                     |
| ActiveComponentBase.hpp(.cpp)  | The base class for active components: Active components have a thread of execution as well as a message queue. It is derived from the queued component class.       |

### Cmd

This Cmd directory contains XML and class declarations used to generate
code for command interfaces to components. The XML generates port
classes in the normal way via the code generator. The code generator
then uses those generated classes as special command input ports for
components that define commands in their component XML. Since the ports
themselves are generated in the same way as any other port, they can be
used by developers in other components that process commands, such as command dispatchers. Table 6 lists the files and their descriptions.

**Table 6.** Cmd directory files.

| File                   | Description                                                                                                                                                                       |
| ---------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| CmdPortAi.xml          | XML description of a command port                                                                                                                                                 |
| CmdResponsePortAi.xml  | XML description of a command response port                                                                                                                                        |
| CmdRegPortAi.xml       | XML description of a command registration port                                                                                                                                    |
| CmdArgBuffer.hpp(.cpp) | A class used by the command port that is a data buffer containing the serialized form of the command arguments                                                                    |
| CmdString.hpp(.cpp)    | A string class used by the command code generator for string arguments                                                                                                            |
| CmdPacket.hpp(.cpp)    | A class representing an encoded command packet that contains a command opcode and arguments: The code generator does not depend on this class, so it can be modified or not used. |

### Tlm

This Tlm directory contains XML and class declarations used to generate
code for channelized telemetry interfaces for components. Channelized
telemetry has historically been a snapshot in time of a set of data. Every
value of that data is not necessarily stored permanently, but is
sampled. The XML generates port classes in the normal way via the code
generator. The code generator then uses those generated classes as
special telemetry output ports for components needing telemetry. Since
the ports themselves are generated in the same way as any other port,
they can be used by developers in other components that process
telemetry, such as a telemetry buffer for downlinking telemetry. Table 7
lists the files and their descriptions.

**Table 7.** Tlm directory files.

| File                | Description                                                                                                                                                                                                       |
| ------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| TlmPortAi.xml       | XML description of a telemetry port                                                                                                                                                                               |
| TlmBuffer.hpp(.cpp) | A data buffer class that represents the serialized form of the telemetry channel                                                                                                                                  |
| TlmString.hpp(.cpp) | A string class used by the telemetry code generator when a string is the telemetry channel                                                                                                                        |
| TlmPacket.hpp(.cpp) | A notional class representing an encoded telemetry packet that contains a telemetry channel identifier and serialized value: The code generator does not depend on this class, so it can be modified or not used. |

###  Log

This Log directory contains XML and class declarations used to generate
code logging (event) interfaces for components. Developer implementation
code sends log events to capture all the events of interest in a system
as they happen. Other components serve to store events for forwarding to
a ground interface or test software. An XML definition for telemetry
ports is defined, which the code generator then uses as special logging
output ports for components. Since the ports themselves are generated in
the same way as any other port, they can be used by developers in other
components that process logging, such as a logging history. Table 8
lists the files and their descriptions.

**Table 8.** Log directory files.

| File                    | Description                                                                                                                                                                                                           |
| ----------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| LogPortAi.xml           | XML description of a logging port                                                                                                                                                                                     |
| LogTextPortAi.xml       | XML description of an optional text logging port: This port generates a string representing the logged event. The code generator generates code to use both kinds of logging ports, but the text log can be disabled. |
| LogBuffer.hpp(.cpp)     | A data buffer class that represents the serialized form of the log entry                                                                                                                                              |
| LogString.hpp(.cpp)     | A string class used by the log code generator when a string is the telemetry channel                                                                                                                                  |
| TextLogString.hpp(.cpp) | A string class used by the text log interface to pass strings                                                                                                                                                         |
| LogPacket.hpp(.cpp)     | A notional class representing an encoded log packet that contains a log entry identifier and serialized set of values: The code generator does not depend on this class, so it can be modified or not used.           |

### Prm

This Prm directory contains XML and class declarations used to generate
parameter interfaces for components. Parameters are values are meant to
be stored in nonvolatile storage that affect various properties of the
software. Parameters are loaded at run time and given to components on
request. An XML definition for a parameter port is used by the code
generator to create special parameter output ports for components. Since
the ports themselves are generated in the same way as any other port,
they can be used by developers in other components that provide
parameters. Table 9 lists the files and their descriptions.

**Table 9.** Prm directory files.

| File                | Description                                                                                                                                                                                               |
| ------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| PrmPortAi.xml       | XML description of a parameter port                                                                                                                                                                       |
| PrmBuffer.hpp(.cpp) | A data buffer that represents a serialized parameter                                                                                                                                                      |
| PrmString.hpp(.cpp) | A string class used by the parameter code generator when a string is the parameter value                                                                                                                  |
| PrmPacket.hpp(.cpp) | A notional class representing an encoded parameter packet that contains a parameter identifier and serialized value: The code generator does not depend on this class, so it can be modified or not used. |

### Time

This Time directory contains XML and class declarations used to generate
time interfaces for components. The time interface port is created by
the code generator as a source of time for time-tagging telemetry
samples and log events. Since the ports themselves are generated in the
same way as any other port, they can be used by developers in other
components that provide time from whatever sources are present in the
system. Table 10 lists the files and their descriptions.

**Table 10.** Time directory files.

| File           | Description                                                                        |
| -------------- | ---------------------------------------------------------------------------------- |
| TimePortAi.xml | XML description of a time port                                                     |
| Time.hpp       | A class containing a time value that represents the time when the port was invoked |


### Com

This Com directory contains definitions for a communication port. This
port could be used as an interface to components that send and receive
data to ground or test software. Table 11 lists the files and their
descriptions.

**Table 11.** Com directory files.

| File                | Description                                                                                                                      |
| ------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| ComPortAi.xml       | XML description of a communication port                                                                                          |
| ComBuffer.hpp(.cpp) | A data buffer class used to represent a packet of serialized communication data                                                  |
| ComPacket.hpp(.cpp) | A data packet class representing data from one of the telemetry or command types: The specific packet types are derived classes. |

## Svc

The intent of this directory is to provide a set of components
implementing services that would be useful for a flight application. The
service layer in software is traditionally the layer that provides the
mechanism for executing the software and managing data. The components,
ports, and other types are examples of how the architecture can be
applied. The component example implementations are very simple; flight
versions would most likely be more sophisticated. The way development is
done is that the developer will define the components and their
properties in XML. The code generator will generate C++ classes that
encapsulate the features of the component. The developer will then write
a class that derives from those generated classes and implement the port
methods. For these directories, each file will not be described, but a
higher-level description of what each directory contains will be given
instead. The descriptions are as follows.

### ActiveLogger

This directory contains a component XML description and implementation
for an active component that accepts serialized log events. The input
port accepting log entries puts them in a message queue for the
component thread. The component thread calls the port handler in the
derived class written by the developer. In this case, the handler simply
takes the log entry, serializes it into a communications buffer, and
sends it out via the output communications port. There are commands for
filtering the event levels.

### ActiveRateGroup

This directory contains a component implementation of an active rate
group. In real-time programming, a rate group is a thread of execution
that does a sequential set of operations that execute cyclically and are
required to be complete by a certain deadline. In this case, this
component provides the thread for the rate group and sequentially calls
a set of output ports that would be connected to other components doing
the specific operations that are required.

### CmdDispatcher

This directory contains an implementation of a command dispatcher. The
commands are received in serialized form from another component. The
command identifier (opcode) is deserialized, and an output port is
looked up that is matched with that identifier. The port is then invoked
with the serialized command arguments. After executing the command, the
component responds back to the dispatcher via a response port and
reports on the outcome of the command. The dispatcher then calls a
status output port if connected in the event there was something else
such as a sequencer waiting for a result. During initialization, a
command registration port is called by components to match the
identifier with the port that the component is executing commands from.
The component also implements some NO\_OP commands.

### CmdSequencer

This directory contains an implementation of a command sequencer. A
sequence file uploaded to the system contains a set of commands that are
executed in order with optional time points. The sequencer waits until
the current command is complete before executing the next in the
sequence. A failed command terminates the sequence.

### CmdRecord

This directory specifies the port used to pass command buffers between
the SequenceFileLoader and the SequenceRunner components.

### Cycle

This directory specifies the port used to drive the ActiveRateGroup
components. The port passes a time stamp indicating when the cycle started.

### Fatal

The directory specifies a port used to pass a notification that a FATAL
event has occurred. It is currently produced by the ActiveLogger
component when it receives a FATAL event from a component.

### GndIf

The directory contains just the XML definition of a component that could
be used to send and receive communications packets. The uplink port
would be connected to the command dispatcher for executing commands, and
the downlink port would be connected by components collecting downlink
telemetry like logs and channelized data. A derived class implementing a
TCP/IP socket version can be seen in SocketGndIf.

### Hub

A hub is a pattern for communicating between computing nodes. A hub
component is a component that contains input and output serialized
ports. As mentioned in Section 5.3.4, when a typed port is connected to
a serialized port, the port arguments are serialized and passed as a
data buffer to the serialized port. Likewise, when a data buffer is sent
from a serialized port to a typed port, the data are deserialized back
into the typed arguments of the port. A hub component takes the
serialized data passed to it and sends it via a communication channel to
a hub on a remote node. The remote hub takes that data and calls a
serialized output port connected to a typed port of the same type as the
original port. That allows components to be interconnected across
computing nodes without any modifications to the components themselves.
This particular hub is implemented as an active hub, which means that
the serial buffers from the incoming ports are put in a message queue
and then sent out to the DataOut ports on the thread of the component. The
data output ports are in the form of a generic com buffer. The data
output ports would be connected to a component that manages the
communication hardware. Likewise, incoming data on the DataIn port is
queued for the component thread, which sends it out via a serialized
output port.

### LinuxTime

This component is an adaptation of the time source component specified in
the Time directory. In this case, it is a time source that makes a Linux
system call for the Linux demo.

### PassiveConsoleTextLogger

This is an adaptation of the PassiveTextLogger component that simply
takes the text version of the log and prints it to the standard output.

### PassiveRateGroup

This is a rate group with the same implementation as the active rate
group in Section 5.4.2, with the exception that the component does not
have a thread. The thread that calls the input run port will be used to
call all the output ports. This component is not currently used in the
reference application.

### PassiveTextLogger

This defines a base class for a component that prints the text version
of events. It executes on the thread of the caller. The implementation
classes are elsewhere, such as PassiveConsoleTextLogger.

### PolyDb

This component implements a database of PolyType entries. The intent is
for this to be used as a database of values being used by different
components in the system. Some components submit new values they have
gathered, and other components retrieve the ones they use.

### PolyIf

This contains the definition for a PolyPort, or a port that passes a
PolyType. It is used to set and get values for the PolyDb component.

### PrmDb

This component implements storage for parameters. It implements the
framework setPrm and getPrm ports. Parameter values are stored as a
table based on parameter ID. It reads a file during initialization that
contains the parameter values and loads them into memory. Subsequent
calls to getPrm will get the loaded file. Parameter values in the
components can be updated by command and saved to PrmDb. The PrmDb
component can be commanded to save the updated values to a file.

### RateGroupDriver

This component takes a primary clock tick in the system and divides it
down to drive output ports. Constructor arguments define the divisors
for each port. The output ports are meant to be connected to the input
ports of rate groups to drive them at the correct rate.

### Sched

This directory contains the definition of a scheduler port that is used
by the rate group components and rate group members.

### SequenceFileLoader

This directory contains a component that loads a set of command buffers
from a file and passes them to the SequenceFileRunner component.

### SequenceRunner

The directory contains a component that will sequence through a series
of command buffers passed through its CmdRecord port.

### SocketGndIf

This directory contains a notional uplink/downlink component that
communicates with ground software via a TCP/IP socket. It would be
connected to telemetry sources and the command dispatcher.

### Time

This directory contains the XML definition for the time source base
class. A time source is necessary for time-tagging the telemetry and log
events in components. Various implementations that derive from this base
class will provide time.

### Tlm

This directory defines a passive telemetry storage component base class.
It has an input port for the telemetry buffers sent by components. It
has an output port to send the telemetry packets to a ground interface
component like the one in Section 5.4.21. It has a scheduler input port
so that it can be executed periodically on a rate group to send the
stored telemetry to the ground interface.

### TlmChan

This directory contains an adaptation of the Tlm base class in Section
5.4.23. In this adaptation, the telemetry is stored in an array of
telemetry buffers based on the telemetry ID. The storage is
double-buffered. When the scheduler port is invoked, the component
switches the active array to non-active and starts copying the
non-active array to the packet output port. If incoming telemetry calls
happen during the copy operation, they are placed in the active array.

## Os

This directory contains classes that abstract operating system features.
This allows the components that are code generated to not be dependent
on a particular operating system. The architecture is dependent on these
classes. The subdirectories contain implementations of the class for
different operating systems. Not all operating systems will implement
all classes. The classes are as follows.

### Task

This class represents a task (AKA thread) in an operating system
process. It has methods for starting, ending, waiting, and suspending
tasks. This class is used by active components.

### Queue

This class represents a message queue. It has methods for creating,
writing to, reading from, and destroying queues. This class is used by
queued components.

### Mutex

This class represents a mutex. It is used to guard critical sections of
data and code. It is used by components that have guarded ports (see
architecture description).

### File

This class represents a file. It is used to abstract away various
operating system implementations of file I/O.

## Drv

The Drv directory contains some hypothetical device driver components
and types. It is part of the example code. The architecture does not
depend on the source code in this directory. There is no particular
significance to the name; rather, it was selected to represent how a
developer might organize their code. The subdirectories are as follows.

### BlockDriver

This represents a hardware driver that accepts buffers of data to send
to a device, and sends buffers that it receives from the device. Since
there is no real hardware behind the driver, the driver takes any
incoming data buffers from the input port and sends them out the output
port.

### DataTypes

This directory contains the port and data buffer types used by the
driver and components using the driver.

## Ref

This directory contains a reference application. Components here
represent what an adapter might do when writing application-specific
logic. An adapter would use the framework layers, drivers, and services
that are meant to be reusable, along with application components for a
particular task.

### SendBuffApp

This passive component represents a part of the application that sends
data to a consumer. It runs periodically in a rate group and sends a
packet upon command. It uses the driver described in Section 5.6.1.

### RecvBuffApp

This active component represents a part of the application that receives
data from a sender, in this case SendBuffApp. It receives a buffer from
the driver.

### Top

This is the topology module. This is where all the components are
instantiated and connected together, and the active components are
started. It is also the location of the C main() function entry point.
Each deployment (see Section 6.2) will have a module similar to this.