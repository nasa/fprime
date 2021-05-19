"""
@brief Class to store data from each specific file_data type

@date Created July 10, 2019
@author Blake A. Harriman

@bug No known bugs
"""
from enum import Enum

from fprime_gds.common.data_types import sys_data


class FilePacketType(Enum):
    """
    An enumeration of available file packets
    """

    START = 0
    DATA = 1
    END = 2
    CANCEL = 3


# Takes care of the START packets that the file_decoder will receive
class StartPacketData(sys_data.SysData):
    """
    The StartPacketData class stores the start packet information
    """

    # Initializes the start packet data with all of the necessary information
    def __init__(self, seqID, size, sourcePath, destPath):
        """
        Constructor.

        Args:
            packetType: The type of packet the object is
            seqID: The ID that identifies this specific object being created
            size: The size of the object
            lengthSP: The length of the source path
            sourcePath: The path that the file is coming from
            lengthDP: The length of the destination path
            destPath: The path that the file is ending up at

        Returns:
            An initialized StartPacketData object
        """
        super().__init__()
        self.packetType = FilePacketType.START
        self.seqID = seqID
        self.size = size
        # self.lengthSP = lengthSP
        self.sourcePath = sourcePath
        # self.lengthDP = lengthDP
        self.destPath = destPath


# Takes care of the DATA packets that the file_decoder will receive
class DataPacketData(sys_data.SysData):
    """
    The DataPacketData class stores the data packet information
    """

    # Initializes the data packet data with all of the necessary information
    def __init__(self, seqID, offset, dataVar):
        """
        Constructor.

        Args:
            packetType: The type of packet the object is
            seqID: The ID that identifies this specific object being created
            offset: How far off the object is from the original location
            length: The length of the object
            dataVar: Contains the rest of the data for the object

        Returns:
            An initialized DataPacketData object
        """
        super().__init__()
        self.packetType = FilePacketType.DATA
        self.seqID = seqID
        self.offset = offset
        self.dataVar = dataVar
        self.length = len(self.dataVar)


# Takes care of the END packets that the file_decoder will receive
class EndPacketData(sys_data.SysData):
    """
    The EndPacketData class stores the end packet information
    """

    # Initializes the end packet data with all of the necessary information
    def __init__(self, seqID, hashValue):
        """
        Constructor.

        Args:
            packetType: The type of packet the object is
            seqID: The ID that identifies this specific object being created
            hashValue: THe 32 bit has value described in the CFPD protocol

        Returns:
            An initialized EndPacketData object
        """
        super().__init__()
        self.packetType = FilePacketType.END
        self.seqID = seqID
        self.hashValue = hashValue


# Takes care of the CANCEL packets that the file_decoder will receive
class CancelPacketData(sys_data.SysData):
    """
    The CancelPacketData class stores the cancel packet information
    """

    # Initializes the cancel packet data with all of the necessary information
    def __init__(self, seqID):
        """
        Constructor.

        Args:
            packetType: The type of packet the object is
            seqID: The ID that identifies this specific object being created

        Returns:
            An initialized StartPacketData object
        """
        super().__init__()
        self.packetType = FilePacketType.CANCEL
        self.seqID = seqID
