# Ground Interface Architecture and Customization

This guide will discuss the F´ ground interface layers and how to customize them. There are two parts to the ground
interface: the spacecraft side, and the ground side. This guide will primarily focus on the spacecraft side adaptation
as the most common pattern is to adapt F´ flight software for some other ground system (e.g.
[Cosmos]( https://github.com/BallAerospace/COSMOS), [OpenMCT](https://nasa.github.io/openmct/), etc). This
document will walk through common adaptations in hopes that projects will not need to replace the ground interface
entirely.

In the most basic form, the F´ ground system pattern consists of two sides: uplink and downlink. These two sides each
have two layers: framing and driver.  Uplink handles data coming from the remote side of the interface, downlink handles
data going to the remote interface, framing handles serializing and deserializing data to and from byte buffers, and the
driver layer handles writing data to and from the hardware.

![Ground Interface Block Diagram](../../img/ground-interface.jpg)


Also of note is the framing protocol, which breaks out the handling of the byte serialization for quick adaptation. Each
of these stages need to allocate memory and thus users should also consult the [buffer pool management](../framework/memory-management/buffer-pool.md)
guide.

> [!NOTE]
> in this guide we will refer to the driver layer but many projects will refer to it as the radio or communication layer. The function of this layer is to read and write bytes to some hardware and the nature of that hardware is irrelevant as long as it can send and receive bytes.

## Ground Interface Architecture

Standard F´ components handle two types of data: com buffers and raw buffers. Com buffers transmit standard F´ items
(e.g. events, telemetry, and commands) whereas raw buffers (`Fw::Buffer`) transmit any raw bytes like file data. Thus
the F´ ground interface must handle both types of data. Communications hardware typically only transmits bytes of data
and knows nothing about the nature of that data. The goal of the ground interfaces is to ensure that the various types
of F´ data can be translated into a sequence of bytes that can be reconstructed on the other side of the interface. This
architecture is described below.

### Driver

Drivers manage hardware communications. These can be simple hardware interfaces (e.g. TCP or UART) or far more complex
constructs (e.g. radios, spacecraft buses). From the perspective of F´, the driver has two functions: provide incoming
data and handle outgoing data.

> [!NOTE]
> typically projects use a single driver to handle both input and output, however; two drivers may be used too if differing behavior is needed for uplink and downlink.(e.g. UDP downlink for speed and  Tcp uplink reliability).

All drivers implement an input port receiving data from the framer. The driver should write input data to the hardware
the driver manages. Drivers implement at least one of two methods to retrieve data from hardware: an input port
to poll for available data and an output read port for asynchronous data, which often is supported by a read thread.
Generic drivers implement both such that they can be used in a threaded context or rate group-driven polling context.
The driver is responsible for reading the data from the hardware in either context.

> [!NOTE]
> the F´ uplink layer is compatible with both polling and receiving drivers as described in **Uplink** below.

**Sending Data**

To send data to a driver, an `Fw::Buffer` is passed to the driver's send input port and the data wrapped by the buffer
will be pushed out to the hardware. Drivers respond to sends with one of the following statuses:

1. SendStatus.OP_OK: indicates the send was successful
2. SendStatus.SEND_RETRY: indicates subsequent retransmission will likely succeed 
3. SendStatus.OTHER_ERROR: send failed, the data was not sent, and future success cannot be predicted

**Polling Data**

Polling for data allows the system to determine when to look for available data. This often means the driver does not
need a thread constantly trying to read data. It is used in rate-group-driven baremetal systems to schedule the
reception of data and remove the need for a task to spin looking for data. To poll data, an `Fw::Buffer` is passed to
the driver's poll input port where the buffer is filled with available data.  Polling returns the following statuses:

1. PollStatus.POLL_OK: indicates the buffer is filled with valid data
2. PollStatus.POLL_RETRY: indicates a subsequent retry of the polling call will likely result in valid data
3. PollStatus.POLL_ERROR: polling failed, the buffer data is invalid, and future success cannot be predicted

**Receiving Data**

Receiving data is to handle asynchronous input of data without the need to poll for it. This typically means the driver
has an internal task that calls the receive output port when data has been received. Receive ports are passed
`Fw::Buffer`s and a receive status as described below. Receive RETRY status is not used as the external system has
nothing to retry.


1. RecvStatus.OP_OK: receive works as expected and the buffer has valid data
2. RecvStatus.OTHER_ERROR: receive failed and the buffer does not have valid data

### Uplink

Uplink handles received data, unpacks F´ data types, and routes these to the greater F´ system. In a typical formation,
these com buffers are sent to the command dispatcher and raw buffers are sent to the file uplink. Uplink is implemented by chaining multiple components:
- a [Svc.FrameAccumulator](../../../Svc/FrameAccumulator/docs/sdd.md) component, accumulating bytes from the driver until it detects a full frame
- a component implementing the [Svc.DeframerInterface](../../../Svc/Interfaces/docs/sdd.md) port interface to unpack the frame into F´ data types. F´ ships with implementations for various protocols:
  - [Svc.FprimeDeframer](../../../Svc/FprimeDeframer/docs/sdd.md) for the lightweight F´ protocol
  - [Svc.Ccsds package](../../../Svc/Ccsds) containing implementations for the CCSDS TC and Space Packet protocols
- a router component implementing the [Svc.RouterInterface](../../../Svc/Interfaces/docs/sdd.md) port interface to route unpacked F´ data types to their destinations (e.g. command dispatcher, file uplink, etc). F´ ships with the [Svc.FprimeRouter](../../../Svc/FprimeRouter/docs/sdd.md) implementation for routing F´ data types

### Downlink

Downlink takes in F´ data and wraps the data with bytes supporting the necessary protocol. This assembled data is then
sent to the driver for handling. Downlink is implemented with a component implementing the [Svc.FramerInterface](../../../Svc/Interfaces/docs/sdd.md) port interface. F´ ships with implementations for two different protocols:
- [Svc.FprimeFramer](../../../Svc/FprimeFramer/docs/sdd.md)
- [Svc.Ccsds Package](../../../Svc/Ccsds) containing implementations for the CCSDS TM and Space Packet protocols

## Adding a Custom Wire Protocol

To add custom protocols (e.g. CCSDS, custom telemetry formats, etc), users should follow the detailed [How-To Implement a Framing Protocol Guide](../../how-to/custom-framing.md)

## Adding a Custom Driver 

To be compatible with this ground interface, a driver must implement the
[byte steam model interface](https://github.com/nasa/fprime/blob/devel/Drv/ByteStreamDriverModel/ByteStreamDriverModel.fpp).
The driver may add any other ports, events, telemetry, or other F´ constructs as needed but it must define the ports as
described in the ByteStreamDriverModel.  These ports are called out in the below FPP snippet.

```fpp
    output port ready: Drv.ByteStreamReady
    guarded input port send: Drv.ByteStreamSend
    
    output port $recv: Drv.ByteStreamRecv
    guarded input port poll: Drv.ByteStreamPoll
```

1. **ready**: (output) drivers call this port without arguments to signal it is ready to receive data via the send port.
2. **send**: (input) clients call this port passing in an `Fw::Buffer` to send data.
3. **recv**: (output) drivers operating in asynchronous mode call this port with a RecvStatus and `Fw::Buffer` to
   provide data.
4. **poll**: (input) drivers operating in poll mode fill an `Fw::Buffer` and return a PollStatus to provide data.
