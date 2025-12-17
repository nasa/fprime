module Svc {

  @| ----------------------------------------------------------------------
  @| A generic hub component
  @| ----------------------------------------------------------------------
  @| In F Prime, a *hub* is a mechanism for implementing logical port connections
  @| that physically span two F Prime deployments. The pattern is called a "hub"
  @| because any number of logical connections may be multiplexed through a single
  @| pair of hubs. For example, a pair of logical connections like this
  @|
  @|             A1 --> B1
  @|             A2 --> B2,
  @|
  @| where Ai and Bi are component instances in separate deployments A and B,
  @| can be implemented using hubs HA and HB like this:
  @|
  @|     A1 -->--+       +-->-- B1
  @|             |       |
  @|             HA ~~> HB
  @|             |       |
  @|     A2 -->--+       +-->-- B2
  @|
  @| The notation ~~> represents data transport between deployments,
  @| e.g., via shared memory or across a network connection.
  @|
  @| The GenericHub component provides a generic capability for implementing a
  @| hub. Typically there is a pair of instances of GenericHub, one in each
  @| deployment, and each instance is paired with a driver for doing the
  @| communication. Sending data between the deployments looks like this:
  @|
  @|     FSW --> GenericHub --> Driver ~~> Driver --> GenericHub --> FSW
  @|
  @| The driver must be a *buffer driver*, i.e., any combination of component
  @| instances that sends and receives Fw.Buffer objects across a network.
  @| For example, the driver may be a pair consisting of (1) a ByteStreamDriver
  @| component that implements ByteStreamDriverInterface and
  @| (2) a ByteStreamBufferAdapter.
  @|
  @| The driver is specific to the transport mechanism.
  @| The GenericHub may be paired with any driver that conforms to
  @| its interface, and so can support any transport mechanism.
  @| ----------------------------------------------------------------------
  passive component GenericHub {

    # ----------------------------------------------------------------------
    # Ports for sending data from FSW to the hub
    # ----------------------------------------------------------------------
    # These ports establish the "send" interface from the rest of FSW to the hub.
    # ----------------------------------------------------------------------
    # Each of these ports has the following behavior:
    # 1. Invoke allocate to allocate a buffer B.
    # 2. Serialize the hub message type (event, telemetry, serial, buffer),
    #    the port number, and the data into B.
    # 3. Emit B on toBufferDriver.
    #
    # Sample connections:
    #
    #    eventProducer.eventOut -> genericHub.eventIn
    #    telemetryProducer.tlmOut -> genericHub.tlmIn
    #
    #    valueProducer0.valueOut[0] -> genericHub.serialIn[0]
    #    valueProducer1.valueOut[1] -> genericHub.serialIn[1]
    #
    #    bufferProducer0.bufferOut -> genericHub.bufferIn[0]
    #    genericHub.bufferInReturn[0] -> bufferProducer0.bufferIn
    #    bufferProducer1.bufferOut -> genericHub.bufferIn[1]
    #    genericHub.bufferInReturn[1] -> bufferProducer1.bufferIn
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
    sync input port serialIn: [GenericHubCfg.NumSerialInputPorts] serial

    @ Ports for sending buffer data to the hub
    @ Output ports connected to these ports must emit Fw.Buffer objects.
    @ On invocation, each of these ports allocates a new buffer B, copies the
    @ data from the incoming buffer to B, and returns the incoming
    @ buffer to the sender for deallocation.
    @
    sync input port bufferIn: [GenericHubCfg.NumBufferInputPorts] Fw.BufferSend

    @ Ports for returning buffers arriving on buffersIn
    output port bufferInReturn: [GenericHubCfg.NumBufferInputPorts] Fw.BufferSend

    @ bufferIn and bufferInReturn ports must match
    match bufferIn with bufferInReturn

    # ----------------------------------------------------------------------
    # Ports for sending data from the hub to a buffer driver
    # ----------------------------------------------------------------------
    # These ports establish the "send" interface from the hub to a buffer driver.
    #
    # Sample connections:
    #
    #    genericHub.allocate -> bufferManager.bufferGetCallee
    #    genericHub.toBufferDriver -> bufferDriver.bufferIn
    #    bufferDriver.bufferInReturn -> genericHub.toBufferDriverReturn
    #    genericHub.deallocate -> bufferManager.bufferSendIn
    # ----------------------------------------------------------------------

    @ This interface provides ports allocate and deallocate
    import Svc.BufferAllocation

    @ This interface provides ports toBufferDriver and toBufferDriverReturn
    import Drv.PassiveBufferDriverClientSend

    # ----------------------------------------------------------------------
    # Ports for receiving data from a buffer driver to the hub
    # ----------------------------------------------------------------------
    # These ports establish the "receive" interface from a driver to the hub.
    # Each of these ports has the following behavior:
    # 1. Unpack the incoming buffer into hub message type, port number, and data.
    # 2. If the hub message type is event, telemetry, or serial,
    #    then pass the data by value to the receiver and invoke fromBufferDriverReturn
    #    to return the incoming buffer for deallocation.
    # 3. Otherwise adjust the metadata of the incoming buffer to point
    #    to the data, and emit the same buffer on bufferOut. When the
    #    buffer is returned on bufferOutReturn, invoke fromBufferDriverReturn to return it.
    #
    # Sample connections:
    #
    #    bufferDriver.bufferOut -> genericHub.fromBufferDriver
    #    genericHub.fromBufferDriverReturn -> bufferDriver.bufferOutReturn
    # ----------------------------------------------------------------------

    @ This interface provides ports fromBufferDriver and fromBufferDriverReturn
    import Drv.PassiveBufferDriverClientRecv

    # ----------------------------------------------------------------------
    # Ports for receiving data from the hub to FSW
    # ----------------------------------------------------------------------
    # These ports establish the "receive" interface from the hub to FSW
    #
    # Sample connections:
    #
    #    genericHub.eventOut -> eventManager.eventIn
    #    genericHub.tlmOut -> tlmDb.eventIn
    #
    #    genericHub.serialOut[0] -> valueConsumer0.valueIn[0]
    #    genericHub.serialOut[1] -> valueConsumer1.valueIn[1]
    #
    #    genericHub.bufferOut[0] -> bufferConsumer0.bufferIn
    #    bufferConsumer0.bufferInReturn -> genericHub.bufferOutReturn[0]
    #    genericHub.bufferOut[1] -> bufferConsumer1.bufferIn
    #    bufferConsumer1.bufferInReturn -> genericHub.bufferOutReturn[1]
    # ----------------------------------------------------------------------

    @ Port for receiving events
    @ Data emitted on this port is copied from a buffer received on fromBufferDriver,
    @ and the buffer is returned.
    output port eventOut:  Fw.Log

    @ Port for receiving telemetry channels
    @ Data emitted on this port is copied from a buffer received on fromBufferDriver,
    @ and the buffer is returned.
    output port tlmOut: Fw.Tlm

    @ Ports for receiving serial data
    @ You can connect each of these output ports to any typed input port.
    @ Data emitted on one of these ports is copied from a buffer received on fromBufferDriver,
    @ and the buffer is returned.
    output port serialOut: [GenericHubCfg.NumSerialOutputPorts] serial

    @ Ports for receiving buffer data
    @ A buffer emitted on one of these ports is a buffer received on fromBufferDriver,
    @ With adjusted metadata to point to the data stored in the buffer.
    output port bufferOut: [GenericHubCfg.NumBufferOutputPorts] Fw.BufferSend

    @ Ports for receiving buffers sent on bufferOut and then returned
    sync input port bufferOutReturn: [GenericHubCfg.NumBufferOutputPorts] Fw.BufferSend

    @ bufferOut and bufferOutReturn ports must match
    match bufferOut with bufferOutReturn

  }

}
