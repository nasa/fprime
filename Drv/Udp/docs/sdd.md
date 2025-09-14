# Drv::Udp UDP Component

The UDP client component bridges the byte stream driver model interface to a remote UDP port to which this udp component
sends/receives bytes. It implements the callback formation (shown below) using a thread to receive data and producing
the callback port call.  Udp does not require bidirectional operation and can be used in a single direction.

For more information on the supporting UDP implementation see: Drv::UdpSocket.
For more information on the ByteStreamModelDriver see: Drv::ByteStreamDriverModel.

## Design

The TcpClient component implements the design specified by the [`Drv::ByteStreamDriverModel`](../../ByteStreamDriverModel/docs/sdd.md).

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

### Ephemeral Port Support

The Drv::UdpComponentImpl supports ephemeral ports for receiving data. This is done by setting the port number to 0
when calling `configureRecv`. The port number will be returned when the socket is opened.

When configured as a receiver-only the Drv::UdpComponentImpl can also be set up to send a response back to the sender and use the
response port that the sender has indicated in the UDP datagram. This is done by setting the port number to 0 when calling
`configureSend`.

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

## Requirements

| Name | Description | Validation |
|---|---|---|
| UDP-COMP-001 | The udp component shall implement the ByteStreamDriverModel  | inspection |
| UDP-COMP-002 | The udp component shall provide a read thread | unit test |
| UDP-COMP-003 | The udp component shall provide single and bidirectional communication across udp | unit test |

