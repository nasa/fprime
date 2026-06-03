# Communication Adapter Interface

Any communication component (e.g. a radio component) that is intended for use with the standard F´ uplink and downlink
stack should implement the *Communication Adapter Interface* [`Com.fpp`](../../Svc/Interfaces/Com.fpp). This interface specifies both the ports and protocols used
to operate with the standard F´ uplink and downlink components.

Implementors of this interface are referred to as *Communication Adapters*.

![Communication Adapter Interface](../img/com-adapter.png)

The communication adapter interface protocol is designed to work alongside the framer status protocol and the com queue
protocol to ensure that data messages do not overload a communication interface. These protocols are discussed below.

## Usage

Components implementing the *Communication Adapter Interface* should include the following in the FPP model:

```
... component ... {
    import Svc.Com
}
```

This can be seen in [`Svc.ComStub`](https://github.com/nasa/fprime/blob/fff0e0bf77d8e03b2005e02e5095571a20048f4a/Svc/ComStub/ComStub.fpp#L6).

## Ports

The `Svc.ComDataWithContext` port type is used to transmit comms data between components of the F´ system. This port type is composed of a `Fw::Buffer` object and a `ComCfg::FrameContext` object. The `Fw::Buffer` object is used to transmit the data, while the `ComCfg::FrameContext` object is used to provide context for the data being transmitted, such as header information once the header has been consumed.

The communication adapter interface is composed of five ports. These ports are used to transmit outgoing data through
some communication hardware and receive incoming data from that same hardware. These ports share types with the byte
stream driver model for backwards compatibility.

| Kind     | Suggested Name | Port Type                | Description                                                    |
|----------|----------------|--------------------------|----------------------------------------------------------------|
| `input`  | `dataIn`       | `Svc.ComDataWithContext` | Port receiving data for outgoing transmission (to be sent on the wire) |
| `output` | `dataOut`      | `Svc.ComDataWithContext` | Port producing data from incoming transmissions (received on the wire) |
| `output` | `dataReturnOut`| `Svc.ComDataWithContext` | Port for sending back ownership of the `Fw::Buffer` received on the `dataIn` port |
| `input`  | `dataReturnIn` | `Svc.ComDataWithContext` | Port for receiving back ownership of the `Fw::Buffer` sent on the `dataOut` port |
| `output` | `comStatusOut` | `Fw.SuccessCondition`    | Port indicating status of outgoing transmission. See protocol. |


> [!NOTE]
> About buffer management: after receiving a buffer on the `dataIn` port, the communication component must return ownership of said buffer through the `dataReturnOut` port. The common scenario is to connect `dataIn` and `dataReturnOut` to the same component, so that the sender can handle deallocation. This is done with a callback so that `dataIn` can be an asynchronous port if needed.
> Similarly, the buffer sent out on `dataOut` is expected to be returned through the `dataReturnIn` port.

### dataIn Description

This port receives data from an F´ application in the form of an argument of `Fw::Buffer` type and a `ComCfg::FrameContext` context. This data is intended to be sent out the communications interface managed by this component (often referred to as _the wire_). From the perspective of the application this is the _outgoing data_ port.

### dataOut Description

This port produces data to the F´ application once it is received on the wire. From the perspective of the application this is the _incoming data_ port.

### dataReturnOut Description

This port is used to send a callback returning ownership of the `Fw::Buffer` object that was received on the `dataIn` port. Ownership is typically returned to the sender. A callback is used so that `dataIn` may be an asynchronous port if needed.

### dataReturnIn Description

This port is used to receive a callback returning ownership of the `Fw::Buffer` object that was sent on the `dataOut` port. Ownership is typically returned from the receiver of dataOut. A callback is used so that `dataOut` may be an asynchronous call if needed.

### comStatusOut Description

This port carries a status of `Fw::Success::SUCCESS` or `Fw::Success::FAILURE` typically in response to a call to the `dataIn` port described above. 

> [!NOTE]
> It is critical to obey the protocol as described in the protocol section below.

> [!CAUTION]
> Calls to `comStatusOut` must happen after calls to `dataReturnOut` returning the data the com status applies to.

## Communication Queue Protocol

`Svc::ComQueue` queues messages until the communication adapter is ready to receive these messages. For each `Fw::Success::SUCCESS` message received, `Svc::ComQueue` will emit exactly one message. `Svc::ComQueue` will not emit messages at any other time. `Fw::Success::FAILURE` pauses the queue; data flow resumes only when a subsequent `Fw::Success::SUCCESS` is received.

The following rules govern the `comStatusIn` port of `Svc::ComQueue`:

1. An initial `Fw::Success::SUCCESS` message must be sent to `Svc::ComQueue` to initiate data flow (typically on start-up or driver connection)
2. Exactly one `Fw::Success::SUCCESS` must eventually be received in response to each emitted message for data to continue to flow
3. `Fw::Success::FAILURE` pauses the queue — no messages will be emitted until a subsequent `Fw::Success::SUCCESS` is received
4. After a `Fw::Success::FAILURE`, a `Fw::Success::SUCCESS` must be sent to resume data flow (this SUCCESS does not correspond to any particular message — it indicates that the communication path has been restored)

> [!IMPORTANT]
> `Fw::Success::SUCCESS` is valid in three contexts: (1) at start-up to initiate data flow, (2) in response to a successfully transmitted message, and (3) after a previous `Fw::Success::FAILURE` to indicate recovery. No other uses of `Fw::Success::SUCCESS` are permitted.

These implications are discussed in more depth in the appropriate protocol sections.

## Framer Status Protocol

Framing typically happens between `Svc::ComQueue` and a communications adapter. The action taken by this protocol is dependent on the number of framed messages (frames) sent to the communication adapter in response to each message received from `Svc::ComQueue`.

The `Svc::FprimeFramer` implements the Framer status protocol by emitting a frame for each message received from `Svc::ComQueue`. Therefore, the `Svc::FprimeFramer` receives a status from the communication adapter on each sent frame and forwards it back to `Svc::ComQueue`. This allows the data flow to continue, or pause if the communication adapter is unable to receive more data.

Framer implementations may choose to implement a different behavior. For example, a framer may choose to accept multiple messages from `Svc::ComQueue` and concatenate them in a single frame. In this case, the framer implementation must manage the flow of statuses accordingly. This is summarized in the table below.

| Produced Frames | Action                                 | Rationale                                                |
|-----------------|----------------------------------------|----------------------------------------------------------|
| 0               | Send one `Fw::Success::SUCCESS` status | Data must continue to flow to produce a frame            |
| 1               | Pass through received status from ComQueue | Frame produced and communication adapter produces status |

> [!IMPORTANT]
> All framer implementations must pass through the initiation message from the communication adapter to start data flow.

## Communication Adapter Protocol

The communication adapter must obey a specific protocol to indicate to F´ application components the status of outgoing
transmissions. This is done with the `comStatusOut` port. A communication status is one of two possibilities:

| Status               | Description                                                                       |
|----------------------|-----------------------------------------------------------------------------------|
| Fw::Success::SUCCESS | *Communication adapter* transmission succeeded and is ready for more data.       |
| Fw::Success::FAILURE | Last transmission failed; *communication adapter* is unable to receive more data. |

A *Communication Adapter* shall emit `comStatusOut` signals according to the following rules:

1. **On start-up / connection:** The adapter shall emit exactly one `Fw::Success::SUCCESS` to initiate data flow. This typically occurs when the underlying driver signals readiness (e.g. via `drvConnected`). This initial SUCCESS is not in response to any data — it tells `Svc::ComQueue` that the adapter is ready to accept its first message.

2. **In response to data received on `dataIn`:** The adapter shall emit exactly one status (`Fw::Success::SUCCESS` or `Fw::Success::FAILURE`) for each call received on `dataIn`.
   - `Fw::Success::SUCCESS` indicates the transmission succeeded and the adapter is ready for the next message.
   - `Fw::Success::FAILURE` indicates the transmission failed and the adapter is temporarily unable to receive more data.

3. **After a previous `Fw::Success::FAILURE`:** The adapter shall emit exactly one `Fw::Success::SUCCESS` once communication has been restored. This recovery SUCCESS is not in response to any data — it tells `Svc::ComQueue` to resume sending. The adapter is responsible for any retransmission of the failed data before emitting this recovery signal.

> [!IMPORTANT]
> `Fw::Success::SUCCESS` is valid in exactly three contexts: (1) at start-up to initiate data flow, (2) in response to a successful transmission of data received on `dataIn`, and (3) after a previous `Fw::Success::FAILURE` to indicate recovery. Each received message on `dataIn` must eventually result in a `Fw::Success::SUCCESS` — either directly on success, or via the failure-then-recovery sequence.

Since the communication status reflects the status of a specific data transmission, it must be sent after the return (deallocation) of that data via the `dataReturnOut` port.

> [!CAUTION]
> Calls to `comStatusOut` must happen after calls to `dataReturnOut` returning the data the status applies to.

> [!NOTE]
> It is imperative that *Communication Adapters* implement the `comStatusOut` protocol correctly.
