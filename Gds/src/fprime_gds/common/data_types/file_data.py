'''
@brief Class to store data from a specific event

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

from fprime_gds.common.data_types import sys_data
from fprime.common.models.serialize import time_type

class StartPacketData(sys_data.SysData):
    '''
    The StartPacketData class stores the start packet information
    '''

    def __init__(self, packetType, seqID, size, sourcePath, destPath):
        '''
        Constructor.

        Args:
            packetType: The type of packet the object is
            seqID: The ID that identifies this specific object being created
            size: The size of the object
            sourcePath: The path that the file is coming from
            destPath: The path that the file is ending up at

        Returns:
            An initialized StartPacketData object
        '''
        # TODO refactor so that arguments are actually of BaseType objects
        self.packetType = packetType
        self.seqID = seqID
        self.size = size
        self.sourcePath = sourcePath
        self.destPath = destPath


    def __str__(self):
        '''
        Convert the event data to a string

        Returns:
            String version of the channel data
        '''
        return self.get_str()

class DataPacketData(sys_data.SysData):
    '''
    The DataPacketData class stores the data packet information
    '''
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

        # TODO refactor so that arguments are actually of BaseType objects
        self.packetType = packetType
        self.seqID = seqID
        self.offset = offset
        self.length = length
        self.dataVar = dataVar

class EndPacketData(sys_data.SysData):
    '''
    The EndPacketData class stores the end packet information
    '''
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

        # TODO refactor so that arguments are actually of BaseType objects
        self.packetType = packetType
        self.seqID = seqID
        self.hashValue = hashValue

class CancelPacketData(sys_data.SysData):
    '''
    The CancelPacketData class stores the cancel packet information
    '''
    def __init__(self, packetType, seqID):
        '''
        Constructor.

        Args:
            packetType: The type of packet the object is
            seqID: The ID that identifies this specific object being created

        Returns:
            An initialized StartPacketData object
        '''

        # TODO refactor so that arguments are actually of BaseType objects
        self.packetType = packetType
        self.seqID = seqID


