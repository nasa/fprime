'''
@brief Encoder for cmd data

This encoder takes in cmd_data objects, serializes them, and sends the results
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
    | (0x5A5A5A5A)                   |
    | (4 byte number)                |
    +--------------------------------+
    | Length of descriptor, opcode,  |
    | and argument data              |
    | (4 bytes)                      |
    +--------------------------------+
    | Descriptor type = 0            |
    | (4 bytes)                      |
    +--------------------------------+
    | Op code                        |
    | (4 bytes)                      |
    +--------------------------------+
    | Argument 1 value               |
    +--------------------------------+
    | Argument 2 value               |
    +--------------------------------+
    | ...                            |
    +--------------------------------+
    | Argument n value               |
    +--------------------------------+

@date Created July 9, 2018
@author R. Joseph Paetz

@bug No known bugs
'''
from __future__ import absolute_import

from . import encoder
from fprime_gds.common.data_types.cmd_data import CmdData
from fprime.common.models.serialize.u32_type import U32Type
from fprime_gds.common.utils.data_desc_type import DataDescType

import binascii

class CmdEncoder(encoder.Encoder):
    '''Encoder class for command data'''

    def __init__(self, dest="FSW", config=None):
        '''
        CmdEncoder class constructor

        Args:
            dest (string, "FSW" or "GUI", default="FSW"): Destination for binary
                  data produced by encoder.
            config (ConfigManager, default=None): Object with configuration data
                    for the sizes of fields in the binary data. If None passed,
                    defaults are used.

        Returns:
            An initialized CmdEncoder object
        '''
        super(CmdEncoder, self).__init__(dest, config)

        self.len_obj = self.config.get_type("msg_len")


    def data_callback(self, data):
        '''
        Function called to pass data through the encoder.

        Objects that the encoder is registered to will call this function to
        pass data to the encoder.

        Args:
            data: CmdData object to encode into binary data.
        '''
        self.send_to_all(self.encode_api(data))


    def encode_api(self, data):
        '''
        Encodes the given CmdData object as binary data and returns the result.

        Args:
            data: CmdData object to encode

        Returns:
            Encoded version of the data argument as binary data
        '''
        # TODO we should be able to handle multiple destinations, not just FSW
        cmd_temp = data.get_template()

        desc = U32Type( 0x5A5A5A5A ).serialize()

        descriptor = U32Type(DataDescType["FW_PACKET_COMMAND"].value).serialize()

        op_code = U32Type(cmd_temp.get_op_code()).serialize()

        arg_data = b""
        for arg in data.get_args():
            arg_data += arg.serialize()

        length_val = len(descriptor) + len(op_code) + len(arg_data)
        self.len_obj.val = length_val
        length = self.len_obj.serialize()

        binary_data = (desc + length + descriptor + op_code + arg_data)

        return binary_data

