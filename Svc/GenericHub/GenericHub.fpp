module Svc {

  @ A generic hub component
  @
  @ In F Prime, a hub is a mechanism for implementing logical port connections
  @ that physically span two F Prime deployments. This component provides a generic
  @ capability for implementing a hub. Typically there is one instance of this
  @ component in each deployment, and each instance is paired with a driver for
  @ communicating between the deployments. Sending data between the deployments
  @ looks like this:
  @
  @   FSW --> GenericHub --> Driver ~~> Driver --> GenericHub --> FSW
  @
  @ The notation ~~> represents data transport between deployments,
  @ e.g., via shared memory or across a network connection.
  @ The Driver is specific to the transport mechanism.
  @ The GenericHub may be paired with any driver that conforms to
  @ its interface, and so can support any transport mechanism.
  passive component GenericHub {

    # ----------------------------------------------------------------------
    # Ports for sending data to the hub
    # ----------------------------------------------------------------------
    # These ports establish the "send" interface from the rest of FSW to the hub.
    #
    # Each of these ports has the following behavior:
    # 1. Invoke dataOutAllocate to allocate a buffer B.
    # 2. Serialize the hub message type (event, telemetry, serial, buffer),
    #    the port number, and the data into B.
    # 3. Emit B on dataOut.
    # ----------------------------------------------------------------------

    @ Port for sending events to the hub
    @ TODO: Rename this port eventIn
    sync input port LogRecv: Fw.Log

    @ Port for sending telemetry to the hub
    @ TODO: Rename this port tlmIn
    sync input port TlmRecv: Fw.Tlm

    @ Ports for sending serial data to the hub
    @ You can connect any typed output port to this input port
    @ TODO: Rename this these ports serialIn
    sync input port portIn: [GenericHubInputPorts] serial

    @ Ports for sending buffer data to the hub
    @ Output ports connected to this port must emit buffers.
    @ On invocation, each of these ports allocates a new buffer B, copies the
    @ data from the incoming buffer to B, and deallocates the incoming
    @ buffer.
    @ TODO: Rename these ports bufferIn
    sync input port buffersIn: [GenericHubInputBuffers] Fw.BufferSend

    @ Port for deallocating buffers sent on buffersIn
    output port bufferDeallocate: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Ports for sending data to a driver
    # ----------------------------------------------------------------------
    # These ports establish the "send" interface from the hub to a driver.
    #
    # TODO: Make this interface conform to the Byte Stream Driver Interface (BSDI)
    # * For the sync BSDI, there should be an output port of type Drv.ByteStreamSend
    # * For the async BSDI, there should be an input port of type Drv.ByteStreamData
    # ----------------------------------------------------------------------

    @ Port for allocating a buffer to send on dataOut
    output port dataOutAllocate: Fw.BufferGet

    @ Port for sending buffers to a driver
    output port dataOut: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Ports for receiving data from a driver
    # ----------------------------------------------------------------------
    # These ports establish the "receive" interface from a driver to the hub.
    # Each of these ports has the following behavior:
    # 1. Unpack the incoming buffer into hub message type, port number, and data.
    # 2. If the hub message type is event, telemetry, or serial,
    #    then pass the data by value and call dataInDeallocate to deallocate
    #    the incoming buffer.
    # 3. Otherwise adjust the metadata of the incoming buffer to point
    #    to the data, and emit the same buffer. Do not deallocate it.
    # ----------------------------------------------------------------------

    @ Port for receiving buffers from a driver
    @ TODO: Make this interface conform to the BSDI.
    @ The type should be Drv.ByteStreamData.
    sync input port dataIn: Fw.BufferSend

    @ Port for returning buffers received on dataIn
    output port dataInDeallocate: Fw.BufferSend

    # ----------------------------------------------------------------------
    # Ports for receiving data from the hub
    # These ports establish the "receive" interface from the hub to FSW
    # ----------------------------------------------------------------------

    @ Port for receiving events
    @ Data emitted on this port is copied from a buffer received on dataIn
    @ TODO: Rename this port eventOut
    output port LogSend:  Fw.Log

    @ Port for receiving telemetry channels
    @ Data emitted on this port is copied from a buffer received on dataIn
    @ TODO: Rename this port tlmOut
    output port TlmSend: Fw.Tlm

    @ Ports for receiving serial data by value
    @ You can connect each of these output ports to any typed input port
    @ Data emitted on one of these ports is copied from a buffer received on dataIn
    @ TODO: Rename this port serialOut
    output port portOut: [GenericHubOutputPorts] serial

    @ Ports for receiving buffer data by reference
    @ A buffer emitted on one of these ports is a buffer received on dataIn,
    @ With adjusted metadata to point to the data stored in the buffer
    @ TODO: Rename this port bufferOut
    output port buffersOut: [GenericHubOutputBuffers] Fw.BufferSend

  }

}
