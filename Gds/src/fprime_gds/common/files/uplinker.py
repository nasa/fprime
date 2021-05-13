"""
uplinker.py:

Contains the code necessary to uplink files through the F prime ground system to a running deployment. The file uplink
process will read in a file from the OS, and uplink it in chunks. The system throttles chunks of the file requiring a
handshake packet in return before the next chunk is sent out.

@author lestarch
"""
import os
import queue
import threading
import time

import fprime_gds.common.handlers
from fprime_gds.common.data_types.file_data import (
    CancelPacketData,
    DataPacketData,
    EndPacketData,
    StartPacketData,
)
from fprime_gds.common.files.helpers import (
    FileStates,
    Timeout,
    TransmitFile,
    file_to_dict,
)


class UplinkQueue:
    """
    Handles queuing of files to send to the uplinker. This offloads the work of determining when done, and what to do
    while waiting. It also owns the thread that starts uplink. This thread watches for the current uplink to finish, and
    then it starts the next one and returns to a quiescent state.
    """

    def __init__(self, uplinker):
        """
        Constructs the uplink queue with a reference to the object that run the uplink.

        :param uplinker: uplinker to callback into
        """
        self.running = True
        self.uplinker = uplinker
        self.busy = threading.Semaphore()
        self.queue = queue.Queue()
        self.__file_store = []
        self.__exit = threading.Event()
        self.__thread = threading.Thread(target=self.run, args=())
        self.__thread.start()

    def enqueue(self, filepath, destination):
        """
        Enqueue the file and destination pair onto the queue

        :param filepath: filepath to upload to the given destination
        :param destination: destination path to upload the filepath to
        """
        file_obj = TransmitFile(filepath, destination)
        self.queue.put(file_obj)
        self.__file_store.append(file_obj)

    def pause(self):
        """ Pause the uplinker, setting the running flag off, then holding the busy semaphore """
        if self.running:
            self.running = False
            self.busy.acquire()

    def unpause(self):
        """ Unpause the uplinker, releasing the busy semaphore and then restoring the running flag """
        if not self.running:
            self.busy.release()
            self.running = True

    def is_running(self):
        """ Check if the uplink is running """
        return self.running

    def remove(self, source):
        """
        Remove a file by its source from the queue and the permanent uplink list. This will touch each record in the
        queue one time, as they are pulled off the queue, inspected, and the replaced.

        :param source: source file to remove
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
            return
        self.__file_store.remove(found)

    def run(self):
        """
        A thread that will uplink files on after another until all files that have been enqueued are properly processed.
        To stop this thread, set the exit event.
        """
        while not self.__exit.is_set():
            # While running (not paused) continue to get - uplink - wait
            while self.running:
                file_obj = self.queue.get()
                # Exit condition is a None message, which unblocks the above get
                if file_obj is None or not self.running:
                    self.queue.put(file_obj)
                    break
                # Once a file is obtained from the queue, acquire the busy lock before starting. This prevents a in-use
                # or paused queue from uplinking.
                self.busy.acquire()
                self.uplinker.start(file_obj)
                # Wait until the file is finished, then release again waiting for more files
                self.busy.acquire()
                self.busy.release()
            time.sleep(0.010)  # Don't busy spin, but yield and sleep

    def current(self):
        """
        Gets a set of current files. This will create a copy, to prevent messing with uplink.

        :return: copy of current files transformed into a JSONable dictionary
        """
        return file_to_dict(self.__file_store)

    def exit(self):
        """ Exit event to shutdown the thread """
        self.__exit.set()
        self.running = False
        self.queue.put(
            None
        )  # Force an end to the wait for a file, if an uplink is not in-progress

    def join(self):
        """ Join with this uplinker """
        self.__thread.join()


class FileUplinker(fprime_gds.common.handlers.DataHandler):
    """
    File uplinking component. Keeps track of the currently uplinking file, and registers as a receiver for the handshake
    packets that are send back on each packet.
    """

    CHUNK_SIZE = 256

    def __init__(self, file_encoder, chunk=CHUNK_SIZE, timeout=20):
        """
        Constructor to build the file uplinker.
        """
        self.state = FileStates.IDLE
        self.queue = UplinkQueue(self)
        self.active = None
        self.sequence = 0
        self.chunk = chunk
        self.file_encoder = file_encoder
        self.__destination_dir = "/"
        self.__expected = []
        self.__timeout = Timeout()
        self.__timeout.setup(self.timeout, timeout)

    def enqueue(self, filepath, destination=None):
        """
        Enqueue files for the upload. This tunnels into the upload queue, which unblocks once files have been enqueued
        and begins to upload each file sequentially.

        :param filepath: filepath to upload to the system
        :param destination: (optional) destination to uplink to. Default: current destination + file's basename
        """
        if destination is None:
            destination = os.path.join(
                self.__destination_dir, os.path.basename(filepath)
            )
        self.queue.enqueue(filepath, destination)

    def exit(self):
        """ Exit this uplinker by killing the thread """
        self.queue.exit()
        self.cancel()
        self.queue.join()

    def is_running(self):
        """ Check if the queue is running """
        return self.queue.is_running()

    def pause(self):
        """ Pause uplink by canceling the uplink, and then pausing the uplink queue """
        self.cancel()
        self.queue.pause()

    def unpause(self):
        """ Unpauses the uplink by unpausing the internal queue """
        self.queue.unpause()

    def cancel_remove(self, file):
        """
        Cancel/remove the uplink of the given file.  If uplinking, it will cancel the uplink, and if queued it will
        remove the file from the queue.  Unknown files will be ignored.

        :param file: file to remove from the uplinker
        """
        if self.active is not None and file == self.active.source:
            self.cancel()
        else:
            self.queue.remove(file)

    def current_files(self):
        """
        Returns the current set of files held by the uplink queue.

        :return: current files as a list in JSONable format
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
        if self.state != FileStates.IDLE:
            raise FileUplinkerBusyException(
                "Currently uplinking file '{}' cannot start uplinking '{}'".format(
                    self.active.source, file_obj.source
                )
            )
        self.state = FileStates.RUNNING
        self.active = file_obj
        self.active.open()
        self.send(
            StartPacketData(
                self.get_next_sequence(),
                self.active.size,
                self.active.source,
                self.active.destination,
            )
        )

    def send(self, packet_data):
        """
        A function to send the packet out.  Starts timeout and then pushes the packet to the file encoder.

        :param packet_data: packet data to send that will be pushed to the encoder
        """
        self.__timeout.restart()
        self.__expected = self.file_encoder.data_callback(packet_data)[8:]

    def data_callback(self, data, sender=None):
        """
        Process incoming handshake data, and if it is the expected handshake, then it uplinks the next packet. In the
        file. Invalid handshakes are ignore. When finished a returning handshake puts the uplinker into idle state.

        :param data: data from handshake packet to be verified against that previously sent
        """
        # Ignore handshakes not for us
        if not self.valid_handshake(data):
            return
        # If it is an end-wait or a cancel state, respond without reading next chunk
        if self.state == FileStates.END_WAIT:
            self.active.state = (
                "FINISHED" if self.active.state != "CANCELED" else "CANCELED"
            )
            self.state = FileStates.IDLE
            self.queue.busy.release()  # Allow the queue to continue
            self.__timeout.stop()
            return
        elif self.state == FileStates.CANCELED:
            self.send(CancelPacketData(self.get_next_sequence()))
            self.finish()
            return
        # Read next chunk of data.  b'' means the file is empty
        outgoing = self.active.read(self.chunk)
        if outgoing == b"":
            self.send(
                EndPacketData(self.get_next_sequence(), self.active.checksum.value)
            )
            self.finish()
        else:
            self.active.checksum.update(outgoing, self.active.seek)
            self.send(
                DataPacketData(self.get_next_sequence(), self.active.seek, outgoing)
            )
            self.active.seek += len(outgoing)

    def cancel(self):
        """
        Cancels the currently active file uplink. Will emit a cancel packet to inform the deployment that the file is
        canceled. This merely sets the state to "canceled" and will handle this later. This implies that in the case of
        a timeout, cancels may be ignored.
        """
        if self.state == FileStates.RUNNING:
            self.state = FileStates.CANCELED
            self.active.state = "CANCELED"
            # self.queue.pause()

    def timeout(self):
        """ Handles timeout o file packet by finishing the upload immediately, and setting the state to timeout """
        self.finish(False)
        self.active.state = "TIMEOUT"

    def finish(self, wait_for_handshake=True):
        """
        Finishes the current file uplink by closing the file, and starting the end process. If the uplinker should
        immediately terminate (like during a timeout) then set wait_for_handshake to False.

        :param wait_for_handshake: (optional) wrap up cleanly by waiting for handshake. Default: True, clean wait
        """
        self.active.close()
        self.sequence = 0
        self.state = FileStates.END_WAIT
        os.remove(self.active.source)
        # Immediate termination items
        if not wait_for_handshake:
            self.state = FileStates.IDLE
            self.__expected = []
            self.queue.busy.release()
            self.__timeout.stop()

    def get_next_sequence(self):
        """ Gets the next sequence number """
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
        Get the destination directory that files will be uplinked to, if not currently specified.

        :return: value of destination
        """
        return self.__destination_dir

    @destination_dir.setter
    def destination_dir(self, destination):
        """
        Set the destination directory that files will be uplinked to.

        :param destination: new destination
        """
        self.__destination_dir = destination


class FileUplinkerBusyException(Exception):
    """ File uplinker is busy and cannot uplink more files """
