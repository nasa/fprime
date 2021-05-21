"""
@brief file writer to write decoded objects into files

This file writer takes in decoded data and writes the data
to the correct log files and destination files

@date Created August 8, 2019
@author Blake A. Harriman

@bug No known bugs
"""

import logging
import os

import fprime.constants
import fprime_gds.common.handlers
from fprime_gds.common.data_types.file_data import FilePacketType
from fprime_gds.common.files.helpers import FileStates, TransmitFile, file_to_dict

LOGGER = logging.getLogger("downlink")
LOGGER.setLevel(logging.INFO)


class FileDownlinker(fprime_gds.common.handlers.DataHandler):
    """File writer class for decoded packets"""

    def __init__(self, directory, timeout=20.0, log_dir=None):
        """
        FileWriter class constructor

        Args:
            directory: directory in which to write downlinked files
            timeout: This is used to keep track of how long the program should wait before throwing
            a timeout exception:

        Returns:
            An initialized FileWriter object.
        """
        super().__init__()
        self.__directory = directory
        self.__log_dir = log_dir if log_dir is not None else directory
        self.active = None
        self.files = []
        self.state = FileStates.IDLE
        self.sequence = 0  # Keeps track of what the current sequence ID should be
        self.timer = fprime_gds.common.files.helpers.Timeout()
        self.timer.setup(self.timeout, timeout)
        os.makedirs(self.__directory, exist_ok=True)

    def data_callback(self, data, sender=None):
        """
        Function called to pass data to the writer class

        Args:
            data: Binary data that has been decoded and passed to the correct consumer
        """
        self.timer.restart()  # Start or restart the timer
        packet_type = data.packetType
        # Check the packet type, and route to the appropriate sub-function
        if packet_type == FilePacketType.START:
            self.handle_start(data)
        elif packet_type == FilePacketType.DATA:
            self.handle_data(data)
        elif packet_type == FilePacketType.END:
            self.handle_end(data)
        elif packet_type == FilePacketType.CANCEL:
            self.handle_cancel(data)
        else:
            LOGGER.warning("Invalid file detected descriptor detected: %d", packet_type)

    def handle_start(self, data):
        """
        Handle a start packet data type.

        :param data: data packet that is a start packet
        """
        # Initialize all relevant START packet attributes into variables from file_data
        size = data.size
        source_path = data.sourcePath.decode(fprime.constants.DATA_ENCODING)
        dest_path = data.destPath.decode(fprime.constants.DATA_ENCODING)
        if self.state != FileStates.IDLE:
            LOGGER.warning("File transfer already inprogress. Aborting original.")
            self.finish()
        # Create the destination file where the DATA packet data will be stored
        assert self.active is None, "File is already open, something went wrong"
        self.active = TransmitFile(
            source_path,
            os.path.join(self.__directory, self.sanitize(dest_path)),
            size,
            self.__log_dir,
        )
        self.active.open("wb+")
        LOGGER.addHandler(self.active.log_handler)
        message = "Received START packet with metadata:\n"
        message += "\tSize: %d\n"
        message += "\tSource: %s\n"
        message += "\tDestination: %s"
        LOGGER.info(message, size, source_path, dest_path)
        self.files.append(self.active)
        self.state = FileStates.RUNNING
        self.sequence += 1

    def handle_data(self, data):
        """
        Handle the data packet.

        :param data: data packet
        """
        # Initialize all relevant DATA packet attributes into variables from file_data
        offset = data.offset
        data_bytes = data.dataVar
        if self.state != FileStates.RUNNING:
            LOGGER.warning("Received unexpected data packet for offset: %d", offset)
        else:
            if data.seqID != self.sequence:
                LOGGER.warning(
                    "Data packet has unexpected sequence id. Expected: %d got %d",
                    self.sequence,
                    data.seqID,
                )
            # Write the data information to the file
            self.active.write(data_bytes, offset)
            self.active.seek = offset + len(data_bytes)
            LOGGER.info(
                "Received DATA packet writing %d bytes to offset %s",
                len(data_bytes),
                offset,
            )
        self.sequence += 1

    def handle_cancel(self, _):
        """
        Handle cancel packet.

        :param _: data unused in cancel packet
        """
        # CANCEL Packets have no data
        if self.state != FileStates.RUNNING:
            LOGGER.warning("Received unexpected cancel packet")
        else:
            LOGGER.info("Received CANCEL packet, stopping downlink")
            self.finish()

    def handle_end(self, data):
        """
        Handle the end packet.

        :param data: end packet
        """
        # Initialize all relevant END packet attributes into variables from file_data
        # hashValue attribute is not relevant right now, but might be in the future
        if self.state != FileStates.RUNNING:
            LOGGER.warning("Received unexpected END packet")
        else:
            if data.seqID != self.sequence:
                LOGGER.warning(
                    "End packet has unexpected sequence id. Expected: %d got %d",
                    self.sequence,
                    data.seqID,
                )
            LOGGER.info("Received END packet, finishing downlink")
            self.finish()

    def timeout(self):
        """ Timeout the current downlink """
        LOGGER.warning("Timeout while downlinking file, aborting")
        self.finish()

    def finish(self):
        """ Finish the file downlink """
        self.timer.stop()
        self.state = FileStates.IDLE
        self.sequence = 0
        self.active.state = "FINISHED"
        LOGGER.removeHandler(self.active.log_handler)
        self.active.close()
        self.active = None

    def current_files(self):
        """ Return the current list of downlinked files"""
        return file_to_dict(self.files, uplink=False)

    @staticmethod
    def sanitize(filename):
        """
        Sanitize the given filename by removing slashes that would make new directories.

        :param filename: filename to sanitize
        :return: sanitized filename
        """
        return filename.replace(os.sep, "_")

    @property
    def directory(self):
        return self.__directory
