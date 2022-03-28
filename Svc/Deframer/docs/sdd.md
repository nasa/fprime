\page SvcDeframer Deframer Component

# Svc::Deframer (Passive Component)

## 1. Introduction

`Svc::Deframer` is a passive component.
It accepts as input a sequence of byte buffers, which
typically come from a ground data system via a
[byte stream driver](../../../Drv/ByteStreamDriverModel/docs/sdd.md).
It interprets the concatenated data of the buffers
as a sequence of uplink frames.
The uplink frames are not required to be aligned on the
buffer boundaries, and each frame may span one or more buffers.
For each complete frame _F_ received, `Deframer`
validates _F_ and extracts a data packet from _F_.
It sends the data packet to another component in the service layer, e.g., 
an instance of [`Svc::CommandDispatcher`](../../CmdDispatcher/docs/sdd.md),
[`Svc::FileUplink`](../../FileUplink/docs/sdd.md),
or [`Svc::GenericHub`](../../GenericHub/docs/sdd.md).

When instantiating Deframer, you must provide an implementation
of the [`Svc::DeframingProtocol` interface](../../FramingProtocol/docs/sdd.md).
By instantiating `Svc::Framer` with a matching implementation of
`Svc::FramingProtocol`, you will get matching framing (for downlink)
and deframing (for uplink).
The implementation of the deframing protocol specifies exactly what is
in each frame; typically it is a frame header, a data packet, and a hash value.

On receiving a buffer _B_, `Deframer` (1) copies the data from _B_
into a circular buffer _CB_ owned by `Deframer` and (2)
calls the `deframe` method of the `Svc::DeframingProtocol` implementation,
passing a reference to _CB_ as input.
If _B_ holds more data than will fit in _CB_,
then `Deframer` repeats this process until _B_ is empty.
If deframing requires more data than is available in _B_,
then `Deframer` defers deframing until the next buffer is available.

Deframer supports two configurations for streaming data:

1. **Poll:** This configuration works with a passive byte stream driver.
   In this configuration, `Deframer` periodically polls the driver for buffers.
   If the polling succeeds (so a buffer _B_ is available), then `Deframer`
   owns _B_ for the rest of the cycle.

2. **Push:** This configuration works with an active byte stream driver.
   In this configuration the driver pushes buffers to the Deframer.
   The Deframer permanently owns each buffer _B_
   that it receives, and it deallocates _B_ when it is finished processing
   the data in _B_.

## 2. Assumptions

1. If you use `Deframer` together with `Svc::Framer`, then the deframing
protocol used with `Deframer` (a)
matches the framing protocol used with `Framer` and (b) match the protocol
used by the ground data system.

1. You should use only one data streaming configuration (poll or push)
with each instance of `Deframer`.

## 3. Requirements

Requirement | Description | Rationale | Verification Method
----------- | ----------- | ----------| -------------------
SVC-DEFRAMER-001 | `Svc::Deframer` shall accept a sequence of byte buffers and interpret their concatenated data as a sequence of uplink frames. | The purpose of the component is to do uplink deframing. | Test
SVC-DEFRAMER-002 | `Svc::Deframer` shall accept byte buffers containing uplink frames that are not aligned on a buffer boundary. | For flexibility, we do not require that the frames be aligned on a buffer boundary. | Test
SVC-DEFRAMER-003 | `Svc::Deframer` shall accept byte buffers containing uplink frames that span one or more buffers. | For flexibility, we do not require each frame to fit in a single buffer. | Test
SVC-DEFRAMER-004 | `Svc::Deframer` shall provide a port interface for pushing the byte buffers to be deframed. | This interface supports applications in which the byte stream driver has its own thread. | Test
SVC-DEFRAMER-005 | `Svc::Deframer` shall provide a port interface for polling for available byte buffers. | This interface supports the applications in which that byte stream driver does not have its own thread. | Test
SVC-DEFRAMER-006 | `Svc::Deframer` shall use an instance of `Svc::DeframingProtocol`, supplied when the component is instantiated, to validate the frames and extract their data. | Using the `Svc::DeframingProtocol` interface provides flexibility and ensures that the deframing protocol matches the framing protocol. | Test
SVC-DEFRAMER-007 | `Svc::Deframer` shall interpret the first four bytes of the extracted data as a 32-bit signed integer holding the packet type. | 32 bits should be enough to hold any packet type. Fixing the size at 32 bits keeps the implementation simple. | Test
SVC-DEFRAMER-008 | `Svc::Deframer` shall extract and send packets with the following types: `Fw::ComPacket::FW_PACKET_COMMAND`, `Fw::ComPacket::FW_PACKET_FILE`. | These are the packet types used for uplink. | Test
SVC-DEFRAMER-009 | `Svc::Deframer` shall send command packets and file packets on separate ports. | Command packets and file packets are typically handled by different components. | Test
SVC-DEFRAMER-009 | `Svc::Deframer` shall operate nominally when its port for sending file packets is unconnected, even if it receives a frame containing a file packet. | Some applications do not use file uplink. Sending a file uplink packet to `Deframer` should not crash the application because of an unconnected port. | Test

## 4. Design
 
### 4.1. Component Diagram

The diagram below shows the `Deframer` component.

<div>
<img src="img/Deframer.png" width=500/>
</div>

### 4.2. Ports

`Deframer` has the following ports:

| Kind | Name | Port Type | Usage |
|-----------|------|------|-------|

### 4.3. TODO

1. Deframer will accept incoming buffers.

1. Upon buffer receipt, it will delegate processing to a `DeframingInstance`.

    1. If that delegation returns an error, it will discard the first byte and keep processing.

    1. If that delegation returns need more status, it will accumulate more 
       buffers until it has the size specified, and then rerun the processing.

    1. If that delegation returns success, it will discard `size` bytes and 
       start at the next message.

1. When a `route` call is called-back to the Deframer, it will send the message 
   to the `Fw::Com` output port of the `Fw::Buffer` output port based on the 
   specified type in the route call.

1. If an allocate callback is made, it will delegate that allocation to the 
   bufferAllocate call.

1. If step three is routing to `Fw::Com`, it must pass the buffer to the 
   `bufferDeallocate` port.

## 5. Usage Examples

When Deframer component receives the data from a manager component (typically a 
service layer such as ground uplinkComm or TcpClient) via polling or a 
callback, the Deframer will perform the deserialization per `DeframingProtocol` 
and will transfer the stream via `bufferOut` or `comOut` port to components 
such as `CmdDisp`, `FileUplink`, or a `GenericHub`.

The following diagram is an example of Deframer usage with and `uplinkComm` component:

![framer_example](./img/deframer_example_1.png)

The following diagram is an example of Deframer usage with a `TcpClient` and 
`GenericHub` component:

![framer_example](./img/deframer_example_2.png)

The following is a typical Deframing setup:

```c++
Svc::FprimeDeframing deframing_obj; // Deframing protocol obj;
Svc::DeframerComponentImpl uplink_obj("Deframer"); // Deframer obj

uplink_obj.init(0);
uplink_obj.setup(deframing_obj);

```

## 6. Class Diagram

![classdiagram](./img/class_diagram_deframer.png)

## 7. Change Log

| Date | Description |
|---|---|
| 2021-01-30 | Initial Draft |
| 2022-03-28 | Revised |
