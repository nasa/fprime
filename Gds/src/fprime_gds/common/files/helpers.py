"""
helpers.py:

Helpers for file uplink, and downlink. This contains shared components, classes, and support architecture in order to
enable both file uplink and downlink to share the same structures.

@author mstarch, and Blake A. Harriman's work
"""
import os
import enum
import struct
import threading


class Timeout(object):
    """
    Starts a timeout thread and will respond with a callback to a function, if the timeout expires.
    """
    def __init__(self):
        """ Sets up needed member variables """
        self.__timer = None
        self.__timeout = None
        self.__callback = None
        self.args = ()

    def setup(self, callback, timeout=5, args=()):
        """
        Sets up the timeout but does not start it.
        :param callback: function called when timeout expires
        :param timeout: (optional) timeout duration. Default: 5 seconds
        """
        self.__timeout = timeout
        self.__callback = callback
        self.args = args

    def start(self):
        """ Starts the timeout after a previous setup. """
        assert self.__timer is None, "Timer already started, call restart() instead"
        assert self.__callback is not None, "Setup timeout before calling start"
        assert self.__timeout is not None, "Setup timeout before calling start"
        self.__timer = threading.Timer(self.__timeout, self.__callback, args=self.args)
        self.__timer.start()

    def restart(self, ignore_stopped=False):
        """
        Restarts the given timer first canceling and then starting it again.
        :param ignore_stopped: ignores an already stopped timer
        """
        if self.__timer is not None or not ignore_stopped:
            self.stop()
        self.start()

    def stop(self):
        """
        Stops the timeout preventing a callback to the stored function.
        """
        if self.__timer is not None:
            self.__timer.cancel()
            self.__timer = None


class FileStates(enum.Enum):
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


class TransmitFile(object):
    """
    Wraps the file information needed for the uplink process.
    """
    def __init__(self, source, destination):
        """ Construct the uplink file """
        self.__mode = None
        self.__source = source
        self.__destination = destination
        self.__size = os.path.getsize(source)
        self.__seek = 0
        self.__state = "QUEUED"
        self.__fd = None
        self.__checksum = CFDPChecksum()

    def open(self, mode="rb"):
        """
        Opens the file descriptor and prepares it for uplink
        """
        assert self.__fd is None, "Must close file before attempting to reopen it"
        self.__mode = mode
        self.__state = "TRANSMITTING"
        self.__fd = open(self.__source if self.__mode.startswith("r") else self.destination, self.__mode)

    def read(self, chunk):
        """ Read the chunk from the file """
        assert self.__fd is not None, "Must open file before reading"
        assert self.__mode.startswith("r"), "File opened with invalid mode for 'read'"
        return self.__fd.read(chunk)

    def write(self, chunk, offset):
        """
        Write a chunk to the file
        :param chunk:
        :param offset: offset to write to
        """
        assert self.__fd is not None, "Must open file before writing"
        assert self.__mode.startswith("w"), "File opened with invalid mode for 'write'"
        self.__fd.seek(offset, 0)
        self.__fd.write(chunk)

    def close(self):
        """
        Opens the file descriptor and prepares it for uplink
        """
        if self.__fd is not None:
            self.__fd.close()
            self.__fd = None

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


def file_to_dict(files, uplink=True):
    """
    Converts files to dictionary
    :param files: list of TransmitFiles to convert
    :return: list of dictionaries
    """
    current = []
    for item in files:
        current.append({
            "source": item.source,
            "destination": item.destination,
            "size": item.size,
            "current": item.seek,
            "state": item.state,
            "percent": int(item.seek/item.size * 100.0),
            "uplink": uplink
        })
    return current