\page SvcFramerComponent Svc::Framer Component
# Svc::Framer (Passive Component)

## 1. Introduction

`Svc::Framer` is a passive component.
It is part of the F Prime data downlink path.
It accepts data packets from service layer components, typically
instances of [`Svc::TlmChan`](../../TlmChan/docs/sdd.md),
[`Svc::ActiveLogger`](../../ActiveLogger/docs/sdd.md),
or [`Svc::FileDownlink`](../../FileDownlink/docs/sdd.md).
For each packet _P_ received, it wraps _P_ in a frame _F_
and sends _F_ to a component instance that downlinks frames,
for example, [`Drv::TcpClient`](../../../Drv/TcpClient/docs/sdd.md).

When instantiating Framer, you must provide an implementation
of [`Svc::FramingProtocol`](../../FramingProtocol/docs/sdd.md).
This implementation specifies exactly what is
in each frame; typically it is a frame header, a data packet, and a hash value.

## 2. Assumptions

1. For any deployment _D_ that uses an instance _I_ of `Framer`, the
   deframing protocol used with _I_ matches the downlink protocol of
   any ground system that receives frames from _I_.

## 3. Requirements

Requirement | Description | Rationale | Verification Method
----------- | ----------- | ----------| -------------------
SVC-FRAMER-001 | `Svc::Framer` shall accept data packets stored in `Fw::Com` buffers or in `Fw::Buffer` objects. | The `Svc` components use both kinds of buffers. `Fw::Com` buffers do not require a buffer manager instance, have a fixed size, and are usually stored on the stack. `Fw::Buffer` objects require a buffer manager, are dynamically sized, and are usually stored on the heap. | Unit test
SVC-FRAMER-002 | `Svc::Framer` shall use an instance of `Svc::FramingProtocol`, supplied when the component is instantiated, to wrap packets in frames. | The purpose of `Svc::Framer` is to frame data packets. Using the `Svc::FramingProtocol` interface allows the same Framer component to operate with different protocols. | Unit test

## 4. Design

### 4.1. Component Diagram

The diagram below shows the `Framer` component.

<div>
<center>
<img src="img/Framer.png" width=700/>
</center>
</div>

### 4.2. Ports

| Kind | Name | Port Type | Usage |
|------|------|-----------|-------|
| `guarded input` | `comIn` | `Fw.Com` | Port for receiving data packets stored in statically-sized Fw::Com buffers |
| `guarded input` | `bufferIn` | `Fw.BufferSend` | Port for receiving data packets stored in dynamically-sized Fw::Buffer objects |
| `output` | `bufferDeallocate` | `Fw.BufferSend` | Port for deallocating buffers received on bufferIn, after copying packet data to the frame buffer |
| `output` | `framedAllocate` | `Fw.BufferGet` | Port for allocating buffers to hold framed data |
| `output` | `framedOut` | `Drv.ByteStreamSend` | Port for sending buffers containing framed data. Ownership of the buffer passes to the receiver. |

<a name="derived-classes"></a>
### 4.3. Derived Classes

`Framer` is derived from `FramerComponentBase` as usual.
It is also derived (via C++ multiple inheritance) from
[`Svc::FramingProtocolInterface`](../../FramingProtocol/docs/sdd.md).
The multiple inheritance makes the `Deframer` instance into the
instance of `Svc::FramingProtocolInterface` that is required
to use `Svc::FramingProtocol`.
See <a href="#fpi-impl">below</a> for a description of how `Deframer` implements
`DeframingProtocolInterface`.

Here is a class diagram for `Deframer`:

```mermaid
classDiagram
    ObjBase <|-- PassiveComponentBase
    PassiveComponentBase <|-- FramerComponentBase
    FramerComponentBase <|-- Framer
    FramingProtocolInterface <|-- Framer
```

### 4.4. State

TODO

### 4.5. Header File Configuration

TODO

### 4.6. Runtime Setup

TODO

### 4.7. Port Handlers

TODO

<a name="fpi-impl"></a>
### 4.8. Implementation of Svc::DeframingProtocolInterface

### 4.9. Helper Functions

TODO

## 5. Ground Interface

TODO

## 6. Example Uses

### 6.1. Topology Diagrams

When using Framer component, the manager component (typically a service layer
or a generic hub) initiates the transfer of data by calling bufferIn port. The
Framer component will perform the serialization per `FramingProtocol` and will
transfer the stream via bufferOut port.

The following diagram is an example of framer usage with chanTlm and eventLogger:

![framer_example](./img/framer_example_1.png)

The following diagram is an example of framer usage with a generic hub and TcpClient:

![framer_example](./img/framer_example_2.png)

The following is a typical example of Framer usage:

```c++
Svc::FprimeFraming framing_obj; // Framing protocol obj;
Svc::FramerComponentImpl downlink_obj("Framer"); // Framer obj

downlink_obj.init(0);
downlink_obj.setup(framing_obj);

...

Fw::Buffer buf; // This could be data from bufferIn port
downlink_obj.send(buf); // Send framed buffer to a port connected to bufferOut
```

### 6.2. Sequence Diagrams

TODO

## 7. Change Log

| Date | Description |
|---|---|
| 2021-01-29 | Initial Draft |
