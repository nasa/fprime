"""
uplinker.py:

Contains the code necessary to uplink files through the F prime ground system to a running deployment. The file uplink
process will read in a file from the OS, and uplink it in chunks. The system throttles chunks of the file requiring a
handshake packet in return before the next chunk is sent out.

@author lestarch
"""
import os
import enum
import copy
import zlib
import queue
import struct
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


class CFDPChecksum(object):
    """ Class running the CFDG checksum """
    def __init__(self):
        """Set value as zero """
        self.__value = 0

    def update(self, data, offset):
        """ Update the checksum """
        while data:
            padding_len = offset % 4
            calc_bytes = bytes([0] * padding_len) + data[:4 - padding_len] + bytes([0, 0, 0, 0])
            self.__value = (self.__value + struct.unpack_from(">I", calc_bytes, 0)[0]) & 0xffffffff
            # Update pointers
            data = data[4 - padding_len:]
            offset = offset + (4 - padding_len)

    @property
    def value(self):
        return self.__value

class UplinkFile(object):
    """
    Wraps the file information needed for the uplink process.
    """
    def __init__(self, source, destination):
        """ Construct the uplink file """
        self.__source = source
        self.__destination = destination
        self.__size = os.path.getsize(source)
        self.__seek = 0
        self.__state = "QUEUED"
        self.__fd = None
        self.__checksum = CFDPChecksum()

    def open(self):
        """
        Opens the file descriptor and prepares it for uplink
        """
        self.__state = "UPLINKING"
        self.__fd = open(self.__source, "rb")

    def read(self, chunk):
        """ Read the chunk from the file """
        return self.__fd.read(chunk)

    def close(self):
        """
        Opens the file descriptor and prepares it for uplink
        """
        self.__fd.close()

    @property
    def source(self):
        return self.__source

    @property
    def destination(self):
        return self.__destination

    @property
    def size(self):
        return self.__size

    @property
    def seek(self):
        return self.__seek

    @seek.setter
    def seek(self, seek):
        self.__seek = seek

    @property
    def state(self):
        return self.__state

    @state.setter
    def state(self, state):
        self.__state = state

    @property
    def checksum(self):
        return self.__checksum


