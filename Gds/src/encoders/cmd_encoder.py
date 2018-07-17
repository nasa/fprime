'''
@brief Encoder for cmd data

This encoder takes in cmd_data objects, serializes them, and sends the results
to all registered senders.

Serialized command format:
    +--------------------------------+
    | Header = "A5A5 "               |
    | (5 byte string)                |
    +--------------------------------+
    | Destination = "GUI " or "FSW " |
    | (4 byte string)                |
    +--------------------------------+
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

import encoder
from data_types.cmd_data import CmdData
from models.serialize.u32_type import U32Type
from utils.data_desc_type import DataDescType

class CmdEncoder(encoder.Encoder):
    '''Encoder class for command data'''

    def __init__(self, cmd_dict):
        '''
        CmdEncoder class constructor

        Args:
            cmd_dict: Command dictionary. Command op codes should be keys and
                      CmdTemplate objects should be values

        Returns:
            An initialized CmdEncoder object
        '''
        super(CmdEncoder, self).__init__()

        self.__dict = cmd_dict

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

        arg_data = ""
        for arg in data.get_args():
            arg_data += arg.serialize()

        length = U32Type(len(descriptor) + len(op_code) + len(arg_data)).serialize()

        binary_data = (desc + length + descriptor + op_code + arg_data)

        return binary_data

