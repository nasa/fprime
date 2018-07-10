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
    | Length of descriptor, opcode,  |
    | and argument data              |
    | (4 bytes)                      |
    +--------------------------------+
    | Descriptor = 0                 |
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
from data_types import cmd_data
from models.serialize import u32_type


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

    def data_callback
