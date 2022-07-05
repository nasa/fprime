\page SvcFramerComponent Svc::Framer Component
# Svc::Framer (Passive Component)

## 1. Introduction

`Svc::Framer` is a passive component.
It accepts data packets from the service layer components, typically 
instances of [`Svc::TlmChan`](../../TlmChan/docs/sdd.md),
[`Svc::ActiveLogger`](../../ActiveLogger/docs/sdd.md),
or [`Svc::FileDownlink`](../../FileDownlink/docs/sdd.md).
For each packet received, it wraps the packet in a frame
and sends the frame to a component instance in the driver layer
that performs downlink,
for example, [`Drv::TcpClient`](../../Drv/TcpClient/docs/sdd.md)).

When instantiating Framer, you must provide an implementation
of [`Svc::FramingProtocol`](../../FramingProtocol/docs/sdd.md).
This implementation specifies exactly what is
in each frame; typically it is a frame header, a data packet, and a hash value.

## 2. Assumptions

1. For any deployment _D_ that uses an instance _I_ of `Framer`, the
   deframing protocol used with _I_ matches the downlink protocol of
   any ground system that receives frames from _I_.

## 3. Requirements

TODO

## 4. Design

### 4.1. Component Diagram

The diagram below shows the `Framer` component.

<div>
<img src="img/Framer.png" width=700/>
</div>

### 4.2. Ports

| Kind | Name | Port Type | Usage |
|------|------|-----------|-------|
| `guarded input` | `comIn` | `Fw.Com` | Port for receiving data packets stored in statically-sized Com buffers |
| `guarded input` | `bufferIn` | `Fw.BufferSend` | Port for receiving data packets stored in dynamically-sized managed bufers |
| `output` | `bufferDeallocate` | `Fw.BufferSend` | Port for deallocating buffers received on bufferIn, after copying packet data to the frame buffer |
| `output` | `framedAllocate` | `Fw.BufferGet` | Port for allocating buffers to hold framed data |
| `output` | `framedOut` | `Drv.ByteStreamSend` | Port for sending buffers containing framed data. Ownership of the buffer passes to the receiver. |

### 4.3. State

TODO

### 4.4. Header File Configuration

TODO

### 4.5. Runtime Setup

TODO

### 4.6. Port Handlers

TODO

### 4.7. Helper Functions

TODO

## 5. Ground Interface

TODO

## 6. Example Uses

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

## 7. Class Diagram

![classdiagram](./img/class_diagram_framer.png)

## 8. Requirements

| Name | Description | Validation |
|---|---|---|
| TBD | TBD | TBD |

## 9. Change Log

| Date | Description |
|---|---|
| 2021-01-29 | Initial Draft |
