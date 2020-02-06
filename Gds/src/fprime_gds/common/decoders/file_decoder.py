'''
@brief Decoder for file data

This decoder takes in serialized files, parses them, and packages the results
in file_data objects.

@date Created June 12, 2019
@author Blake A. Harriman

@bug No known bugs
'''
from __future__ import print_function
from struct import *

from fprime_gds.common.data_types.file_data import FilePacketType
from fprime_gds.common.decoders import decoder
from fprime_gds.common.data_types import file_data


#More info about File packets can be found at fprime-sw/Fw/FilePacket/docs/sdd.md
#Enum class for Packet Types

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
        packetType = FilePacketType(unpack('B', data[:1])[0]).name
        seqID = unpack('>I', data[1:5])[0]

        #Packet Type determines the variables following the seqID
        if (packetType == 'START'):  #Packet Type is START
            size = unpack('>I', data[5:9])[0]
            lengthSP = unpack('B', data[9])[0] #Length of the source path
            sourcePath = data[10:lengthSP + 10]
            lengthDP = unpack('B', data[lengthSP + 10])[0] #Length of the destination path
            destPath = data[lengthSP + 11: lengthSP + lengthDP + 11]

            return file_data.StartPacketData(seqID, size, lengthSP, sourcePath, lengthDP, destPath)
        elif (packetType == 'DATA'):   #Packet Type is DATA
            offset = unpack('>I', data[5:9])[0]
            length = unpack('BB', data[9:11])[0]
            dataVar = data[11:]
                
            return file_data.DataPacketData(seqID, offset, length, dataVar)
        elif (packetType == 'END'):   #Packet Type is END
            hashValue = unpack('>I', data[5:9])[0]

            return file_data.EndPacketData(seqID, hashValue)
        elif (packetType == 'CANCEL'):   #Packet Type is CANCEL
            #CANCEL Packets have no data
            return file_data.CancelPacketData(seqID)

        #The data was not determined to be any of the packet types so return none
        return None
