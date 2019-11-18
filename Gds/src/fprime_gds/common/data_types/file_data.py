'''
@brief Class to store data from each specific file_data type

@date Created July 10, 2019
@author Blake A. Harriman

@bug No known bugs
'''

from fprime_gds.common.data_types import sys_data
from fprime.common.models.serialize import time_type

#Takes care of the START packets that the file_decoder will receive
class StartPacketData(sys_data.SysData):
    '''
    The StartPacketData class stores the start packet information
    '''
    #Initializes the start packet data with all of the necessary information
    def __init__(self, packetType, seqID, size, lengthSP, sourcePath, lengthDP, destPath):
        '''
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
        '''

        self.packetType = packetType
        self.seqID = seqID
        self.size = size
        self.lengthSP = lengthSP
        self.sourcePath = sourcePath
        self.lengthDP = lengthDP
        self.destPath = destPath

#Takes care of the DATA packets that the file_decoder will receive
class DataPacketData(sys_data.SysData):
    '''
    The DataPacketData class stores the data packet information
    '''
    #Initializes the data packet data with all of the necessary information
    def __init__(self, packetType, seqID, offset, length, dataVar):
        '''
        Constructor.

        Args:
            packetType: The type of packet the object is
            seqID: The ID that identifies this specific object being created
            offset: How far off the object is from the original location
            length: The length of the object
            dataVar: Contains the rest of the data for the object

        Returns:
            An initialized DataPacketData object
        '''

        self.packetType = packetType
        self.seqID = seqID
        self.offset = offset
        self.length = length
        self.dataVar = dataVar

#Takes care of the END packets that the file_decoder will receive
class EndPacketData(sys_data.SysData):
    '''
    The EndPacketData class stores the end packet information
    '''
    #Initializes the end packet data with all of the necessary information
    def __init__(self, packetType, seqID, hashValue):
        '''
        Constructor.

        Args:
            packetType: The type of packet the object is
            seqID: The ID that identifies this specific object being created
            hashValue: THe 32 bit has value described in the CFPD protocol

        Returns:
            An initialized EndPacketData object
        '''

        self.packetType = packetType
        self.seqID = seqID
        self.hashValue = hashValue

#Takes care of the CANCEL packets that the file_decoder will receive
class CancelPacketData(sys_data.SysData):
    '''
    The CancelPacketData class stores the cancel packet information
    '''
    #Initializes the cancel packet data with all of the necessary information
    def __init__(self, packetType, seqID):
        '''
        Constructor.

        Args:
            packetType: The type of packet the object is
            seqID: The ID that identifies this specific object being created

        Returns:
            An initialized StartPacketData object
        '''

        self.packetType = packetType
        self.seqID = seqID


