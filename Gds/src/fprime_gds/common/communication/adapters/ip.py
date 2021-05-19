"""
ip.py:

Module containing a comm-layer adapter for the Tcp/UDP/IP stack. This pairs with the F prime component "SocketIpDriver"
in order to read data being sent via Tcp or Udp. This is the default adapter used by the system to handle data sent
across a Tcp and/or UDP network interface.

@author lestarch
"""
import atexit
import abc
import logging
import queue
import socket
import threading
import time

import fprime_gds.common.logger
import fprime_gds.common.communication.adapters.base

LOGGER = logging.getLogger("ip_adapter")


def check_port(address, port):
    """
    Checks a given address and port to ensure that it is available. If not available, a ValueError is raised. Note: this
    is done by binding to an address. It does not call "listen"

    :param address: address that will bind to
    :param port: port to bind to
    """
    socket_trial = None
    try:
        socket_trial = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        socket_trial.bind((address, port))
    except OSError as err:
        raise ValueError(
            "Error with address/port of '{}:{}' : {}".format(address, port, err)
        )
    finally:
        if socket_trial is not None:
            socket_trial.close()


class IpAdapter(fprime_gds.common.communication.adapters.base.BaseAdapter):
    """
    Adapts IP traffic for use with the GDS ground system. This serves two different "servers" both on the same address
    and port, but one uses TCP and the other uses UDP. Writes go to the TCP connection, and reads request data from
    both. This data is concatenated and returned up the stack for processing.
    """

    # Interval to send a KEEPALIVE packet. None will turn off KEEPALIVE.
    KEEPALIVE_INTERVAL = 0.500
    # Data to send out as part of the KEEPALIVE packet. Should not be null nor empty.
    KEEPALIVE_DATA = b"sitting well"
    MAXIMUM_DATA_SIZE = 4096

    def __init__(self, address, port):
        """
        Initialize this adapter by creating a handler for UDP and TCP. A thread for the KEEPALIVE application packets
        will be created, if the interval is not none.
        """
        self.stop = False
        self.keepalive = None
        self.tcp = TcpHandler(address, port)
        self.udp = UdpHandler(address, port)
        self.thtcp = None
        self.thudp = None
        self.data_chunks = queue.Queue()
        self.blob = b""

    def open(self):
        """
        Open up the interface to the stored Address and Port. This will create a TCP and UDP socket. The TCP socket will
        be bound to the address, and listened to for incoming connects.
        """
        # Keep alive thread
        try:
            # Setup the tcp and udp adapter and run a thread to service them
            self.thtcp = threading.Thread(target=self.th_handler, args=(self.tcp,))
            self.thtcp.daemon = True
            self.thtcp.start()
            self.thudp = threading.Thread(target=self.th_handler, args=(self.udp,))
            self.thudp.daemon = True
            self.thudp.start()
            # Start up a keep-alive ping if desired. This will hit the TCP uplink, and die if the connection is down
            if IpAdapter.KEEPALIVE_INTERVAL is not None:
                self.keepalive = threading.Thread(
                    target=self.th_alive, args=[float(self.KEEPALIVE_INTERVAL)]
                )
                self.keepalive.start()
        except (ValueError, TypeError) as exc:
            LOGGER.error(
                "Failed to start keep-alive thread. %s: %s"
                % (type(exc).__name__, str(exc))
            )

    def close(self):
        """Close the adapter, by setting the stop flag"""
        self.stop = True
        self.tcp.stop()
        self.udp.stop()

    def th_handler(self, handler):
        """Adapter thread function"""
        handler.open()
        while not self.stop:
            self.data_chunks.put(handler.read())
        handler.close()

    def write(self, frame):
        """
        Send a given framed bit of data by sending it out the serial interface. It will attempt to reconnect if there is
        was a problem previously. This function will return true on success, or false on error.

        :param frame: framed data packet to send out
        :return: True, when data was sent through the UART. False otherwise.
        """
        if self.tcp.connected == IpHandler.CONNECTED:
            return self.tcp.write(frame)

    def read(self, timeout=0.500):
        """
        Read up to a given count in bytes from the TCP adapter. This may return less than the full requested size but
        is expected to return some data.

        :param timeout: timeout to wait for data. Needed as the get call below may not interrupt if it waits forever
        :return: data successfully read or "" when no data available within timeout
        """
        data = b""
        # The read function should block until data is available, but for efficiency, it should read all data available
        # thus the data is read, blocking for 0.5 seconds, and then reads as long as it is not empty.
        try:
            data += self.data_chunks.get(timeout=timeout)
            while not self.data_chunks.empty():
                data += self.data_chunks.get_nowait()
        except queue.Empty:
            pass
        return data

    def th_alive(self, interval):
        """
        Run this thread in order to accept incoming connections and spiral them off into another thread for handling the
        given client.
        """
        while not self.stop:
            self.write(IpAdapter.KEEPALIVE_DATA)
            time.sleep(interval)

    @classmethod
    def get_arguments(cls):
        """
        Returns a dictionary of flag to argparse-argument dictionaries for use with argparse to setup arguments.

        :return: dictionary of flag to argparse arguments for use with argparse
        """
        return {
            ("--ip-address",): {
                "dest": "address",
                "type": str,
                "default": "0.0.0.0",
                "help": "Address of the IP adapter server. Default: %(default)s",
            },
            ("--ip-port",): {
                "dest": "port",
                "type": int,
                "default": 50000,
                "help": "Port of the IP adapter server. Default: %(default)s",
            },
        }

    @classmethod
    def check_arguments(cls, args):
        """
        Code that should check arguments of this adapter. If there is a problem with this code, then a "ValueError"
        should be raised describing the problem with these arguments.

        :param args: arguments as dictionary
        """
        check_port(args["address"], args["port"])


