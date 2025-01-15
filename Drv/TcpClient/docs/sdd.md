# Drv::TcpClient TCP Client Component

The TCP client component bridges the byte stream driver model interface to a remote TCP server to which this tcp client
connects and sends/receives bytes. It implements the callback formation (shown below) using a thread to receive data
and producing the callback port call.

For more information on the supporting TCP implementation see: Drv::TcpClientSocket.
For more information on the ByteStreamModelDriver see: Drv::ByteStreamDriverModel.

## Design

The manager component (typically the ground interface) initiates the transfer of send data by calling the "send" port.
The caller will provide a `Fw::Buffer` containing the data to send and the port call will return a status of that send.
These responses are an enumeration whose values are described in the following table:

| Value | Description |
|---|---|
| Drv::SEND_OK    | Send functioned normally. |
| Drv::SEND_RETRY | Send should be retried, but a subsequent send should return SEND_OK. |
| Drv::SEND_ERROR | Send produced an error, future sends likely to fail. |

This data is immediately sent out to the remote tcp server with a configured send timeout. See Usage described below.

**Callback Formation**

![Callback](../../ByteStreamDriverModel/docs/img/canvas-callback.png)

In the callback formation, the byte stream driver component initiates the transfer of received data by calling the
"readCallback" output port. This port transfers any read data in a `Fw::Buffer` along with a status for the receive.
This status is an enumeration whose values are described in the following table:

| Value | Description |
|---|---|
| Drv::RECV_OK    | Receive functioned normally buffer contains valid data. |
| Drv::RECV_ERROR | Receive produced an error and buffer contains no valid data. |

## Usage

The Drv::TcpClientComponentImpl must be configured with the address of the remote connection, and the socket must be
open to begin. Usually, the user runs the Drv::TcpClientComponentImpl engaging its read thread, which will automatically
open the  connection. The component is passive and has no commands meaning users should `init`, `configure`, and
`startSocketTask`. Upon shutdown, the `stopSocketThread` and `joinSocketThread` methods should be called to ensure
proper resource deallocation. This typical usage is shown in the C++ snippet below.

```c++
Drv::TcpClientComponentImpl comm = Drv::TcpClientComponentImpl("TCP Client");

bool constructApp(bool dump, U32 port_number, char* hostname) {
    ...
    comm.init(0);
    ...
    if (hostname != nullptr && port_number != 0) {
        Os::TaskString name("ReceiveTask");
        comm.configure(hostname, port_number);
        comm.startSocketTask(name);
    }
}

void exitTasks() {
    ...
    comm.stopSocketTask();
    (void) comm.joinSocketTask(nullptr);
}
```
## Class Diagram
![tcp client](./img/class_diagram_tcp_client.png)

## Requirements

| Name | Description | Validation |
|---|---|---|
| TCP-CLIENT-COMP-001 | The tcp client component shall implement the ByteStreamDriverModel  | inspection |
| TCP-CLIENT-COMP-002 | The tcp client component shall provide a read thread | unit test |
| TCP-CLIENT-COMP-003 | The tcp client component shall provide bidirectional communication with a tcp server | unit test |

## Change Log

| Date | Description |
|---|---|
| 2020-12-17 | Initial Draft |
| 2021-01-28 | Updated |
