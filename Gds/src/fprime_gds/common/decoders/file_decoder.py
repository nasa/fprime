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
import traceback
import datetime
import os

#More info about File packets can be found at fprime-sw/Fw/FilePacket/docs/sdd.md
#Enum class for Packet Types
class PacketType(Enum):
    START = 0
    DATA = 1
    END = 2
    CANCEL = 3

#TestConsumer class for testing the file decoder
class TestConsumer:
    def data_callback(self, data):
        pass
        #new_path = '/home/blake/Documents/' + file_data.StartPacketData(data).fileDest
        #print(new_path)
        #print(new_path)
        #new_file = open(new_path, 'w')
        #new_file.write(str(data))
        #print(type(data))

#Main FileDecoder class
class FileDecoder(decoder.Decoder):
    '''Decoder class for file data'''

    def __init__(self):
        '''
        FileDecoder class constructor

        Args:
            file: Files.  Take in the file packet as an argument for the constructor
            to be decoded later on in this script

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
        #decode_api still needs work I think

        #Decode file here
        packetType = PacketType(unpack('B', data[:1])[0]).name
        seqID = unpack('I', data[1:5])[0]
        
        #Packet Type determines the variables following the seqID
        if (packetType == 'START'):  #Packet Type is START
            print(data)
            size = unpack('I', data[5:9])[0]
            lengthSP = unpack('B', data[9])[0] #Length of the source path
            print(lengthSP)
            sourcePath = data[10:lengthSP + 10]
            lengthDP = unpack('B', data[lengthSP + 10])[0] #Length of the destination path
            print(lengthDP)
            destPath = data[lengthSP + 10: lengthSP + lengthDP + 11]
            print(sourcePath)
            print(destPath)
            self.create_log_file(sourcePath, lengthDP)

            return file_data.StartPacketData(packetType, seqID, size, lengthSP, sourcePath, lengthDP, destPath)

        elif (packetType == 'DATA'):   #Packet Type is DATA
            offset = unpack('I', data[5:9])[0]
            length = unpack('BB', data[9:11])[0]
            dataVar = data[11:]
            return file_data.DataPacketData(packetType, seqID, offset, length, dataVar)

        elif (packetType == 'END'):   #Packet Type is END
            hashValue = unpack('I', data[5:9])[0]
            return file_data.EndPacketData(packetType, seqID, hashValue)

        elif (packetType == 'CANCEL'):   #Packet Type is CANCEL
            #CANCEL Packets have no data
            return file_data.CancelPacketData(packetType, seqID)

        #The data was not determined to be any of the packet types so return none
        return None

    def create_log_file(self, sourcePath, lengthDP):
        #Write the source path and dest path size to a log file
        print(os.getcwd())
        log_path = os.getcwd() + '/file_decoder_logs'
        try:  
            os.makedirs(log_path)
        except OSError:  
            pass
        else:  
            pass

        #Get the current date time to create a new log file folder
        time = datetime.datetime.now()
        log_path = log_path + '/' + str(time.year) + '_' + str(time.month) + '_' + str(time.day) + '_' + str(time.hour) + '_' + str(time.minute)
        try:  
            os.makedirs(log_path)
        except OSError:  
            pass
        else:  
            pass

        new_log = log_path + '/log_file'
        new_file = open(new_log, 'w')
        new_file.write('Source Path: ' + str(sourcePath) + '\n')
        new_file.write('Destination Size: ' + str(lengthDP))



if __name__ == "__main__":
    pass

