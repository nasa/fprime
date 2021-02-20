\page SvcFramingProtocol FramingProtocol
# Svc::FramingProtocol FramingProtocol Component

FramingProtocol is used to build the messages for Framer and Deframer. It splits out the serialization from the topology hookup allowing the same system component connections to support different byte protocols "on-the-wire". F´ is shipped with a default "F´ protocol" which works with F´ Test Gds and amounts to: startword, data, CRC32.

# Usage

If a project needs to implement a custom Framing and Deframing the following functions need to be implemented:

FramingProtocol Interface:
```c++
    virtual Fw::Buffer allocate(const U32 size) = 0;

    virtual Fw::Time time() = 0;

    virtual void send(Fw::Buffer& outgoing) = 0;
```

FramingProtocol Interface:
```c++
    virtual Fw::Buffer allocate(const U32 size) = 0;

    virtual void route(Fw::Buffer& data) = 0;
```

Diagram view of FramingProtocol:

![FramingProtocol Impl Diagram](./img/framingProtocol_impl_diagram.png)

Diagram view of DeframingProtocol:

![DeframingProtocol Impl Diagram](./img/deframingProtocol_impl_diagram.png)

*Diagrams generated with [SourceTrail](https://github.com/CoatiSoftware/Sourcetrail)*

## Change Log

| Date | Description |
|---|---|
| 2021-01-30 | Initial Draft |
