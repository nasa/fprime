"""
ground.py:

Sets up the ground-side handlers for the comm layer. This allows the communications layer to send data and receive data
to and from the rest of the ground system. This layer consists of an Abstract base class, which guarantees the methods
available to the comm layer, and currently a single implementation used to attach to the ThreadedTcpServer.

@author lestarch
"""

import abc
import logging

from .framing import TcpServerFramerDeframer
from fprime_gds.common.communication.adapters.ip import TcpHandler

LOGGER = logging.getLogger("gds_sender")


class GroundHandler(abc.ABC):
    """
    Ground handler class interacts upstream from the comm adapter layer to the greater ground system. This
    effectively means handling the following functions:

    1. receive_all: receives any and all frames from the ground layer for uplink to the spacecraft
    2. send_all: sends any and all frames to the ground system from the spacecraft's downlink
    """

    @abc.abstractmethod
    def open(self):
        """
        Opens any needed resources and prepares the system for receiving and sending.
        """

    @abc.abstractmethod
    def receive_all(self):
        """
        Receive all packet available from the ground layer. This will return full ground packets up to the uplinker.
        These packets should be fully-deframed and ready for reframing in the comm-layer specified format.

        :return: list deframed packets
        """

    @abc.abstractmethod
    def send_all(self, frames):
        """
        Receive all packet available from the ground layer. This will return full ground packets up to the uplinker.
        These packets should be fully-deframed and ready for reframing in the comm-layer specified format.

        :return: list deframed packets
        """


class TCPGround(GroundHandler):
    """
    Interface class defining necessary functions to talk to the GDS.
    """

    def __init__(self, address="127.0.0.1", port=50050):
        """
        Initialize this interface with the address and port needed to connect to the GDS.

        :param address: Address of the tcp server. Default 127.0.0.1
        :param port: port of the tcp server. Default: 50000
        """
        self.tcp = TcpHandler(
            address, port, False, LOGGER, post_connect=b"Register FSW\n"
        )
        self.data = bytearray()
        self.deframer = TcpServerFramerDeframer()

    def open(self):
        """
        Opens any needed resources and prepares the system for receiving and sending. This means opening the TCP handler
        and sending out the initial register command to the TcpServer.
        """
        if not self.tcp.open():
            return False
        return True

    def close(self):
        """
        Closes the open adapter.
        """
        self.tcp.close()

    def receive_all(self):
        """
        Receive all packet available from the ground layer. This will return full ground packets up to the uplinker.
        These packets should be fully-deframed and ready for reframing in the comm-layer specified format.

        :return: list deframed packets
        """
        self.data += self.tcp.read()
        (frames, self.data) = self.deframer.deframe_all(self.data, no_copy=True)
        return frames

    def send_all(self, frames):
        """
        Send all packets out to the tcp socket server. This adds the framing data for the TCP Server.

        :param frames: bytes object of data to write out to the socket server
        """
        for packet in frames:
            framed = self.deframer.frame(packet)
            self.tcp.write(framed)
