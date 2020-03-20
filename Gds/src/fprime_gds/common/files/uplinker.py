"""
uplinker.py:

Contains the code necessary to uplink files through the F prime ground system to a running deployment. The file uplink
process will read in a file from the OS, and uplink it in chunks. The system throttles chunks of the file requiring a
handshake packet in return before the next chunk is sent out.

@author lestarch
"""
import os
import enum
import zlib
import queue
import threading

import fprime_gds.common.handlers
from fprime_gds.common.data_types.file_data import *


class UplinkStates(enum.Enum):
    """
    An enumeration of states used in the below FileUplinker class. This allows uplinking to follow a basic state machine
    that waits for handshaking and can handle canceling packets.
    """
    IDLE = 0
    RUNNING = 1
    CANCELED = 2
    END_WAIT = 3  # Waiting for the handshake for CANCEL or END packet


class FileUplinker(fprime_gds.common.handlers.DataHandler):
    """
    File uplinking component. Keeps track of the currently uplinking file, and registers as a receiver for the handshake
    packets that are send back on each packet. This uplinker defines the following externally available functions for
    use in the greater ground layer:

    1. start: to start a file uplink. Will throw exception if already running.
    2. get_progress: returns the current progress of the upload: (file name, size uploaded, total size)
    3. data_callback: receives and verifies handshake, then uplinks next block.
    4. cancel: cancel the current upload.
    """
    CHUNK_SIZE = 256

    def __init__(self, file_encoder, chunk=CHUNK_SIZE, timeout=20):
        """

        """
        self.state = UplinkStates.IDLE
        self.active = None
        self.current_size = 0
        self.total = 0
        self.sequence = 0
        self.chunk = chunk
        self._fd = None
        self.timeout = timeout
        self.file_encoder = file_encoder
        self.current_crc = 0
        self.queue = queue.Queue()
        self.flag = threading.Semaphore()
        threading.Thread(target=self.uplink_thread, args=()).start()
        self.__destination_dir = "/"

    def enqueue(self, filepath, destination=None):
        """
        Enqueue files for the upload. This tunnels into the upload thread, which unblocks once files have been enqueued
        read to upload.
        :param filepath: filepath to upload to the system
        :param destination: (optional) destination to uplink to. Default: current destination + file's basename
        """
        if destination is None:
            destination = os.path.join(self.__destination_dir, os.path.basename(filepath))
        self.queue.put((filepath, destination))

    def uplink_thread(self):
        """
        A thread that will uplink files on after another until all files that have been enqueued are properly processed.
        """
        while True:
            filepath, dest = self.queue.get()
            self.start(filepath, dest)
            if not self.flag.acquire():
                self.cancel()
            self.flag.release() # Just wanted to know where were done by blocking on the semaphore

    def start(self, filepath, destination):
        """
        Starts a file uplink to a running F prime deployment. This will open of the file, save the file handle, and emit
        the start file uplink packet. It will also store the state for the file uplink progress. If already uplinking
        then a FileUplinkerBusyException will be raised.
        :param filepath: file path to upload.
        :param destination: destination to upload to
        :raises FileUplinkerBusyException: when an upload is already running
        :raises OSError: when file is inaccessible
        """
        # Prevent multiple uplinks at once
        if self.state != UplinkStates.IDLE:
            raise FileUplinkerBusyException("Currently uplinking file '{}' cannot start uplinking '{}'"
                                            .format(self.active, filepath))
        self.flag.acquire()
        self.state = UplinkStates.RUNNING
        self.active = filepath
        self.current_size = 0
        self.total = os.path.getsize(filepath) #Source of OSError when inaccesible
        self._fd = open(filepath, "rb") #Source of OSError when no permissions
        self.file_encoder.data_callback(StartPacketData(self.get_next_sequence(), self.total, filepath, destination))
        pass

    def data_callback(self, data, sender=None):
        """
        Process incoming handshake data, and if it is the expected handshake, then it uplinks next packet.
        :param data: data from handshake packet.
        """
        # TODO: check handshake
        # If it is an end-wait or a cancel state, respond without reading next chunk
        if self.state == UplinkStates.END_WAIT:
            self.state = UplinkStates.IDLE
            self.flag.release()
            return
        elif self.state == UplinkStates.CANCELED:
            self.file_encoder.data_callback(CancelPacketData(self.get_next_sequence()))
            self.finish()
            return
        # Read next chunk of data.  b'' means the file is empty
        outgoing = self._fd.read(self.chunk)
        if outgoing == b'':
            self.file_encoder.data_callback(EndPacketData(self.get_next_sequence(), self.current_crc))
            self.finish()
        else:
            self.current_crc = zlib.crc32(outgoing, self.current_crc)
            self.file_encoder.data_callback(DataPacketData(self.get_next_sequence(), self.current_size, outgoing))
            self.current_size += len(outgoing)

    def cancel(self):
        """
        Cancels the currently active file uplink. Will emit a cancel packet to inform the deployment that the file is
        canceled. This merely sets the state to "canceled" and will handle this later.
        """
        if self.state == UplinkStates.RUNNING:
            self.state = UplinkStates.CANCELED

    def finish(self):
        """
        Finishes the current file uplink by closing the file
        """
        self._fd.close()
        self._fd = None
        self.state = UplinkStates.END_WAIT
        self.sequence = 0

    def get_next_sequence(self):
        """Gets the next sequence number"""
        tmp = self.sequence
        self.sequence = self.sequence + 1
        return tmp

    @property
    def destination_dir(self):
        """
        Get the destination property.
        :return: value of destination
        """
        return self.__destination_dir

    @destination_dir.setter
    def destination_dir(self, destination):
        """
        Set the destination property
        :param destination: new destination
        """
        self.__destination_dir = destination


class FileUplinkerBusyException(Exception):
    """ File uplinker is busy and cannot uplink more files """
    pass