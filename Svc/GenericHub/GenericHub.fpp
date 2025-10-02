module Svc {

  @|---------------------------------------------------------------------- 
  @|A generic hub component
  @|---------------------------------------------------------------------- 
  @|In F Prime, a *hub* is a mechanism for implementing logical port connections
  @|that physically span two F Prime deployments. The pattern is called a "hub"
  @|because any number of logical connections may be multiplexed through a single
  @|pair of hubs. For example, a pair of logical connections like this
  @|
  @|            A1 --> B1
  @|            A2 --> B2,
  @|
  @|where An and Bn are component instances in separate deployments A and B,
  @|can be implemented using hubs H1 and H2 like this:
  @|
  @|    A1 -->--+       +-->-- B1
  @|            |       |
  @|            H1 ~~> H2
  @|            |       |
  @|    A1 -->--+       +-->-- B2
  @|
  @|The notation ~~> represents data transport between deployments,
  @|e.g., via shared memory or across a network connection.
  @|
  @|The GenericHub component provides a generic capability for implementing a
  @|hub. Typically there is a pair of instances of GenericHub, one in each
  @|deployment, and each instance is paired with a driver for doing the
  @|communication. Sending data between the deployments looks like this:
  @|
  @|    FSW --> GenericHub --> BufferDriver ~~> BufferDriver --> GenericHub --> FSW
  @|
  @|In this diagram, BufferDriver represents any combination of component instances
  @|that sends and receives Fw.Buffer objects. For example, BufferDriver may be
  @|a pair consisting of (1) a ByteStreamDriver component that implements the
  @|ByteStreamDriverInterface and (2) a ByteStreamBufferAdapter.
  @|The buffer driver is specific to the transport mechanism.
  @|The GenericHub may be paired with any buffer driver that conforms to
  @|its interface, and so can support any transport mechanism.
  @|---------------------------------------------------------------------- 
  passive component GenericHub {

    # ----------------------------------------------------------------------
    # Ports for sending data from FSW to the hub
    # ----------------------------------------------------------------------
    # These ports establish the "send" interface from the rest of FSW to the hub.
    # ----------------------------------------------------------------------
    # Each of these ports has the following behavior:
    # 1. Invoke dataOutAllocate to allocate a buffer B.
    # 2. Serialize the hub message type (event, telemetry, serial, buffer),
    #    the port number, and the data into B.
    # 3. Emit B on dataOut.
    # ----------------------------------------------------------------------

    @ Port for sending events to the hub
    sync input port eventIn: Fw.Log

    @ Port for sending telemetry to the hub
    sync input port tlmIn: Fw.Tlm

    @ Ports for sending serial data to the hub
    @ You can connect any typed output ports to these input ports, so
    @ long as the data carried by the ports is serialized by value.
    @ Do not connect ports that emit Fw.Buffer objects, because these objects
    @ store pointers to data that is not serialized across the port
    @ interface. To connect output ports that emit buffers, use
    @ buffersIn below.
    sync input port serialIn: [GenericHubInputPorts] serial

    @ Ports for sending buffer data to the hub
    @ Output ports connected to these ports must emit Fw.Buffer objects.
    @ On invocation, each of these ports allocates a new buffer B, copies the
    @ data from the incoming buffer to B, and returns the incoming
    @ buffer to the sender for deallocation.
    sync input port bufferIn: [GenericHubInputBuffers] Fw.BufferSend

    @ Port for returning buffers arriving on buffersIn
    @ TODO: Rename this port bufferInReturn
    output port bufferDeallocate: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Ports for sending data from the hub to a buffer driver
    # ----------------------------------------------------------------------
    # These ports establish the "send" interface from the hub to a buffer driver.
    #
    # TODO:
    # * Add an input port of type Fw.BufferSend for receiving returned buffers.
    # * Add an output port of type Fw.BufferSend for deallocating the returned buffers.
    # ----------------------------------------------------------------------

    @ Port for allocating a buffer to send on dataOut
    output port dataOutAllocate: Fw.BufferGet

    @ Port for sending buffers to a buffer driver
    output port dataOut: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Ports for receiving data from a buffer driver to the hub
    # ----------------------------------------------------------------------
    # These ports establish the "receive" interface from a driver to the hub.
    # Each of these ports has the following behavior:
    # 1. Unpack the incoming buffer into hub message type, port number, and data.
    # 2. If the hub message type is event, telemetry, or serial,
    #    then pass the data by value to the receiver and call dataInDeallocate
    #    to return the incoming buffer for deallocation.
    # 3. Otherwise adjust the metadata of the incoming buffer to point
    #    to the data, and emit the same buffer. Do not return it.
    # ----------------------------------------------------------------------

    @ Port for receiving buffers from a buffer driver
    sync input port dataIn: Fw.BufferSend

    @ Port for returning buffers arriving on dataIn
    @ TODO: Rename this port dataInReturn
    output port dataInDeallocate: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Ports for receiving data from the hub to FSW
    # ----------------------------------------------------------------------
    # These ports establish the "receive" interface from the hub to FSW
    # ----------------------------------------------------------------------

    @ Port for receiving events
    @ Data emitted on this port is copied from a buffer received on dataIn,
    @ and the buffer is returned.
    @ TODO: Rename this port eventOut
    output port LogSend:  Fw.Log

    @ Port for receiving telemetry channels
    @ Data emitted on this port is copied from a buffer received on dataIn,
    @ and the buffer is returned.
    @ TODO: Rename this port tlmOut
    output port TlmSend: Fw.Tlm

    @ Ports for receiving serial data
    @ You can connect each of these output ports to any typed input port.
    @ Data emitted on one of these ports is copied from a buffer received on dataIn,
    @ and the buffer is returned.
    @ TODO: Rename this port serialOut
    output port portOut: [GenericHubOutputPorts] serial

    @ Ports for receiving buffer data
    @ A buffer emitted on one of these ports is a buffer received on dataIn,
    @ With adjusted metadata to point to the data stored in the buffer.
    @ TODO: Rename this port bufferOut
    output port buffersOut: [GenericHubOutputBuffers] Fw.BufferSend

  }

}
