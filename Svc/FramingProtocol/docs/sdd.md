\page SvcFramingProtocol FramingProtocol
# Svc::FramingProtocol Library

The `FramingProtocol` library defines the interface to the
framing and deframing
protocols used by the `Svc::Framer` and `Svc::Deframer` components.
`Svc::Framer` uses a framing protocol to wrap data in framed packets
for transmission to the ground.
`Svc::Deframer` uses a deframing protocol to extract
data from framed packets received from the ground.
The `FramingProtocol` library allows the same framer and deframer
components to operate with multiple protocols.
Each protocol corresponds to a different implementation
of the interface.

This library implements a default F' protocol that works with
the F' Ground Data System (GDS).
The F' protocol uses the following format for both framing and
deframing: four-byte start word `0xDEADBEEF`, data, four-byte Cyclic Redundancy Check
(CRC) value.
Users may provide new protocols by implementing the abstract classes
defined in this library.

# Using the Interface

## Framing

To use the F' framing protocol, do the following:

1. Instantiate the class `FPrimeFraming` defined in this library.

1. Instantiate the `Svc::Framer` component, passing the instance
created in step 1 to its `setup` method.

For an example, see the definition of the instance `downlink`
at `Ref/Top/instances.fpp`.

To implement and use a new framing protocol, do the following:

1. Implement the abstract class `FramingProtocolInterface`
as discussed in the next section.
This class defines helper operations used when framing a packet.

1. Implement the abstract class `FramingProtocol` as discussed
in the next section.
This class defines the operation of framing a data packet.

1. Instantiate the class implemented in step 1.

1. Instantiate the class implemented in step 2, passing 
the instance created in step 3 to its `setup` method.

1. Instantiate the `Svc::Framer` component, passing the instance created 
in step 4 to its `setup` method.

## Deframing

To use the F' deframing protocol, do the following:

1. Instantiate the class `FPrimeDeframing` defined in this library.

1. Instantiate the `Svc::Deframer` component, passing the instance
created in step 1 to its `setup` method.

For an example, see the definition of the instance `uplink`
at `Ref/Top/instances.fpp`.

To implement and use a new deframing protocol, do the following:

1. Implement the abstract class `DeframingProtocolInterface`
as discussed in the next section.
This class defines helper operations used when deframing a framed
packet.

1. Implement the abstract class `DeframingProtocol` as discussed
in the next section.
This class defines the operation of deframing a framed packet.

1. Instantiate the class implemented in step 1.

1. Instantiate the class implemented in step 2, passing 
the instance created in step 3 to its `setup` method.

1. Instantiate the `Svc::Deframer` component, passing the instance created 
in step 4 to its `setup` method.

# Implementing a Protocol

## Framing

To implement a framing protocol, do the following:

1. Implement the abstract class `FramingProtocolInterface`.

1. Use the implementation in step 1 to implement the abstract class
`FramingProtocol`.

### Implementing `FramingProtocolInterface`

`FramingProtocolInterface` defines helper methods for framing data.
Typically these methods are implemented by an F Prime component (e.g., `Svc::Framer`),
because they require port invocations.
The component `Svc::Framer` provides an implementation of `FramingProtocolInterface`
that you can use.
It does this by inheriting from `FramingProtocolInterface` and implementing
its abstract methods.

To implement `FramingProtocolInterface`, you must implement the following
pure virtual methods:

```c++
    virtual Fw::Buffer allocate(const U32 size) = 0;

    virtual void send(Fw::Buffer& outgoing) = 0;
```

The method `allocate` should accept a size in bytes and return (1) an `Fw::Buffer`
that points to a memory allocation of at least that size if the allocation
succeeded; or (2) an invalid buffer (i.e., a buffer whose address is zero)
if the allocation failed.
A typical implementation invokes a port connected to a memory allocation component.

The method `send` should send a buffer.
A typical implementation invokes a `BufferSend` port.

### Implementing `FramingProtocol`

`FramingProtocol` defines the operation of framing a packet.
To implement `FramingProtocol`, you must implement the following pure
virtual method:

```c++
virtual void frame(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type) = 0;
```

This method is called with the following arguments:

* `data`: A pointer to the data to frame.

* `size`: The number of bytes to frame.

* `packet_type`: The type of data to frame.

The absract class `FramingProtocol` provides a protected member `m_interface`.
This member is a pointer, initially null.
After the `setup` method of `FramingProtocol` is called, it
and points to a concrete instance of `FramingProtocolInterface`.

When implementing the `frame` method, you should do the following:

1. Use `m_interface->allocate` to allocate a buffer to hold the framed data.

1. Frame the data into the buffer allocated in step 1.

1. Use `m_interface->send` to send the buffer.

## Deframing

TODO

# Default F' Implementation

## Framing

TODO

## Deframing

TODO

# Usage


DeframingProtocol:
```c++
virtual DeframingStatus deframe(
    Types::CircularBuffer& buffer,
    U32& needed
) = 0;
```

DeframingProtocol Interface:
```c++
    virtual Fw::Buffer allocate(const U32 size) = 0;

    virtual void route(Fw::Buffer& data) = 0;
```

## Class Diagrams

![FramingProtocol Impl Diagram](./img/framingProtocol_impl_diagram.png)

Diagram view of DeframingProtocol:

![DeframingProtocol Impl Diagram](./img/deframingProtocol_impl_diagram.png)

*Diagrams generated with [SourceTrail](https://github.com/CoatiSoftware/Sourcetrail)*

## Change Log

| Date | Description |
|---|---|
| 2021-01-30 | Initial Draft |
| 2021-02-07 | Revised |
