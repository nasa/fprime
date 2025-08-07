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
| ByteStreamStatus::OTHER_ERROR | Send produced an error, future sends likely to fail. | Ownership of the `Fw::Buffer` passes to the byte stream driver. |

### Receive

The byte stream driver component initiates the transfer of received data by calling the "recv" output port. This port transfers any read data in a `Fw::Buffer` along with a status for the receive.
This status is an enumeration whose values are described in the following table:

| Value | Description |
|---|---|
| ByteStreamStatus::OP_OK    | Receive functioned normally and buffer contains valid data. |
| ByteStreamStatus::RECV_NO_DATA    | Receive worked, but there was no data  |
| ByteStreamStatus::OTHER_ERROR | Receive produced an error and buffer contains no valid data. |

The following components implement the byte stream model using the synchronous interface:
- [`Drv::TcpClient`](../../TcpClient/docs/sdd.md): a F´ component wrapper of the tcp client
- [`Drv::TcpServer`](../../TcpServer/docs/sdd.md): a F´ component wrapper of the tcp server
- [`Drv::Udp`](../../Udp/docs/sdd.md): a F´ component wrapper of the udp
- `Drv::LinuxUartDriver`


## Requirements

| Name | Description | Validation |
|---|---|---|
| BYTEDRV-001 | The ByteStreamDriverModel shall provide the capability to send bytes | inspection |
| BYTEDRV-002 | The ByteStreamDriverModel shall provide the capability to produce bytes | inspection |