class IpHandler(abc.ABC):
    """
    Base handler for IP types. This will provide the basic methods, and synchronization for reading/writing to multiple
    child implementations, namely: UDP and TCP. These child objects can then be instantiated individually.
    """

    ERROR_RETRY_INTERVAL = (
        1  # Seconds between a non-timeout error and a socket reconnection
    )
    MAX_CLIENT_BACKLOG = 1  # One client backlog, allowing for reconnects
    # Connection states, it will go between these states
    CONNECTING = "CONNECTING"
    CONNECTED = "CONNECTED"
    CLOSED = "CLOSED"

    def __init__(
        self,
        address,
        port,
        adapter_type,
        server=True,
        logger=logging.getLogger("ip_handler"),
        post_connect=None,
    ):
        """
        Initialize this handler. This will set the variables, and start up the internal receive thread.

        :param address: address of the handler
        :param port: port of the handler
        :param adapter_type: type of this adapter. socket.SOCK_STREAM or socket.SOCK_DGRAM
        """
        self.running = True
        self.type = adapter_type
        self.address = address
        self.next_connect = 0
        self.port = port
        self.socket = None
        self.server = server
        self.connected = IpHandler.CLOSED
        self.logger = logger
        self.post_connect = post_connect
        atexit.register(self.stop)

    def open(self):
        """
        Open up this IP type adapter. Returning if already connected.
        """
        if self.CONNECTED == self.connected:
            return True
        # Just continually try and reconnect essentially "blocking" until open. Will only fail if we aborted.
        while self.running and self.CONNECTED != self.connected:
            try:
                # Prevent reconnects when the socket is connected. Socket should be closed on all errors
                if (
                    self.connected == IpHandler.CLOSED
                    and self.next_connect < time.time()
                ):
                    self.connected = IpHandler.CONNECTING
                    self.socket = socket.socket(socket.AF_INET, self.type)
                    if self.server:
                        self.socket.bind((self.address, self.port))
                    else:
                        self.socket.connect((self.address, self.port))
                    self.open_impl()
                    self.connected = IpHandler.CONNECTED
                    self.logger.info(
                        "%s connected to %s:%d",
                        "Server" if self.server else "Client",
                        self.address,
                        self.port,
                    )
                    # Post connect handshake
                    if self.post_connect is not None:
                        self.write(self.post_connect)
            # All errors (timeout included) we should close down the socket, which sets self.connected
            except ConnectionAbortedError:
                return False
            except OSError as exc:
                self.logger.warning(
                    "Failed to open socket at %s:%d, retrying: %s: %s",
                    self.address,
                    self.port,
                    type(exc).__name__,
                    str(exc),
                )
                self.next_connect = time.time() + IpHandler.ERROR_RETRY_INTERVAL
                self.close()
        return self.connected == self.CONNECTED

    @abc.abstractmethod
    def open_impl(self):
        """ Implementation of the handler's open call"""

    def close(self):
        """
        Close this specific IP handler. This involves setting connected to False, and closing non-null sockets.
        """
        try:
            self.close_impl()
            IpHandler.kill_socket(self.socket)
        finally:
            self.socket = None
            self.connected = IpHandler.CLOSED

    @abc.abstractmethod
    def close_impl(self):
        """ Implementation of the handler's close call"""

    def stop(self):
        """ Stop the handler from reconnecting and close"""
        self.running = False
        self.close()

    def read(self):
        """
        Reads a single message after ensuring that the socket is fully open. On a non-timeout error, close the socket in
        preparation for a reconnect. This internally will call the child's read_impl

        :return: data read from TCP server or b"" when nothing is available
        """
        # This will block waiting for data
        try:
            return self.read_impl()
        except OSError as exc:
            if self.running:
                self.close()
                self.logger.warning(
                    "Read failure attempting reconnection. %s: %s",
                    type(exc).__name__,
                    str(exc),
                )
                self.open()
        return b""

    @abc.abstractmethod
    def read_impl(self):
        """ Implementation of the handler's read call"""

    def write(self, message):
        """
        Writes a single message after ensuring that the socket is fully open. On any error, close the socket in
        preparation for a reconnect. This internally will call the child's write_impl

        :param message: message to send
        :return: True if all data was written, False otherwise
        """
        try:
            self.write_impl(message)
            return True
        except OSError as exc:
            if self.running:
                self.logger.warning(
                    "Write failure: %s: %s", type(exc).__name__, str(exc)
                )
        return False

    @abc.abstractmethod
    def write_impl(self, message):
        """ Implementation of the handler's write call"""

    @staticmethod
    def kill_socket(sock):
        """ Kills a socket connection, but shutting it down and then closing. """
        if sock is None:
            return
        try:
            sock.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass
        finally:
            sock.close()


