\page SvcDeframer Deframer Component
# Svc::Deframer Component

## 1. Introduction

`Svc::Deframer` takes a stream of bytes from the ground data system,
checks the header and footer, and 
then extracts message data from the stream. The extracted data usually is 
passed to the service layer (typically
[`Svc::CommandDispatcher`](../../CmdDispatcher/docs/sdd.md),
[`Svc::FileUplink`](../../FileUplink/docs/sdd.md),
or [`Svc::GenericHub`](../../GenericHub/docs/sdd.md)). 

Deframer uses the interface provided by
[`Svc::DeframingProtocol`](../../DeframingProtocol/docs/sdd.md).
By instantiating `Svc::Framer` with a matching implementation of
`Svc::FramingProtocol`, you will get matching framing and deframing.

The Deframing Protocol implementation is backed by a circular buffer
that accumulates streaming data.
Each phase of deframing proceeds when enough data has accumulated
to provide the input for that phase.

Deframer is typically connected to a
[byte stream driver](../../../Drv/ByteStreamDriverModel).
It supports two configurations for streaming data:

1. **Poll:** This configuration supports drivers that lack their own threads.
   In this configuration Deframer polls the driver for data.

2. **Push:** This configuration supports drivers that have their own threads.
   In this configuration the driver pushes data to the Deframer.

## 2. Assumptions

1. If you use `Deframer` together with `Svc::Framer`, then the deframing
and framing protocols used to instantiate the components (a)
match each other and (b) match the protocol used by the ground data system.

1. You should use only one data streaming configuration (poll or push)
in any FSW topology that uses `Svc::Framer` and `Svc::Deframer`.

## 3. Requirements

Requirement | Description | Rationale | Verification Method
----------- | ----------- | ----------| -------------------
| TBD | TBD | TBD | TBD |

## 4. Design
 
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
