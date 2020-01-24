import time
import logging
import socket
import threading
import fprime_gds.common.adapters.base
import fprime_gds.common.logger

LOGGER = logging.getLogger("ip_adapter")

class IpAdapter(fprime_gds.common.adapters.base.BaseAdapter):
    """
    Adapts IP traffic for use with the GDS ground system. This serves two different "servers" both on the same address
    and port, but one uses TCP and the other uses UDP. Writes go to the TCP connection, and reads request data from
    both. This data is concatenated and returned up the stack for processing.
    """
    KEEPALIVE_INTERVAL = 0.500 # Interval to send a KEEPALIVE packet. None will turn off KEEPALIVE.
    KEEPALIVE_DATA = b"sitting well" # Data to send out as part of the KEEPALIVE packet. Should not be null nor empty.

    def __init__(self, sender, address, port):
        """
        Initialize this adapter by creating a handler for UDP and TCP. A thread for the KEEPALIVE application packets
        will be created, if the interval is not none.
        """
        super(IpAdapter, self).__init__(sender)
        self.stop = False
        self.keepalive = None
        self.running = True
        self.tcp = TcpHandler(address, port)
        self.udp = UdpHandler(address, port)
        self.thtcp = None
        self.thudp = None
        self.lock = threading.Lock()
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
            self.thtcp.start()
            self.thudp = threading.Thread(target=self.th_handler, args=(self.udp,))
            self.thudp.start()
            # Start up a keep-alive ping if desired. This will hit the TCP uplink, and die if the connection is down
            if IpAdapter.KEEPALIVE_INTERVAL is not None:
                self.keepalive = threading.Thread(target=self.th_alive, args=[float(self.KEEPALIVE_INTERVAL)]).start()
        except (ValueError, TypeError) as exc:
            LOGGER.error("Failed to start keep-alive thread. {}: {}".format(type(exc).__name__, str(exc)))

    def close(self):
        """Close the adapter, by setting the stop flag"""
        self.stop = True

    def th_handler(self, handler):
        """Adapter"""
        handler.open() #Initialize with an open call
        while not self.stop:
            data = handler.read()
            with self.lock:
                self.blob += data
        handler.close()

    def write(self, frame):
        """
        Send a given framed bit of data by sending it out the serial interface. It will attempt to reconnect if there is
        was a problem previously. This function will return true on success, or false on error.
        :param frame: framed data packet to send out
        :return: True, when data was sent through the UART. False otherwise.
        """
        return self.tcp.write(frame)

    def read(self, _):
        """
        Read up to a given count in bytes from the TCP adapter. This may return less than the full requested size but
        is expected to return some data.
        :param _: upper bound of data requested, unused with IP connections
        :return: data successfully read
        """
        with self.lock:
            data = self.blob
            self.blob = b""
        return data

    def th_alive(self, interval):
        """
        Run this thread in order to accept incoming connections and spiral them off into another thread for handling the
        given client.
        """
        while self.running:
            self.write(IpAdapter.KEEPALIVE_DATA)
            time.sleep(interval)

    @classmethod
    def get_arguments(cls):
        """
        Returns a dictionary of flag to argparse-argument dictionaries for use with argparse to setup arguments.
        :return: dictionary of flag to argparse arguments for use with argparse
        """
        return {
            ("-a", "--address"):{
                "dest":"address",
                "type":str,
                "default":"0.0.0.0",
                "help":"Address of the IP adapter server. Default: %(default)s"
            },
            ("-p", "--port"): {
                "dest":"port",
                "type":int,
                "default": 50000,
                "help":"Port of the IP adapter server. Default: %(default)s"
            }
        }