class TcpHandler(IpHandler):
    """
    An IpAdapter that allows for interfacing with TCP socket.
    """

    def __init__(
        self,
        address,
        port,
        server=True,
        logger=logging.getLogger("tcp_handler"),
        post_connect=None,
    ):
        """
        Init the TCP adapter with port and address

        :param address: address of TCP
        :param port: port of TCP
        """
        super().__init__(
            address, port, socket.SOCK_STREAM, server, logger, post_connect
        )
        self.client = None
        self.client_address = None

    def open_impl(self):
        """
        Open up this particular adapter. This adapter
        """
        # When a server, must accept and spawn new socket
        if self.server:
            self.socket.listen(IpHandler.MAX_CLIENT_BACKLOG)
            (self.client, self.client_address) = self.socket.accept()
        # When a client, use normal socket
        else:
            self.client = self.socket

    def close_impl(self):
        """
        Close the TCP socket that was spawned as appropriate.
        """
        try:
            IpHandler.kill_socket(self.client)
        finally:
            self.client = None
            self.client_address = None


    def read_impl(self):
        """
        Specific read implementation for the TCP handler. This involves reading from the spawned client socket, not the
        primary socket.
        """
        data = self.client.recv(IpAdapter.MAXIMUM_DATA_SIZE)
        if not data:
            self.close_impl()
            self.open_impl()
        return data

    def write_impl(self, message):
        """
        Send is implemented with TCP. It will send it to the connected client.

        :param message: message to send out
        """
        # Block until the port is open
        while self.connected != IpHandler.CONNECTED or self.client is None:
            pass
        self.client.sendall(message)


class UdpHandler(IpHandler):
    """
    Handler for UDP traffic. This will work in unison with the TCP adapter.
    """

    def __init__(
        self, address, port, server=True, logger=logging.getLogger("udp_handler")
    ):
        """
        Init UDP with address and port

        :param address: address of UDP
        :param port: port of UDP
        """
        super().__init__(address, port, socket.SOCK_DGRAM, server, logger)

    def open_impl(self):
        """No extra steps required"""

    def close_impl(self):
        """No extra steps required"""

    def read_impl(self):
        """
        Receive from the UDP handler. This involves receiving from an unconnected socket.
        """
        (data, address) = self.socket.recvfrom(IpAdapter.MAXIMUM_DATA_SIZE)
        return data

    def write_impl(self, message):
        """
        Write not implemented with UDP
        """
        raise NotImplementedError("UDP Handler cannot send data.")
