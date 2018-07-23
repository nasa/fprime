# GDS

### Overview
The Gds provides a remote interface for fprime deployments, allowing users to 
view telemetry and events and send commands.

The Gds is a heavily refactored version of the fprime Gse. Both the Gse and Gds 
use the ThreadedTCPServer to receive data from the fprime deployment. They also
have very similar looking GUIs, start up scripts, and command line arguments. 
However, The infrastructure supporting each one is very different.

The Gds was designed to be adaptable, easily understanable, and easily 
expandable. To this end, it is built using publisher/subscriber relationships.

The diagram below shows an example structure for incomming data. Data from the 
F' deployment first enters the Gds at the TCP client. Each packet is then passed
directly to the distributor which is responsible for parsing the packets in to 
data messages and sending on each message type (currently only events, channels,
and packetized telemetry are supported) to decoders registered for that type. 
The decoder is responsible for turning that data message into a data object 
which it passes along to all consumers registered to it. These consumers could 
be anything, but in the Gds they are gui panels that display the data. 
![Example Gds incoming data layout](docs/ExampleIncomingDataLayout.png)

For outgoing data, the structure is similar. Currently, only commands are 
supported for outgoing data. Command data objects are created in panels and 
then sent to all encoders who are registered to that panel. The encoders take 
the data object and turn it into binary data that can be sent to the fprime 
deployment. The binary data is then passed to the TCP client which is 
registered to the encoder. Finally, the TCP client send the data back
to the TCP server and the F' deployment.
![Example Gds outgoing data layout](docs/ExampleOutgoingDataLayout.png)

All of these objects are created and registered to other objects when the Gds 
is initialized. Thus, all of the structure of the Gds is created in one place, 
and can be easily modified.

## Classes
The Gds back end is composed of several different data proecssing units. For 
most of the units described below, a base class describes the interface and 
subclasses implement the interface for specific data types (such as events,
channels, etc).

### TCP Client
The TCP client is simply a passthrough for data coming from the TCP Server and 
the F' Distrobution. The client handles all the socket connection overhead and 
passes unparsed data onto all objects registered with it.

### Distributor
The distributer is responsible for taking in raw binary data, parsing off the 
length and descriptor, and then passing the data to all decoders registered to 
that descriptor. Descriptor types include events, channels, packets, etc (a full
enumeration can be found in src/utils/data_desc_type.py). The binary data that 
the descriptor receives should be of the form:

| Length (4 bytes) | Type Descriptor (4 bytes) | Message Data |
| ---------------- | ------------------------- | ------------ |

The distributor should then pass only the message data along to the decoders. 

### Templates
For each general data type (channel, event, etc) there is a template type. 
Instances of these classes hold information about specific channels or event 
types (ex. the NumPkts channel or the FirstPacketReceived event).

Template classes hold information such as the channel/event/packet's id, name, 
argument types, value type, format string, etc. This information is used by 
decoders when parsing data that they receive. 

### Data Types
For each general data type (channel, event, etc) there is a type class. 
Instances of these classes hold information about a specific channel reading or 
event. They contain the actual data parsed by the decoders. As such, they are 
the data type returned by encoders. All of these classes have a time field are 
derived from type SysData, which implements a compare function, allowing any list 
of SysData objects to be sorted by time. Each inherited type should also 
implement the __str__ method so the objects can be easily printed. 

Each instance of a type class also has a reference to the corresponding template
class for that channel or event type.

### Loaders
Loaders are used to construct dictionaries of channel and events. These 
dictionaries have template classes as values and ids or names as keys. 

Each dictionary type has their own loader, but subclassing is used to prevent
code duplication. For example, there are loaders for channel and event python
file dicitonaries, but they both subclass the python loader class which provides
helper functions for reading python file dictionaries. 

### Decoders
Decoders are responsible for parsing the message data for a specific descriptor
type. 

Each decoder uses dictionaries produced by loaders to help with its parsing.
These are given to the decoder's constructor.

The knowledge for how to parse that descrptor type should stay within the 
decoder. Each decoder type takes in the binary message data, parses it, and 
sends the resulting data object to all consumers registered to it. 

### Encoders
Encoders are responsible for taking data objects from consumers (gui panels), 
converting them to binary data, and passing them to the TCP client to send to 
the F' deployment. 

Like the decoders, encoders use dictionaries produced by loaders to help craft
the binary output.

### Consumers
Consumers do not have a specific base class, but instead simply implement a data
callback method that is called by decoders with parsed data objects as the 
argument. In the case of the Gds, the consumers are the GUI panels that display 
data. Consumers can also produce data that is sent to encoders and eventually on
to the F' deployment. 

### Main Panel Factory
This class is responsible for setting up the pipeline of data between different 
components in the publisher/subscriber interface - that is, it is 
responsible for registering all of the various components that whish to share
data. This class also supports the creation of multiple Gds GUI windows which 
all share the same subscriptions and therefore recieve the same data. 

## Notes
 - Currently, the models/common directory has command.py, event.py, and
   channel.py. These files must be present in order for the python dictionaries
   to be properly imported. However, they are empty and not used in the GDS. 
   When we switch to XML dictionaries, these can go away. 

