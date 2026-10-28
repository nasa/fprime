# Drv::ByteStreamDriverModel Byte Stream Driver Model

The byte stream driver is a generic model for drivers implementing a "stream of bytes" interface. Typically these drivers operate with an outgoing stream and an incoming stream.  
The outgoing stream is represented by the input `send` port; other components can invoke this port to send data through the driver. The incoming stream is represented by the output `recv` port; the driver will call this port to send data to the component that is receiving data from the driver.

## Design

There are two versions for the ByteStreamDriver, a synchronous version (`Drv.ByteStreamDriver`) and an asynchronous version (`Drv.AsyncByteStreamDriver`). In the synchronous version, the (guarded) `send` port blocks and returns status. In the asynchronous version, the (async) `send` port calls back on the `sendReturnOut` port to return status and buffer ownership.

### Send

The manager component (for example a radio manager) initiates the transfer of send data by calling the "send" port. The caller will provide a `Fw::Buffer` containing the data to send. 

1. Async case: The driver component **must** perform a callback on its `sendReturnOut` port to return the status of that send as well as returning ownership of the `Fw::Buffer` to the caller.
2. Sync case: The driver component **must** return a status of the send operation and ownership of the `Fw::Buffer` to the caller.

These responses are an enumeration whose values are described in the following table:

| Value | Description | Buffer Ownership |
|---|---|---|
| ByteStreamStatus::OP_OK    | Send functioned normally. | Ownership of the `Fw::Buffer` passes to the byte stream driver. |
| ByteStreamStatus::SEND_RETRY | Send should be retried, but a subsequent send should return OP_OK. | The caller retains ownership of the `Fw::Buffer`. |
| ByteStreamStatus::OTHER_ERROR | Send produced an error, future sends likely to fail. Some clients stop sending until the driver invokes `ready` again (see [Ready](#ready)). | Ownership of the `Fw::Buffer` passes to the byte stream driver. |

The ownership column describes the asynchronous interface. With the synchronous `send` port the caller retains ownership of the `Fw::Buffer` whatever the status (see [Design](#design)), so a synchronous driver must not keep using the buffer after returning.

### Receive

The byte stream driver component initiates the transfer of received data by calling the "recv" output port. This port transfers any read data in a `Fw::Buffer` along with a status for the receive.
This status is an enumeration whose values are described in the following table:

| Value | Description |
|---|---|
| ByteStreamStatus::OP_OK    | Receive functioned normally and buffer contains valid data. |
| ByteStreamStatus::RECV_NO_DATA    | Receive worked, but there was no data  |
| ByteStreamStatus::OTHER_ERROR | Receive produced an error and buffer contains no valid data. |

### Ready

The driver invokes its `ready` output port (`Drv.ByteStreamReady`) when it is able to send and receive data: on connection or reconnection, and again whenever sending becomes possible after it has answered sends with `ByteStreamStatus::OTHER_ERROR` or with a run of `SEND_RETRY`. A client may have stopped sending on either status and waits for `ready`; a redundant `ready` is harmless.

Clients may rely on this signal to resume sending after a failure. [`Svc::ComStub`](../../../Svc/ComStub/docs/sdd.md) does: when a send is answered with `ByteStreamStatus::OTHER_ERROR`, or `SEND_RETRY` is still returned after its retry limit, it reports `Fw::Success::FAILURE`, which pauses [`Svc::ComQueue`](../../../Svc/ComQueue/docs/sdd.md). It emits the recovery `Fw::Success::SUCCESS` that resumes the queue when the driver next invokes `ready` (see the [Communication Adapter Interface](../../../docs/reference/communication-adapter-interface.md)). A driver used behind `Svc::ComStub` that returns `OTHER_ERROR` must therefore invoke `ready` again once it can send, or traffic from `Svc::ComQueue` stays paused. The send that was answered with `OTHER_ERROR` is not retransmitted by `Svc::ComStub`: that frame is dropped.

`Svc::ComStub` retries `SEND_RETRY` immediately, with no delay, and only a bounded number of times (`RETRY_LIMIT` = 10: at most 10 sends on the synchronous path, 1 + 10 on the asynchronous path), so that status suits momentary conditions; when the limit is exhausted the frame is dropped and `Svc::ComQueue` is paused until the driver next invokes `ready`, exactly as for `OTHER_ERROR`. The driver is not told when that happens, which is why it should invoke `ready` whenever sending becomes possible again after a run of `SEND_RETRY`. A driver that cannot accept data for a longer time, for example because of limited bandwidth, can instead return `OTHER_ERROR` and invoke `ready` when it is able to send again, at the cost of that one frame. A driver that must not lose frames should buffer the data internally and drain it from its own task, rather than use `OTHER_ERROR` as back-pressure; it should not block inside its guarded `send`, because the guard mutex is shared with `recvReturnIn` and a long block there stalls the receive path as well.

> **Note:** the stock drivers listed below invoke `ready` only when they (re)connect. A send error that does not drop the connection therefore leaves `Svc::ComQueue` paused until the next reconnection.

The following components implement the byte stream model using the synchronous interface:

- [`Drv::TcpClient`](../../TcpClient/docs/sdd.md): a F´ component wrapper of the tcp client
- [`Drv::TcpServer`](../../TcpServer/docs/sdd.md): a F´ component wrapper of the tcp server
- [`Drv::Udp`](../../Udp/docs/sdd.md): a F´ component wrapper of the udp
- [`Drv::PosixUartDriver`](../../PosixUartDriver/docs/sdd.md): a F´ component wrapper of the POSIX UART driver


## Requirements

| Name | Description | Validation |
|---|---|---|
| BYTEDRV-001 | The ByteStreamDriverModel shall provide the capability to send bytes | inspection |
| BYTEDRV-002 | The ByteStreamDriverModel shall provide the capability to produce bytes | inspection |
