# Drv::TcpClient TCP Client Component

The TCP client component bridges the byte stream driver model interface to a remote TCP server to which this tcp client
connects and sends/receives bytes. It implements the callback formation (shown below) using a thread to receive data
and producing the callback port call.

For more information on the supporting TCP implementation see: Drv::TcpClientSocket.
For more information on the ByteStreamModelDriver see: [`Drv::ByteStreamDriverModel`](../../ByteStreamDriverModel/docs/sdd.md).

## Design

The TcpClient component implements the design specified by the [`Drv::ByteStreamDriverModel`](../../ByteStreamDriverModel/docs/sdd.md).

## Usage

The Drv::TcpClientComponentImpl must be configured with the address of the remote connection using the `configure` method.
The sockets must also be opened to send and receive data using `open`. When the component is set to automatically open,
`open` is called via the first send or receive. Users declining to use automatic opening or who wish to control when open
initially happens should call `open` before any send or receive. 

Automatic opening is the default.  Call `setAutomaticOpen(false);` to disable this behavior.

Users desiring to receive via TCP should start the receive thread using `start`, may stop the thread using `stop` and may
wait for the thread to exit using `join`.

```c++
Drv::TcpClientComponentImpl comm = Drv::TcpClientComponentImpl("TCP Client");

bool constructApp(bool dump, U32 port_number, char* hostname) {
    ...
    comm.configure(hostname, port_number);
    ...
    if (hostname != nullptr && port_number != 0) {
        Os::TaskString name("ReceiveTask");
        comm.start(name);
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
| TCP-CLIENT-COMP-001 | The tcp client component shall implement the ByteStreamDriverModel  | inspection |
| TCP-CLIENT-COMP-002 | The tcp client component shall provide a read thread | unit test |
| TCP-CLIENT-COMP-003 | The tcp client component shall provide bidirectional communication with a tcp server | unit test |

