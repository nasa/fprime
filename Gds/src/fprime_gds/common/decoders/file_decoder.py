"""
@brief Decoder for file data

This decoder takes in serialized files, parses them, and packages the results
in file_data objects.

@date Created June 12, 2019
@author Blake A. Harriman

@bug No known bugs
"""

import struct

from fprime_gds.common.data_types import file_data
from fprime_gds.common.data_types.file_data import FilePacketType
from fprime_gds.common.decoders import decoder

# More info about File packets can be found at fprime-sw/Fw/FilePacket/docs/sdd.md
# Enum class for Packet Types


# Main FileDecoder class
class FileDecoder(decoder.Decoder):
    """Decoder class for file data"""

    def decode_api(self, data):
        """
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        Args:
            data: Binary data to decode (array of bytes)

        Returns:
            Parsed version of the file data in the form of a File object
            or None if the data is not decodable
        """

        # Decode file here
        packetTypeNum, seqID = struct.unpack_from(">BI", data, 0)
        packetType = FilePacketType(packetTypeNum).name

        # Packet Type determines the variables following the seqID
        if packetType == "START":  # Packet Type is START
            fileSize, sourcePathSize = struct.unpack_from(">IB", data, 5)
            sourcePath = data[10: sourcePathSize + 10]
            (destPathSize,) = struct.unpack_from(">B", data, sourcePathSize + 10)
            destPath = data[sourcePathSize + 11: sourcePathSize + destPathSize + 11]
            return file_data.StartPacketData(seqID, fileSize, sourcePath, destPath)
        elif packetType == "DATA":  # Packet Type is DATA
            offset, length = struct.unpack_from(">IH", data, 5)
            dataVar = data[11: 11 + length]
            return file_data.DataPacketData(seqID, offset, dataVar)
        elif packetType == "END":  # Packet Type is END
            hashValue = struct.unpack_from(">I", data, 5)
            return file_data.EndPacketData(seqID, hashValue)
        elif packetType == "CANCEL":  # Packet Type is CANCEL
            # CANCEL Packets have no data
            return file_data.CancelPacketData(seqID)
        # The data was not determined to be any of the packet types so return none
        return None
