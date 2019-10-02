"""
Sender:

Sends data to the GDS internal interface. This version connects to the ThreadedTcpServer and sends data to port. This
sender should be used with a radio adapter in order to pull and push data to/from the Gds internal ThreadedTcpServer.

@author lestarch
"""
from __future__ import print_function
import logging
import struct

from .ip import TcpHandler

LOGGER = logging.getLogger("gds_sender")

class TCPSender(TcpHandler):
    """
    Interface class defining necessary functions to talk to the GDS.
    """
    SOCKET_TIMEOUT = 0.250
    def __init__(self, address="127.0.0.1", port=50050):
        """
        Initialize this interface with the address and port needed to connect to the GDS.
        :param address: Address of the tcp server. Default 127.0.0.1
        :param port: port of the tcp server. Default: 50000
        """
        super(TCPSender, self).__init__(address, port, False, logging.getLogger("gds_sender"))
        self.frame = True

    def post_open(self):
        """
        Opens the connection to the TCP as a client. Then sends the required write packet to startup the session.
        """
        with self.lock:
            self.frame = False
        self.write(b"Register FSW\n")
        with self.lock:
            self.frame = True

    def read_impl(self):
        """
        Write a packet out to the tcp socket server. This adds the framing data for the TCP Server.
        :param packet: bytes object of data to write out to the socket server
        """
        data = super(TCPSender, self).read_impl()
        return data[8:]


    def write_impl(self, packet):
        """
        Write a packet out to the tcp socket server. This adds the framing data for the TCP Server.
        :param packet: bytes object of data to write out to the socket server
        """
        if self.frame:
            packet = b"A5A5 GUI %s%s" % (struct.pack(">I", len(packet)), packet)
        super(TCPSender, self).write_impl(packet)



