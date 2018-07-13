# GDS

### Overview
The Gds provides a remote interface for fprime deployments, allowing users to view 
telemetry and events and send commands.

The Gds is a heavily refactored version of the fprime Gse. Both the Gse and Gds use 
the ThreadedTCPServer to receive data from the fprime deployment. They also have very 
similar looking GUIs, start up scripts, and command line arguments. However, The infrastructure
supporting each one is very different.

The Gds was designed to be adaptable, easily understanable, and easily expandable. 
To this end, it is built using publisher/subscriber relationships.

The diagram below shows an example structure for incomming data. Data from the F' deployment 
first enters the Gds at the TCP client. Each packet is then passed directly to the 
distributor which is responsible for parsing the packets in to data messages and 
sending on each message type (currently only events, channels, and packetized telemetry 
are supported) to decoders registered for that type. The decoder is responsible for 
turning that data message into a data object which it passes along to all consumers 
registered to it. These consumers could be anything, but in the Gds they are gui 
panels that display the data. 
![Example Gds incoming data layout](docs/ExampleIncomingDataLayout.png)

For outgoing data, the structure is similar. Currently, only commands are supported for
outgoing data. Command data objects are created in panels and then sent to all encoders
who are registered to that panel. The encoders take the data object and turn it into binary
data that can be sent to the fprime deployment. The binary data is then passed to the 
TCP client which is registered to the encoder. Finally, the TCP client send the data back
to the TCP server and the F' deployment.
![Example Gds outgoing data layout](docs/ExampleOutgoingDataLayout.png)

All of these objects are created and registered to other objects when the Gds is initialized.
Thus, all of the structure of the Gds is created in one place, and can be easily modified.

## Classes

### TCP Client
The TCP client is simply a passthrough for data coming from the TCP Server and the F'
Distrobution. The client handles all the socket connection overhead and passes unparsed
data onto all objects registered with it.

### Distributor
The distributer is responsible for taking in raw binary data, parsing off the length and 
descriptor, and then passing the data to all decoders registered to that descriptor. Descriptor
types include events, channels, packets, etc (a full enumeration can be found in 
src/utils/data_desc_type.py). The binary data that the descriptor receives should be of the form:

| Length (4 bytes) | Type Descriptor (4 bytes) | Message Data |
| ---------------- | ------------------------- | ------------ |

The distributor should then pass only the message data along to the decoders. 

### Templates
For each general data type (channel, event, etc) there is a template type. Instances of these 
classes hold information about specific channels or event types (ex. the NumPkts channel or the 
FirstPacketReceived event).

Template classes hold information such as the channel/event/packet's id, name, argument types, 
value type, format string, etc. This information is used by decoders when parsing data that they
receive. 

### Data Types
For each general data type (channel, event, etc) there is a type class. Instances of these
classes hold information about a specific channel reading or event. They contain the actual
data parsed by the decoders. As such, they are the data type returned by encoders. All of
these classes have a time field are derived of type SysData, which implements a compare 
function, allowing any list of SysData objects to be sorted by time. Each inherited type 
should also implement the __str__ method so the objects can be easily printed. 

Each instance of a type class also has a reference to the corresponding template class for
that channel or event type.

### Loaders


### Decoders

### Encoders

### Consumers

## Notes
 - Currently, the models/common directory has command.py, event.py, and
   channel.py. These files must be present in order for the python dictionaries
   to be properly imported. However, they are empty and not used in the GDS. 
   When we switch to XML dictionaries, these can go away. 

