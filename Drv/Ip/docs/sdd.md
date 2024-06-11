\page DrvIp Drv::Ip IPv4 Socket Implementations
# Drv::Ip IPv4 Socket Implementations

This package contains utility classes to help interact with standard IPv4 (Berkeley) sockets. These classes implement the
core features of IPv4. This includes a tcp server socket (Drv::TcpServerSocket), a tcp client socket (Drv::TcpClient)
and a udp socket (Drv::UdpSocket).  These are not F´ components, but an F´ component wrapper of each exists:
Drv::TcpClientComponent,  Drv::TcpServerComponent, and Drv::UdpComponent.

In addition to each individual socket there is a Drv::IpSocket base class containing high-level functions for
interacting with sockets generically. Drv::IpSocket delegates protocol specific functions to protocol functions
implemented by the children concrete socket classes. i.e. Drv::IpSocket::open delegates to the functions
Drv::TcpClientSocket::openProtocol to open up specifically a tcp client socket.

Drv::SocketReadTask is a virtual base class that comes with the functionality for setting up a generic reading thread
complete with the ability to reconnect to a closed/broken connection. This virtual base class is intended to be
inherited by an F´ component wrapper that need to support a receive thread such that this functionality need not be
redundantly implemented.

Each of these classes is explained in more detail below.

