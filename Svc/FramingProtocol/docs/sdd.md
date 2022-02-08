\page SvcFramingProtocol FramingProtocol
# Svc::FramingProtocol Library

The `FramingProtocol` library defines the interface to the
framing and deframing
protocols used by the `Svc::Framer` and `Svc::Deframer` components.
`Svc::Framer` uses a framing protocol to wrap data in framed packets
for transmission to the ground.
`Svc::Deframer` uses a deframing protocol to extract
data from framed packets received from the ground.
The `FramingProtocol` library allows these components to operate with multiple 
protocols.
Each protocol corresponds to a different implementation
of the interface.

This library implements a default F' protocol that works with
the F' Ground Data System (GDS).
The F' protocol uses the following format for both framing and
deframing: four-byte start word `0xDEADBEEF`, data, four-byte Cyclic Redundancy Check
(CRC) value.
Users may provide new protocols by implementing the abstract classes
defined in this library.

## 1. Using the Interface

### 1.1. Framing

To use the F' framing protocol, do the following:

1. Instantiate the class `FPrimeFraming` defined in this library.

1. Instantiate the `Svc::Framer` component, passing the instance
created in step 1 to its `setup` method.

For an example, see the definition of the instance `downlink`
at `Ref/Top/instances.fpp`.

To implement and use a new framing protocol, do the following:

1. Implement the abstract class `FramingProtocolInterface`
as discussed in Section 2.1.
This class defines helper operations used when framing a packet.

1. Implement the abstract class `FramingProtocol` as discussed
in Section 2.1.
This class defines the operation of framing a data packet.

1. Instantiate the class implemented in step 1.

1. Instantiate the class implemented in step 2, passing 
the instance created in step 3 to its `setup` method.

1. Instantiate the `Svc::Framer` component, passing the instance created 
in step 4 to its `setup` method.

### 1.2. Deframing

To use the F' deframing protocol, do the following:

1. Instantiate the class `FPrimeDeframing` defined in this library.

1. Instantiate the `Svc::Deframer` component, passing the instance
created in step 1 to its `setup` method.

For an example, see the definition of the instance `uplink`
at `Ref/Top/instances.fpp`.

To implement and use a new deframing protocol, do the following:

1. Implement the abstract class `DeframingProtocolInterface`
as discussed in Section 2.2.
This class defines helper operations used when deframing a framed
packet.

1. Implement the abstract class `DeframingProtocol` as discussed
in Section 2.2.
This class defines the operation of deframing a framed packet.

1. Instantiate the class implemented in step 1.

1. Instantiate the class implemented in step 2, passing 
the instance created in step 3 to its `setup` method.

1. Instantiate the `Svc::Deframer` component, passing the instance created 
in step 4 to its `setup` method.

## 2. Implementing a Protocol

### 2.1. Framing

To implement a framing protocol, do the following:

1. Implement the abstract class `FramingProtocolInterface`.

1. Use the implementation in step 1 to implement the abstract class
`FramingProtocol`.

#### 2.1.1. Implementing `FramingProtocolInterface`

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

The method `send` should send the data stored in the buffer.
A typical implementation invokes an `Fw::BufferSend` port.

#### 2.1.2. Implementing `FramingProtocol`

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

The abstract class `FramingProtocol` provides a protected member `m_interface`.
This member is a pointer, initially null.
After the `setup` method of `FramingProtocol` is called, it
points to a concrete instance of `FramingProtocolInterface`.

Your implementation of `frame` should do the following:

1. Use `m_interface->allocate` to allocate a buffer to hold the framed data.

1. Frame the data into the buffer allocated in step 1.

1. Use `m_interface->send` to send the buffer.

### 2.2. Deframing

To implement a deframing protocol, do the following:

1. Implement the abstract class `DeframingProtocolInterface`.

1. Use the implementation in step 1 to implement the abstract class
`DeframingProtocol`.

#### 2.2.1. Implementing `DeframingProtocolInterface`

`DeframingProtocolInterface` defines helper methods for framing data.
Typically these methods are implemented by an F Prime component (e.g., `Svc::Deframer`),
because they require port invocations.
The component `Svc::Deframer` provides an implementation of `DeframingProtocolInterface`
that you can use.
It does this by inheriting from `DeframingProtocolInterface` and implementing
its abstract methods.

To implement `DeframingProtocolInterface`, you must implement the following
pure virtual methods:

```c++
virtual Fw::Buffer allocate(const U32 size) = 0;

virtual void route(Fw::Buffer& data) = 0;
```

The method `allocate` should allocate memory, as described in
Section 2.1.1.

The method `route` should send (route) the data stored in the buffer.
A typical implementation invokes either an `Fw::Com` port (e.g., for sending
commands) or a `Fw::BufferSend` port (e.g., for sending file packets).

#### 2.2.2. Implementing `DeframingProtocol`

`DeframingProtocol` defines the operation of deframing a packet.
To implement `DeframingProtocol`, you must implement the following pure
virtual method:


```c++
virtual DeframingStatus deframe(Types::CircularBuffer& buffer, U32& needed) = 0;
```

This method is called with the following arguments:

* `buffer`: A circular buffer holding the data to deframe.

* `needed`: A reference for returning the number of bytes needed
for deframing.

`deframe` returns a value of type `DeframingStatus` indicating what happened.

The abstract class `DeframingProtocol` provides a protected member `m_interface`.
It operates as described in Section 2.1.2.

Your implementation of `deframe` should do the following:

1. Use `m_interface->allocate` to allocate a buffer to hold the deframed data.

1. Peek into the circular buffer and determine how many bytes are needed
for deframing.
If that many bytes are available, deframe the data into the buffer allocated in step 1.
Otherwise set status indicating invalid size.

1. Use `m_interface->route` to send the buffer.

1. Return status.

## 3. Default F' Implementation

### 3.1. Framing

TODO

### 3.2. Deframing

TODO

## 4. Class Diagrams

![FramingProtocol Impl Diagram](./img/framingProtocol_impl_diagram.png)

Diagram view of DeframingProtocol:

![DeframingProtocol Impl Diagram](./img/deframingProtocol_impl_diagram.png)

*Diagrams generated with [SourceTrail](https://github.com/CoatiSoftware/Sourcetrail)*

## 5. Change Log

| Date | Description |
|---|---|
| 2021-01-30 | Initial Draft |
| 2021-02-07 | Revised |
