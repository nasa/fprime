'''
@brief Decoder for file data

This decoder takes in serialized files, parses them, and packages the results
in file_data objects.

@date Created June 12, 2019
@author Blake A. Harriman

@bug No known bugs
'''
from __future__ import print_function
import copy
from struct import *
from enum import Enum
from fprime_gds.common.decoders import decoder
from fprime_gds.common.data_types import file_data
from fprime.common.models.serialize import u32_type
from fprime.common.models.serialize import time_type
from fprime.common.models.serialize.type_exceptions import *
import multiprocessing as mp
from multiprocessing import Process
import threading
import time
import traceback
import datetime
import os
import random

#More info about File packets can be found at fprime-sw/Fw/FilePacket/docs/sdd.md
#Enum class for Packet Types
class PacketType(Enum):
    START = 0
    DATA = 1
    END = 2
    CANCEL = 3

#Main FileDecoder class
class FileDecoder(decoder.Decoder):
    '''Decoder class for file data'''
    def __init__(self):
        '''
        FileDecoder class constructor

        Args:
            None
        Returns:
            An initialized FileDecoder object.
        '''

        super(FileDecoder, self).__init__()


    def data_callback(self, data):
        '''
        Function called to pass data to the decoder class

        Args:
            data: Binary data to decode and pass to registered consumers
        '''

        result = self.decode_api(data)

        # Make sure we don't send None data
        if result != None:
            self.send_to_all(result)


    def decode_api(self, data):
        '''
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        Args:
            data: Binary data to decode (array of bytes)

        Returns:
            Parsed version of the file data in the form of a File object
            or None if the data is not decodable
        '''

        #Decode file here
        packetType = PacketType(unpack('B', data[:1])[0]).name
        seqID = unpack('>I', data[1:5])[0]

        #Packet Type determines the variables following the seqID
        if (packetType == 'START'):  #Packet Type is START
            size = unpack('>I', data[5:9])[0]
            lengthSP = unpack('B', data[9])[0] #Length of the source path
            sourcePath = data[10:lengthSP + 10]
            lengthDP = unpack('B', data[lengthSP + 10])[0] #Length of the destination path
            destPath = data[lengthSP + 11: lengthSP + lengthDP + 11]

            return file_data.StartPacketData(packetType, seqID, size, lengthSP, sourcePath, lengthDP, destPath)
        elif (packetType == 'DATA'):   #Packet Type is DATA
            offset = unpack('>I', data[5:9])[0]
            length = unpack('BB', data[9:11])[0]
            dataVar = data[11:]
                
            return file_data.DataPacketData(packetType, seqID, offset, length, dataVar)
        elif (packetType == 'END'):   #Packet Type is END
            hashValue = unpack('>I', data[5:9])[0]

            return file_data.EndPacketData(packetType, seqID, hashValue)
        elif (packetType == 'CANCEL'):   #Packet Type is CANCEL
            #CANCEL Packets have no data
            return file_data.CancelPacketData(packetType, seqID)

        #The data was not determined to be any of the packet types so return none
        return None
    

if __name__ == "__main__":
    pass