- [Drv::IpSocket](#drvipsocket-baseclass)
- [Drv::TcpClientSocket](#drvtcpclientsocket-class)
- [Drv::TcpServerSocket](#drvtcpserversocket-class)
- [Drv::UdpSocket](#drvudpsocket-class)
- [Drv::SocketReadTask](#drvsocketreadtask-virtual-baseclass)

## Drv::IpSocket Baseclass

The Drv::IpSocket class represents the external interface to IPv4 socket components. This class provides a top-level
interface to IPv4 sockets for connecting, disconnecting, sending, and receiving  using the socket. This implements the
IPv4 socket protocol as provided by the unix (Berkeley) sockets implementation.

High-level interfaces are provided for the standard functions of: `open`, `close`, `send`, and `recv`.  FramingProtocol and
implementation specific functionality are implemented in derived classes by implementing the pure virtual functions:
`openProtocol`, `sendProtocol`, and `recvProtocol`.

In order to use a Drv::IpSocket it must first be configured with a call to the `Drv::IpSocket::configure` function.
Once configured, it can be opened using `Drv::IpSocket::open`. Opening the socket will verify arguments, allocate
system resources and form a connection.  In server implementations (Drv::TcpServerSocket) the open call will block until
a client connection has been made.  It is safe to assume that a successfully opened socket is ready to send or receive,
however; those calls may detect an error and close the socket in response.

`Drv::TcpServerSocket::send` will attempt to send data across the socket. It will retry to transmit data a configured
number of times on correctable errors before finally succeeding once all data has been transmitted or failing should the
socket send fail. Interrupts and timeouts are the only recoverable errors. Other problems result in an error status and
when a remote disconnect is detected `Drv::IpSocket::close` is closed to ensure the socket is ready for a subsequent
call to `Drv::IpSocket::open`.

`Drv::TcpServerSocket::recv` will attempt to read data from across the socket. It will block until data is received and
in the case that the socket is interrupted without data, it will retry a configurable number of times. Other errors will
result in an error status with a specific `Drv::IpSocket::close` call issued in the case of detected disconnects.

A call to `Drv::IpSocket::close` will specifically shutdown and close the client connection. This has the effect of
stopping any blocking reads on the socket, issuing a formal disconnect, and cleaning up the allocated resources. Once
called, the Drv::IpSocket should be ready for another call to `Drv::IpSocket::open`.

### Example Usage of Drv::IpSocket

This section will show some example usages of the Drv::IpSocket. In this section it is assumed the initialization is
done using a concrete derived class as shown in subsequent sections.

```c++
Drv::IpSocket& socket = ...; // Initialize the component
socket.configure("127.0.0.1", 60210, 0, 100); // Use 127.0.0.1:60210 with a 100us send timeout
...
socket.open();

while (chatting) {
    socket.send(); // Timeout of 100us
    socket.recv(); // Will block
}
socket.close();
socket.open();
...
```

## Drv::TcpClientSocket Class

The Drv::TcpClientSocket class represents an IPv4 TCP client. It inherently provides bidirectional communication with
a remote server tcp server. TCP communications acknowledge transmissions so the remote server will have received a
message or this client will return an error. In other words, dropped packets will cause an error. Usage is as stipulated
above and an example instantiation is shown below.

```c++
Drv::IpSocket& socket = Drv::TcpClientSocket;
```

## Drv::TcpServerSocket Class

The Drv::TcpServerSocket class represents an IPv4 TCP server. It inherently provides bidirectional communication with
a remote tcp client server. The TCP server must be started up such that it may listen for incoming client requests.
Since this class is intended to communicate with exactly one client, no listen queue is provided and subsequent connects
from clients will be ignored until the primary client has been closed. Like the TCP client packet drops will result in an
error.

**Note:** the `Drv::TcpServerSocket::open` call will block until a client connects to the server.

In order to startup the server to listen, the `Drv::TcpServerSocket::startup` method should be called. It will create a
socket that will listen for incoming connections.  `Drv::TcpServerSocket::startup` should be called before any
`Drv::TcpServerSocket::open` calls to ensure that the server is ready for communication.  `Drv::TcpServerSocket::close`
will only close the client connection and does not affect the server from listening for clients, however; it does free
up the server to accept a new client.

`Drv::TcpServerSocket::shutdown` will close the TCP server from receiving any new clients and effectively releases all
resources allocated to the server. `Drv::TcpServerSocket::shutdown` implies `Drv::TcpServerSocket::close` and client
connections will be stopped.

## Example TcpServer Usage

```c++
Drv::TcpServerSocket& server = Drv::; // Initialize the component
server.configure("127.0.0.1", 60210, 0, 100); // Use 127.0.0.1:60210 with a 100us send timeout
...
server.startup(); // Listen on 127.0.0.1:60210
server.open(); // Blocks until a client is available

while (chatting) {
    server.send(); // Timeout of 100us
    server.recv(); // Will block
}
server.close();
server.open(); // Blocks until a client is available
...
server.shutdown();
```

## Drv::UdpSocket Class

The Drv::UdpSocket class represents an IPv4 UDP sender/receiver. Drv::UdpSocket will provide unidirectional or
bidirectional communication using UDP. UDP is typically faster than TCP as it does not acknowledge transmissions. There
is no guarantee that a sent packet is received, or even that the remote side is listening.

A udp socket must be configured for each direction that it will communicate in. This can be done using calls to
`Drv::UdpSocket::configureSend` and `Drv::UdpSocket::configureRecv`.  If either call is omitted only a single direction
of communication will function. It is erroneous to omit both configuration calls.  Calling `Drv::UdpSocket::configure`
is equivalent to  calling `Drv::UdpSocket::configureSend` for compatibility with `Drv::IpSocket`.  Other interaction
with the UDP socket is as stipulated with `Drv::IpSocket`.  Examples of instantiation and configuration are provided
below.

```c++
Drv::UdpSocket& socketSend = Drv::UdpSocket;
socketSend.configureSend(127.0.0.1, 60211, 0, 100); // Send only socket
...

Drv::UdpSocket& socketRecv = Drv::UdpSocket;
socketRecv.configureRecv(127.0.0.1, 60212); // Receive only socket
...

Drv::UdpSocket& socketBoth = Drv::UdpSocket;
socketBoth.configureSend(127.0.0.1, 60211, 0, 100);
socketBoth.configureRecv(127.0.0.1, 60212);
...
```

## Drv::SocketReadTask Virtual Baseclass

The Drv::SocketReadTask is intended as a base class used to add in the functionality of an automatically reconnecting
receive thread to another class (typically an F´ component). In order for this thread to function, the inheritor must
implement several methods to provide the necessary interaction of this thread. These functions are described in the next
section.

In order to start the receiving thread a call to the `Drv::SocketReadTask::startSocketTask` method is performed passing
in a name, and all arguments to `Os::Task::start` to start the task. An optional parameter, reconnect, will determine if
this read task will reconnect to sockets should a disconnect or error occur. Once started the read task will continue
until a `Drv::SocketReadTask::stopSocketTask` has been called or an error occurred when started without reconnect set to
`true`.  Once the socket stop call has been made, the user should call `Drv::SocketReadTask::joinSocketTask` in order to
wait until the full task has finished.  `Drv::SocketReadTask::stopSocketTask` will call `Drv::IpSocket::close` on the
provided Drv::IpSocket to ensure that any blocking reads exit freeing the thread to completely stop. Normal usage of
a Drv::SocketReadTask derived class is shown below.

```c++
Os::TaskString name("ReceiveTask");
uplinkComm.startSocketTask(name); // Default reconnect=true
...

uplinkComm.stopSocketTask();
(void) uplinkComm.joinSocketTask(nullptr);
```

`Drv::SocketReadTask::open` and `Drv::SocketReadTask::close` convenience methods are also provided to open and close the
provided Drv::IpSocket, although it should be noted that both are called automatically during the normal process of the
receive thread.


### Drv::SocketReadTask Inheritance

Drv::SocketReadTask is used via inheritance. A class that needs to provide receive thread functionality may inherit from
this class and implement the three virtual methods to integrate with the thread. These methods are described below and
each provides its prototype.

`Drv::SocketReadTask::getSocketHandler` returns a reference to an existing Drv::IpSocket for this task to interact with
for receiving, sending, opening, and closing. This should not be allocated within the function call as it will be used
outside the call. The instance is expected to have been configured and started (in the case of servers). The prototype
of the function is shown below.

```c++
virtual IpSocket& getSocketHandler() = 0;
```

`Drv::SocketReadTask::getBuffer` returns an Fw::Buffer instance that wraps data for the read call to fill. This can wrap
stack memory, or delegate the call to a buffer manager instance.  This buffer will be returned via a call to
`Drv::SocketReadTask::sendBuffer` once the received data has filled it.  The prototype of the function is shown below.

```c++
virtual Fw::Buffer getBuffer() = 0;
```

`Drv::SocketReadTask::sendBuffer` returns the buffer obtained from `Drv::SocketReadTask::getBuffer` after it has been
filled with data read from the socket. This will come with a status associated with the read but will always be called
such that the allocated buffer can be freed when needed.

```c++
virtual void sendBuffer(Fw::Buffer buffer, SocketIpStatus status) = 0;
```

## Further Information

Further information can be read by referencing the following components.

`Drv::TcpClientComponent`: a F´ component wrapper of the tcp client
`Drv::TcpServerComponent`: a F´ component wrapper of the tcp server
`Drv::UdpComponent`: a F´ component wrapper of the udp
