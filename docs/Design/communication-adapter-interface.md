# Communication Adapter Interface

Any communication component (e.g. a radio component) that is intended for use with the standard F´ uplink and downlink
stack should implement the *Communication Adapter Interface*. This interface specifies both the ports and protocols used
to operate with the standard F´ uplink and downlink components.

Implementors of this interface are referred to as *Communication Adapters*.

![Communication Adapter Interface](./img/com-adapter.png)

The communication adapter interface protocol is designed to work alongside the framer status protocol and the com queue
protocol to ensure that data messages do not overload a communication interface. These protocols are discussed below.

## Ports

The communication adapter interface is composed of three ports. These ports are used to transmit outgoing data through
some communication hardware and receive incoming data from that same hardware. These ports share types with the byte
stream driver model for backwards compatibility.

| Kind     | Suggested Name | Port Type             | Usage                                                          |
|----------|----------------|-----------------------|----------------------------------------------------------------|
| `input`  | `comDataIn`    | `Drv.ByteStreamSend`  | Port receiving `Fw::Buffer` objects for outgoing transmission. |
| `output` | `comDataOut`   | `Drv.ByteStreamRecv`  | Port producing incoming `Fw::Buffer` objects.                  |
| `output` | `comStatus`    | `Fw.SuccessCondition` | Port indicating status of outgoing transmission. See protocol. |


> Note: components implementing the *Communication Adapter Interface* must deallocate any `Fw::Buffer` received on the
> `comDataIn` port or must delegate the deallocation to another component (e.g. a driver).
> See [Memory Management in F´](./memory.md)

### comDataIn Description

This port receives data from an F´ application in the form of an argument of `Fw::Buffer` type. This data is intended to
be sent out the communications interface managed by this component. From the perspective of the application this is
the outgoing data port.

### comDataOut Description

This port receives data from the communication interface managed by this component and provides it to the F´ application
in the form of an argument of `Fw::Buffer` type. From the perspective of the application this is the incoming data port.

### comStatus Description

This port carries a status of `Fw::Success::SUCCESS` or `Fw::Success::FAILURE` typically in response to a call to the
`comDataIn` port described above. 

> Note: it is critical to obey the protocol as described in the protocol section below.

## Communication Queue Protocol

`Svc::ComQueue` queues messages until the communication adapter is ready to receive these messages. For each
Fw::Success::SUCCESS message received, `Svc::ComQueue` will emit one message. `Svc::ComQueue` will not emit messages
at any other time. This implies several things:

1. An initial `Fw::Success::SUCCESS` message must be sent to `Svc::ComQueue` to initiate data flow
2. A `Fw::Success::SUCCESS` must be eventually received in response to each message for data to continue to flow

These implications are discussed in more depth in the appropriate protocol sections.

## Framer Status Protocol

Framing typically happens between `Svc::ComQueue` and a communications adapter. The `Svc::Framer` component implements
this protocol in order to interoperate with these other components. The action taken by this protocol is dependent on
the number of framed messages (frames) sent to the communication adapter in response to each message received from
`Svc::ComQueue`.

| Produced Frames | Action                                 | Rationale                                                |
|-----------------|----------------------------------------|----------------------------------------------------------|
| 0               | Send one `Fw::Success::SUCCESS` status | Data must continue to flow to produce a frame            |
| 1               | Pass through received statuses         | Frame produced and communication adapter produces status |

When a frame is produced the communication adapter is responsible for determining the status of the message and its
statuses must be passed through. When no frame is produced, the communication adapter does not receive a frame and is
incapable of producing a status. `Svc::Framer` must act appropriately in this case.

> Note: `Svc::Framer` must also pass through the initiation message from the communication adapter to start data flow.

## Communication Adapter Protocol

The communication adapter must obey a specific protocol to indicate to F´ application components the status of outgoing
transmissions. This is done with the `comStatus` port. A communication status is one of two possibilities:

| Status               | Description                                                                       |
|----------------------|-----------------------------------------------------------------------------------|
| Fw::Success::SUCCESS | *Communication adapter* transmission succeeded  and is ready for more data.       |
| Fw::Success::FAILURE | Last transmission failed; *communication adapter* is unable to receive more data. |

* Fw::Success::SUCCESS may also indicate a connection/reconnection success when data flow must be initiated.

A *Communication Adapter* shall emit either Fw::Success::SUCCESS or Fw::Success::FAILURE via the `comStatus` port once
for each call received on `comDataIn`. Additionally, a *Communication Adapter* shall emit Fw::Success::SUCCESS once at
startup to indicate communication is initially ready and once after each Fw::Success::FAILURE event to indicate that
communication has been restored. By emitting Fw::Success::SUCCESS after any failure, the communication adapter ensures
that each received message eventually results in a Fw::Success::SUCCESS.

> It is imperative that *Communication Adapters* implement the `comStatus` protocol correctly.
