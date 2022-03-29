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
   In this configuration, `Deframer` polls the driver for buffers
   on its `schedIn` cycle.
   No buffer allocation occurs when polling.
   The polling uses a 1024-byte buffer owned by `Deframer`.

2. **Push:** This configuration works with an active byte stream driver.
   In this configuration the driver pushes buffers to the Deframer.
   The Deframer takes ownership of each buffer _B_ that it receives.
   It deallocates _B_ when it is finished processing the data in _B_.

## 2. Assumptions

1. If you use `Deframer` together with `Svc::Framer`, then the deframing
protocol used with `Deframer` (a)
must match the framing protocol used with `Framer` and (b) must match
the protocol used by the ground data system.

1. In any topology that uses an instance _I_ of `Deframer`, you must connect
the poll interface of _I_ or the push interface of _I_, but not both.

## 3. Requirements

Requirement | Description | Rationale | Verification Method
----------- | ----------- | ----------| -------------------
SVC-DEFRAMER-001 | `Svc::Deframer` shall accept a sequence of byte buffers and interpret their concatenated data as a sequence of uplink frames. | The purpose of the component is to do uplink deframing. | Test
SVC-DEFRAMER-002 | `Svc::Deframer` shall accept byte buffers containing uplink frames that are not aligned on a buffer boundary. | For flexibility, we do not require that the frames be aligned on a buffer boundary. | Test
SVC-DEFRAMER-003 | `Svc::Deframer` shall accept byte buffers containing uplink frames that span one or more buffers. | For flexibility, we do not require each frame to fit in a single buffer. | Test
SVC-DEFRAMER-004 | `Svc::Deframer` shall provide a port interface for pushing the byte buffers to be deframed. | This interface supports applications in which the byte stream driver has its own thread. | Test
SVC-DEFRAMER-005 | `Svc::Deframer` shall provide a port interface for polling for byte buffers to be deframed. | This interface supports the applications in which that byte stream driver does not have its own thread. | Test
SVC-DEFRAMER-006 | If the polling interface is connected, then `Svc::Deframer` shall poll for byte buffers on its `schedIn` port. | This requirement allows the system scheduler to drive the periodic polling. | Test
SVC-DEFRAMER-007 | `Svc::Deframer` shall use an instance of `Svc::DeframingProtocol`, supplied when the component is instantiated, to validate the frames and extract their packet data. | Using the `Svc::DeframingProtocol` interface provides flexibility and ensures that the deframing protocol matches the framing protocol. | Test
SVC-DEFRAMER-008 | `Svc::Deframer` shall interpret the first four bytes of the packet data as a 32-bit signed integer holding the packet type. | 32 bits should be enough to hold any packet type. Fixing the size at 32 bits keeps the implementation simple. | Test
SVC-DEFRAMER-009 | `Svc::Deframer` shall extract and send packets with the following types: `Fw::ComPacket::FW_PACKET_COMMAND`, `Fw::ComPacket::FW_PACKET_FILE`. | These are the packet types used for uplink. | Test
SVC-DEFRAMER-010 | `Svc::Deframer` shall send command packets and file packets on separate ports. | Command packets and file packets are typically handled by different components. | Test
SVC-DEFRAMER-011 | `Svc::Deframer` shall operate nominally when its port for sending file packets is unconnected, even if it receives a frame containing a file packet. | Some applications do not use file uplink. Sending a file uplink packet to `Deframer` should not crash the application because of an unconnected port. | Test

## 4. Design
 
### 4.1. Component Diagram

The diagram below shows the `Deframer` component.

<div>
<img src="img/Deframer.png" width=700/>
</div>

### 4.2. Ports

`Deframer` has the following ports:

| Kind | Name | Port Type | Usage |
|------|------|-----------|-------|
| `guarded input` | `framedIn` | `Drv.ByteStreamRecv` | Port for receiving data pushed from the byte stream driver. After using a buffer received on this port, Deframer deallocates it by invoking framedDeallocate. _TBD: Why is this port guarded? By assumption framedIn and framedPoll should never both be connected._ |
| `output` | `framedDeallocate` | `Fw.BufferSend` | Port for deallocating buffers received on framedIn. |
| `guarded input` | `schedIn` | `Svc.Sched` | Schedule in port, driven by a rate group. _TBD: Why is this port guarded? By assumption framedIn and framedPoll should never both be connected._ |
| `output` | `framedPoll` | `Drv.ByteStreamPoll` | Port that polls for data from the byte stream driver. Deframer invokes this port on its schedIn cycle, if it is connected. No allocation or occurs when invoking this port. The data transfer uses a 1024-byte pre-allocated buffer owned by Deframer. |
| `output` | `bufferAllocate` | `Fw.BufferGet` | Port for allocating Fw::Buffer objects from a buffer manager. When Deframer invokes this port, it receives a buffer B and takes ownership of it. It uses B internally for deframing. Then one of two things happens:  1. B contains a file packet, which Deframer sends on bufferOut. In this case ownership of B passes to the receiver.  2. B does not contain a file packet, or bufferOut is unconnected. In this case Deframer deallocates B on bufferDeallocate. |
| `output` | `bufferOut` | `Fw.BufferSend` | Port for sending file packets (case 1 above). The file packets are wrapped in Fw::Buffer objects allocated with bufferAllocate. Ownership of the Fw::Buffer passes to the receiver, which is responsible for the deallocation. |
| `output` | `bufferDeallocate` | `Fw.BufferSend` | Port for deallocating temporary buffers allocated with bufferAllocate (case 2 above). Deallocation occurs here when there is nothing to send on bufferOut. |
| `output` | `comOut` | `Fw.Com` | Port for sending command packets as Com buffers to the command dispatcher. |
| `sync input` | `cmdResponseIn` | `Fw.CmdResponse` | Port for receiving command responses from the command dispatcher. Invoking this port does nothing. The port exists to allow the matching connection in the topology. |

### 4.3. State

`Deframer` maintains the following state:

1. `m_protocol`: A pointer to the implementation of `DeframingProtocol`
   used for deframing.

1. `m_in_ring`: An instance of `Types::CircularBuffer` for storing data to be deframed.

1. `m_ring_buffer`: The storage backing the circular buffer: an array of 1024 `U8` values.

1. `m_poll_buffer`: The buffer used for polling input: an array of 1024 `U8` values.

### 4.4. Port Handlers

#### 4.4.1. framedIn

TODO

#### 4.4.2. schedIn

TODO

#### 4.4.3. cmdResponseIn

TODO

### 4.5. [Previous SDD]]

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