class IpHandler(object):
    """
    Base handler for IP types. This will provide the basic methods, and synchronization for reading/writing to multiple
    child implementations, namely: UDP and TCP. These child objects can then be instantiated individually.
    """
    ERROR_RETRY_INTERVAL = 1 # Seconds between a non-timeout error and a socket reconnection
    MAX_CLIENT_BACKLOG = 1 # One client backlog, allowing for reconnects
    # Connection states, it will go between these states
    CONNECTING = "CONNECTING"
    CONNECTED = "CONNECTED"
    CLOSED = "CLOSED"

    def __init__(self, address, port, type, server=True, logger=logging.getLogger("ip_handler")):
        """
        Initialize this handler. This will set the variables, and start up the internal receive thread.
        :param address: address of the handler
        :param port: port of the handler
        :param type: type of this adapter. socket.SOCK_STREAM or socket.SOCK_DGRAM
        """
        self.type = type
        self.address = address
        self.next_connect = 0
        self.port = port
        self.socket = None
        self.server = server
        self.lock = threading.Lock()
        self.connected = IpHandler.CLOSED
        self.logger = logger

    def open(self):
        """
        Open up this IP type adapter. Returning if already connected.
        """
        if self.CONNECTED == self.connected:
            return True
        # If a server, just continually try and reconnect. Otherwise, just try once.
        while True:
            try:
                # Prevent reconnects when the socket is connected. Socket should be closed on all errors
                if self.connected == IpHandler.CLOSED and self.next_connect < time.time():
                    self.connected = IpHandler.CONNECTING
                    self.socket = socket.socket(socket.AF_INET, self.type)
                    if self.server:
                        self.socket.bind((self.address, self.port))
                    else:
                        self.socket.connect((self.address, self.port))
                    self.open_impl()
                    self.connected = IpHandler.CONNECTED
                    self.logger.info("{} connected to {}:{}"
                                .format("Server" if self.server else "Client", self.address, self.port))
            # All errors (timeout included) we should close down the socket, which sets self.connected
            except socket.error as exc:
                self.logger.warning("Failed to open socket at {}:{}, retrying: {}: {}"
                                    .format(self.address, self.port, type(exc).__name__, str(exc)))
                self.next_connect = time.time() + IpHandler.ERROR_RETRY_INTERVAL
                self.close()
            # Check ending condition of loop
            if not self.server or self.CONNECTED == self.connected:
                break
        if self.CONNECTED == self.connected:
            self.post_open()
        return self.connected == self.CONNECTED

    def post_open(self):
        """Used by sender subclass to perform actions on start-up"""
        pass

    def close(self):
        """
        Close this specific IP handler. This involves setting connected to False, and closing non-null sockets.
        """
        try:
            self.close_impl()
            if self.socket is not None:
                self.socket.close()
                self.socket = None
        finally:
            self.connected = IpHandler.CLOSED

    def read(self):
        """
        Reads a single message after ensuring that the socket is fully open. On a non-timeout error, close the socket in
        preparation for a reconnect. This internally will call the child's read_impl
        :return: data read from TCP server or b"" when nothing is available
        """
        # Open if not already open, on failure return b""
        if self.open():
            # This will block waiting for data
            try:
                return self.read_impl()
            except socket.error as exc:
                self.close()
                self.logger.warning("Read failure attempting reconnection. {}: ".format(type(exc).__name__, str(exc)))
                self.open()
        return b""

    def write(self, message):
        """
        Writes a single message after ensuring that the socket is fully open. On any error, close the socket in
        preparation for a reconnect. This internally will call the child's write_impl
        :param message: message to send
        :return: True if all data was written, False otherwise
        """
        # Open if not already open, on failure return b""
        if self.open():
            # This will block waiting for data
            try:
                self.write_impl(message)
                return True
            except socket.error as exc:
                self.close()
                self.logger.warning("Write failure, reconnecting. {}: ".format(type(exc).__name__, str(exc)))
                self.open()
        return False


class TcpHandler(IpHandler):
    """
    An IpAdapter that allows for interfacing with TCP socket.
    """
    def __init__(self,  address, port, server=True, logger=logging.getLogger("tcp_handler")):
        """
        Init the TCP adapter with port and address
        :param address: address of TCP
        :param port: port of TCP
        """
        super(TcpHandler, self).__init__(address, port, socket.SOCK_STREAM, server, logger)
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
        if self.client is not None:
            self.client.close()
            self.client = None
            self.client_address = None

    def read_impl(self):
        """
        Specific read implementation for the TCP handler. This involves reading from the spawned client socket, not the
        primary socket.
        """
        data = self.client.recv(IpAdapter.MAXIMUM_DATA_SIZE)
        return data

    def write_impl(self, message):
        """
        Send is implemented with TCP. It will send it to the connected client.
        :param message:
        :return:
        """
        self.client.sendall(message)


class UdpHandler(IpHandler):
    """
    Handler for UDP traffic. This will work in unison with the TCP adapter.
    """
    def __init__(self,  address, port, server=True, logger=logging.getLogger("udp_handler")):
        """
        Init UDP with address and port
        :param address: address of UDP
        :param port: port of UDP
        """
        super(UdpHandler, self).__init__(address, port, socket.SOCK_DGRAM, server, logger)

    def open_impl(self):
        """No extra steps required"""
        pass

    def close_impl(self):
        """No extra steps required"""
        pass

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
