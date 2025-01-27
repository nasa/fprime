# Drv::TcpServer Tcp Server Component

The TCP server component bridges the byte stream driver model interface to a remote TCP client to which this tcp server
connects and sends/receives bytes. It implements the callback formation (shown below) using a thread to receive data
and producing the callback port call. Since it is a server, it must startup and listen for client connections. Designed
for single client communication, it does not permit a queue of connecting clients.

For more information on the supporting TCP implementation see: [Drv::TcpServerSocket](../../Ip/docs/sdd.md#drvtcpserversocket-class).
For more information on the ByteStreamModelDriver see: [Drv::ByteStreamDriverModel](../..//ByteStreamDriverModel/docs/sdd.md).

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

The Drv::TcpClientComponentImpl must be configured with the address of the remote connection using the `configure` method.
The `configure` method will also start the TCP server listening for remote connections. However, clients will not be accepted
until the connection is opened. Sockets are opened using `open`. When the component is set to automatically open,
`open` is called done via the first send or receive. Users declining to use automatic opening or who wish to control when
open initially happens should call `open` before any send or receive.  

Automatic opening is the default.  Call `setAutomaticOpen(false);` to disable this behavior.

Users desiring to receive via TCP should start the receive thread using `start`, may stop the thread using `stop` and may
wait for the thread to exit using `join`.

The Drv::TcpServerComponentImpl must be configured with the address of the remote connection, and the socket must be
open to begin. When the user calls `configure` the listening port of the TCP server is automatically connected and begins
listening for incoming connections.

When configured to automatically connect as is the default, the single queued client will be accepted on the first send
data call, or when the receive thread is started. Usually, the user runs the Drv::TcpServerComponentImpl's read thread
with automatic connection enabled, which will automatically accept the client connection.

`configure` must be called before any data is sent or received. `start` must be called for the component to receive data.


Users should call `stop` to stop the receive task and `join` to wait for it to exit.

Users turning off automatic connection must call `open` to open the connection.

```c++
Drv::TcpServerComponentImpl comm = Drv::TcpServerComponentImpl("TCP Server");

bool constructApp(bool dump, U32 port_number, char* hostname) {
    ... configure 
    comm.configure(hostname, port_number);
    ...
    Os::TaskString name("ReceiveTask");
    comm.start(name);
}

void exitTasks() {
    ...
    comm.stop();
    (void) comm.join();
}
```
## Class Diagram
![class diagram](./img/class_diagram_tcpserver.png)

## Requirements

| Name | Description | Validation |
|---|---|---|
| TCP-SERVER-COMP-001 | The tcp server component shall implement the ByteStreamDriverModel  | inspection |
| TCP-SERVER-COMP-002 | The tcp server component shall provide a read thread | unit test |
| TCP-SERVER-COMP-003 | The tcp server component shall provide bidirectional communication with a tcp client | unit test |
