"""
Sender:

Sends data to the GDS internal interface. This version connects to the ThreadedTcpServer and sends data to port. This
sender should be used with a radio adapter in order to pull and push data to/from the Gds internal ThreadedTcpServer.

@author lestarch
"""
from __future__ import print_function
import socket
import threading

import logging
LOGGER = logging.getLogger("gds_sender")
logging.basicConfig(level=logging.INFO)

class TCPSender(object):
    """
    Interface class defining necessary functions to talk to the GDS.
    """
    SOCKET_TIMEOUT = 0.250
    def __init__(self, address="127.0.0.1", port=50000):
        """
        Initialize this interface with the address and port needed to connect to the GDS.
        :param address: Address of the tcp server. Default 127.0.0.1
        :param port: port of the tcp server. Default: 50000
        """
        # Socket variables
        self.address = address
        self.port = port
        self.socket = None
        self.connected = True
        self.recv_lock = threading.Lock()
        self.send_lock = threading.Lock()

        # Callback and threading variables
        self.callbacks = []
        self.connected = False

    def open(self):
        """
        Opens the connection to the TCP server. Raises exception on connection issues.
        """
        try:
            # Close in preparation for reopening
            self.close()
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.address, self.port))
            self.socket.settimeout(TCPSender.SOCKET_TIMEOUT)
            # Opening volley
            self.socket.sendall(b"Register FSW\n")
            LOGGER.info("Connected to GDS at: {}:{}".format(self.address, self.port))
            self.connected = True
        except:
            self.close()
            raise

    def close(self):
        """
        Close the connection to the GDS TCP server.
        """
        if self.socket is not None:
            self.socket.close()
            self.socket = None
            # Fully conected otherwise stop
            if self.connected:
                LOGGER.info("Disconnected from GDS at {}:{}".format(self.address, self.port))
                self.connected = False

    def write(self, packet):
        """
        Write a packet out to the tcp socket server
        :param packet: bytes object of data to write out to the socket server
        """
        try:
            if self.socket is None:
                self.open()
            data = b"A5A5 GUI %s" % (packet)
            with self.send_lock:
                self.socket.sendall(data)
        except socket.error:
            pass

    def poll(self):
        """
        Polls the interface looking for a packet of data.
        :return: bytes object containing packet
        """
        try:
            if self.socket is None:
                self.open()
            self.socket.settimeout(0.010)
            with self.recv_lock:
                return self.socket.recv(4096)
        except socket.error:
            return None