class UplinkQueue(object):
    """
    Handles queuing of files to send to the uplinker. This offloads the work of determining when done, and what to do
    while waiting. It also owns the thread that runs uplink.
    """
    def __init__(self, uplinker):
        """
        Constructs the uplink queue with a reference to the object that run the uplink.
        :param uplinker: uplinker to callback into
        """
        self.uplinker = uplinker
        self.busy = threading.Semaphore()
        self.queue = queue.Queue()
        self.__file_store = []
        threading.Thread(target=self.run, args=()).start()

    def enqueue(self, filepath, destination):
        """
        Enqueue the file and destination pair onto the queue
        :param filepath: filepath to upload to the given destination
        :param destination: destination path to upload the filepath to
        """
        file_obj = UplinkFile(filepath, destination)
        self.queue.put(file_obj)
        self.__file_store.append(file_obj)

    def pause(self):
        """ Pause the uplinker """
        self.busy.acquire()

    def unpause(self):
        """ Pause the uplinker """
        self.busy.release()

    def remove(self, source):
        """
        Remove a file by its source from the queue and the permanent uplink list.
        """
        try:
            first = None
            found = self.queue.get_nowait()
            while found != first:
                if found.source == source:
                    break
                elif first is None:
                    first = found
            self.queue.put_nowait(found)
            found = self.queue.get_nowait()
        except queue.Empty:
            pass
        self.__file_store.remove(found)

    def run(self):
        """
        A thread that will uplink files on after another until all files that have been enqueued are properly processed.
        """
        while True:
            file_obj = self.queue.get()
            self.busy.acquire()
            self.uplinker.start(file_obj)
            # Wait until the file is finished, then release again waiting for more files
            self.busy.acquire()
            self.busy.release()

    def current(self):
        """
        Gets a set of current files. This will create a copy, to prevent messing with uplink.
        :return: copy of current files
        """
        current = []
        for item in self.__file_store:
            current.append({
                "source": item.source,
                "destination": item.destination,
                "size": item.size,
                "current": item.seek,
                "state": item.state,
                "percent": int(item.seek/item.size * 100.0)
            })
        return current


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
        Constructor to build the file uplinker.
        """
        self.state = UplinkStates.IDLE
        self.queue = UplinkQueue(self)
        self.active = None
        self.sequence = 0
        self.chunk = chunk
        self.timeout = timeout
        self.file_encoder = file_encoder
        self.__destination_dir = "/"
        self.__expected = []

    def enqueue(self, filepath, destination=None):
        """
        Enqueue files for the upload. This tunnels into the upload thread, which unblocks once files have been enqueued
        read to upload.
        :param filepath: filepath to upload to the system
        :param destination: (optional) destination to uplink to. Default: current destination + file's basename
        """
        if destination is None:
            destination = os.path.join(self.__destination_dir, os.path.basename(filepath))
        self.queue.enqueue(filepath, destination)

    def pause_unpause(self, pause=False):
        """ Pause/Unpause uplink """
        pass

    def cancel_remove(self, file):
        """
        Cancel/remove the uplink of the given file
        """
        if file == self.active.source:
            self.cancel()
        else:
            self.queue.remove(file)

    def current_files(self):
        """
        Returns the current set of files.
        :return: current files
        """
        return self.queue.current()

    def start(self, file_obj):
        """
        Starts a file uplink to a running F prime deployment. This will open of the file, save the file handle, and emit
        the start file uplink packet. It will also store the state for the file uplink progress. If already uplinking
        then a FileUplinkerBusyException will be raised.
        :param file_obj: file object to upload
        :raises FileUplinkerBusyException: when an upload is already running
        :raises OSError: when file is inaccessible
        """
        # Prevent multiple uplinks at once
        if self.state != UplinkStates.IDLE:
            raise FileUplinkerBusyException("Currently uplinking file '{}' cannot start uplinking '{}'"
                                            .format(self.active.source, file_obj.source))
        self.state = UplinkStates.RUNNING
        self.active = file_obj
        self.active.open()
        self.send(StartPacketData(self.get_next_sequence(), self.active.size,
                                  self.active.source, self.active.destination))

    def send(self, packet_data):
        """
        A function to send the packet out.  Starts timeout and handles timeouts.
        :param packet_data: packet data to send,
        """
        self.__expected = self.file_encoder.data_callback(packet_data)[8:]

    def data_callback(self, data, sender=None):
        """
        Process incoming handshake data, and if it is the expected handshake, then it uplinks next packet.
        :param data: data from handshake packet.
        """
        # Ignore handshakes not for us
        if not self.valid_handshake(data):
            return
        # If it is an end-wait or a cancel state, respond without reading next chunk
        if self.state == UplinkStates.END_WAIT:
            self.active.state = "FINISHED" if self.active.state != "CANCELED" else "CANCELED"
            self.state = UplinkStates.IDLE
            self.queue.unpause() # Allow the queue to continue
            return
        elif self.state == UplinkStates.CANCELED:
            self.send(CancelPacketData(self.get_next_sequence()))
            self.finish()
            return
        # Read next chunk of data.  b'' means the file is empty
        outgoing = self.active.read(self.chunk)
        if outgoing == b'':
            self.send(EndPacketData(self.get_next_sequence(), self.active.checksum.value))
            self.finish()
        else:
            self.active.checksum.update(outgoing, self.active.seek)
            self.send(DataPacketData(self.get_next_sequence(), self.active.seek, outgoing))
            self.active.seek += len(outgoing)

    def cancel(self):
        """
        Cancels the currently active file uplink. Will emit a cancel packet to inform the deployment that the file is
        canceled. This merely sets the state to "canceled" and will handle this later.
        """
        if self.state == UplinkStates.RUNNING:
            self.state = UplinkStates.CANCELED
            self.active.state = "CANCELED"
            #self.queue.pause()

    def finish(self, wait_for_handshake=True):
        """
        Finishes the current file uplink by closing the file, and starting the end processs. If the uplinker should
        immediately terminate (like during a timeout) then set wait_for_handshake to False.
        :param wait_for_handshake: (optional) wrap up cleanly by waiting for handshake. Default: True, clean wait
        """
        self.active.close()
        self.sequence = 0
        self.state = UplinkStates.END_WAIT
        # Immediate termination items
        if not wait_for_handshake:
            self.state = UplinkStates.IDLE
            self.__expected = []
            self.queue.unpause()

    def get_next_sequence(self):
        """Gets the next sequence number"""
        tmp = self.sequence
        self.sequence = self.sequence + 1
        return tmp

    def valid_handshake(self, data):
        """
        Check the handshake data and ensure that it is as expected. This will allow us to only handle handshakes that
        we expected. This will ensure that the handshake data is an exact match of the send data.
        :param data: data to check against what was transmitted
        :return: True, if proper handshake, False otherwise
        """
        return data == self.__expected

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