"""
@brief Encoder for file data

This encoder takes in file objects, serializes them, and sends the results
to all registered senders.

Serialized command format:
    +--------------------------------+          -
    | Header = "A5A5 "               |          |
    | (5 byte string)                |          |
    +--------------------------------+      Added by
    | Destination = "GUI " or "FSW " |       Sender
    | (4 byte string)                |          |
    +--------------------------------+          -
    | Command descriptor             |
    | (0x????????)                   |
    | (4 byte number)                |
    +--------------------------------+
    | Length of descriptor, opcode,  |
    | and argument data              |
    | (4 bytes)                      |
    +--------------------------------+
    | Packet type                    |
    | (4 bytes)                      |
    +--------------------------------+
    | Sequence ID                    |
    | (4 bytes)                      |
    +--------------------------------+
    | Type specific                  |
    +--------------------------------+
    | Type specific                  |
    +--------------------------------+
    | ...                            |
    +--------------------------------+
    | Type specific                  |
    +--------------------------------+

@date Created February 2020-02-05
@author mstarch

@bug No known bugs
"""

import struct

from fprime.common.models.serialize.numerical_types import U32Type
from fprime.constants import DATA_ENCODING
from fprime_gds.common.data_types.file_data import FilePacketType
from fprime_gds.common.utils.data_desc_type import DataDescType

from . import encoder


class FileEncoder(encoder.Encoder):
    """
    Encodes the file data. This plugs into the uplink system and allows for data to be sent to the spacecraft.
    """

    def __init__(self, config=None):
        """
        Constructs a file encoder. Defaults to FSW as destination..

        Args:
            config (ConfigManager, default=None): Object with configuration data
                    for the sizes of fields in the binary data. If None passed,
                    defaults are used.
        """
        super().__init__(config)
        self.len_obj = self.config.get_type("msg_len")

    def encode_api(self, data):
        """
        Encodes specific file packets. This will allow the data to be sent out.

        :param data: FilePacket type to send.
        :return: encoded bytes data
        """
        out_data = struct.pack(">BI", int(data.packetType.value), data.seqID)
        # Packet Type determines the variables following the seqID
        if data.packetType == FilePacketType.START:
            file_src_enc = data.sourcePath.encode(DATA_ENCODING)
            file_dst_enc = data.destPath.encode(DATA_ENCODING)
            out_data += struct.pack(">IB", data.size, len(file_src_enc))
            out_data += file_src_enc
            out_data += struct.pack(">B", len(file_dst_enc))
            out_data += file_dst_enc
        elif data.packetType == FilePacketType.DATA:
            out_data += struct.pack(">IH", data.offset, data.length)
            out_data += data.dataVar
        elif data.packetType == FilePacketType.END:
            out_data += struct.pack(">I", data.hashValue)
        elif data.packetType != FilePacketType.CANCEL:
            raise Exception(
                "Invalid packet type found while encoding: {}".format(data.packetType)
            )
        descriptor = U32Type(DataDescType["FW_PACKET_FILE"].value).serialize()
        length_obj = self.config.get_type("msg_len")
        length_obj.val = len(descriptor) + len(out_data)
        header = U32Type(0x5A5A5A5A).serialize() + length_obj.serialize() + descriptor
        return header + out_data
