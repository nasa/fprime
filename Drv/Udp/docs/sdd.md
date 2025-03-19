# Drv::Udp UDP Component

The UDP client component bridges the byte stream driver model interface to a remote UDP port to which this udp component
sends/receives bytes. It implements the callback formation (shown below) using a thread to receive data and producing
the callback port call.  Udp does not require bidirectional operation and can be used in a single direction.

For more information on the supporting UDP implementation see: Drv::UdpSocket.
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

This data is immediately sent out to the remote UDP server with a configured send timeout. See Usage described below.

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

The Drv::UdpComponentImpl must be configured with the address(es) of the remote entity. Users can configure send and
receive independently using `configureSend` and `configureRecv`.  The sockets must also be opened to send and receive
data using the `open` call. When the component is set to automatically open, `open` is called via the first send or
receive call. Users declining to use automatic opening or who wish to control when open initially happens should call
`open` before sending or receiving.

Automatic opening is the default.  Call `setAutomaticOpen(false);` to disable this behavior.

Users desiring to receive via UDP should start the receive thread using `start`, may stop the thread using `stop` and may
wait for the thread to exit using `join`.

Since UDP support single or bidirectional communication, configuring each direction is done separately using the two
methods `configureSend` and `configureRecv`. The user must call at least one of the configure methods and may call both.

```c++
Drv::UdpComponentImpl comm = Drv::UdpComponentImpl("UDP Client");

bool constructApp(bool dump, U32 port_number, char* hostname) {
    ...
    comm.configureSend(hostname, port_number);
    comm.configureRecv(hostname, port_number);
    ...
    if (hostname != nullptr && port_number != 0) {
        Os::TaskString name("ReceiveTask");
        // Needed for receiving only, remove if not configuring to receive
        comm.startSocketTask(name);
    }
}

void exitTasks() {
    ...
    comm.stop();
    (void) comm.join();
}
```
## Class Diagram

![classdiagram](./img/class_diagram_udp.png)

## Requirements

| Name | Description | Validation |
|---|---|---|
| UDP-COMP-001 | The udp component shall implement the ByteStreamDriverModel  | inspection |
| UDP-COMP-002 | The udp component shall provide a read thread | unit test |
| UDP-COMP-003 | The udp component shall provide single and bidirectional communication across udp | unit test |

