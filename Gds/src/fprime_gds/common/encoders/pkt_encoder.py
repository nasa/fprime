'''
@brief Encoder for Packet data

This encoder takes in PktData objects, serializes them, and sends the results
to all registered senders.

Serialized Packet format:
    +--------------------------------+          -
    | Header = "A5A5 "               |          |
    | (5 byte string)                |          |
    +--------------------------------+      Added by
    | Destination = "GUI " or "FSW " |       Sender
    | (4 byte string)                |          |
    +--------------------------------+          -
    | Length of descriptor, ID,      |
    | and channel data               |
    | (variable bytes, check config) |
    +--------------------------------+
    | Descriptor type = 4            |
    | (4 bytes)                      |
    +--------------------------------+
    | ID                             |
    | (2 bytes)                      |
    +--------------------------------+
    | Channel 1 value                |
    +--------------------------------+
    | Channel 2 value                |
    +--------------------------------+
    | ...                            |
    +--------------------------------+
    | Channel n value                |
    +--------------------------------+

@date Created August 9, 2018
@author R. Joseph Paetz

@bug No known bugs
'''
from __future__ import print_function
from __future__ import absolute_import

from .encoder import Encoder
from fprime_gds.common.data_types.pkt_data import PktData
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.utils.data_desc_type import DataDescType

# For Unit Test
import sys
from fprime_gds.common.templates.ch_template import ChTemplate
from fprime_gds.common.templates.pkt_template import PktTemplate
from fprime_gds.common.data_types.ch_data import ChData
from fprime.common.models.serialize.time_type import TimeType
from fprime.common.models.serialize.u8_type import U8Type
from fprime.common.models.serialize.u16_type import U16Type
from fprime.common.models.serialize.u32_type import U32Type


class PktEncoder(Encoder):
    '''Encoder class for packet data'''

    def __init__(self, dest="GUI", config=None):
        '''
        Constructor

        Args:
            dest (string, "FSW" or "GUI", default="GUI"): Destination for binary
                  data produced by encoder.
            config (ConfigManager, default=None): Object with configuration data
                    for the sizes of fields in the binary data. If None passed,
                    defaults are used.

        Returns:
            An initialized PktEncoder object
        '''
        super(PktEncoder, self).__init__(dest, config)

        self.len_obj = self.config.get_type("msg_len")
        self.desc_obj = self.config.get_type("msg_desc")
        self.id_obj = self.config.get_type("pkt_id")


    def data_callback(self, data):
        '''
        Function called to pass data through the encoder.

        Objects that the encoder is registered to will call this function to
        pass data to the encoder. This function will then send the encoded
        data to all registered senders.

        Args:
            data (PktData obj): object to encode into binary data.
        '''
        self.send_to_all(self.encode_api(data))


    def encode_api(self, data):
        '''
        Encodes the given PktData object as binary data and returns the result.

        Args:
            data (PktData obj): object to encode

        Returns:
            Encoded version of the data argument as binary data
        '''
        # TODO Should we verify that it is a PktData object? Or is that too much
        #      overhead.
        pkt_temp = data.get_template()

        self.desc_obj.val = DataDescType["FW_PACKET_PACKETIZED_TLM"].value
        desc_bin = self.desc_obj.serialize()

        self.id_obj.val = pkt_temp.get_id()
        id_bin = self.id_obj.serialize()

        time_bin = data.get_time().serialize()

        ch_bin = ""
        for ch in data.get_chs():
            ch_bin += ch.get_val_obj().serialize()

        len_val = len(desc_bin) + len(id_bin) + len(time_bin) + len(ch_bin)
        self.len_obj.val = len_val
        len_bin = self.len_obj.serialize()

        binary_data = (len_bin + desc_bin + id_bin + time_bin + ch_bin)

        return binary_data


if __name__ == "__main__":
    # Unit Tests
    config = ConfigManager()
    config.set('types', 'msg_len', 'U16')

    enc = PktEncoder()
    enc_config = PktEncoder("GUI", config)

    ch_temp_1 = ChTemplate(101, "test_ch", "test_comp", U32Type())
    ch_temp_2 = ChTemplate(102, "test_ch2", "test_comp2", U8Type())
    ch_temp_3 = ChTemplate(103, "test_ch3", "test_comp3", U16Type())

    pkt_temp = PktTemplate(64, "test_pkt", [ch_temp_1, ch_temp_2, ch_temp_3])

    time_obj = TimeType(2, 0, 1533758629, 123456)

    ch_obj_1 = ChData(U32Type(1356), time_obj, ch_temp_1)
    ch_obj_2 = ChData(U8Type(143), time_obj, ch_temp_2)
    ch_obj_3 = ChData(U16Type(1509), time_obj, ch_temp_3)

    pkt_obj = PktData([ch_obj_1, ch_obj_2, ch_obj_3], time_obj, pkt_temp)

    desc_bin = "\x00\x00\x00\x04"
    id_bin = "\x00\x40"
    time_bin = "\x00\x02\x00\x5b\x6b\x4c\xa5\x00\x01\xe2\x40"
    ch_bin = "\x00\x00\x05\x4c\x8F\x05\xe5"
    long_len_bin = "\x00\x00\x00\x18"
    short_len_bin = "\x00\x18"

    reg_expected = (long_len_bin + desc_bin + id_bin + time_bin + ch_bin)
    config_expected = (short_len_bin + desc_bin + id_bin + time_bin + ch_bin)

    reg_output = enc.encode_api(pkt_obj)

    if (reg_output != reg_expected):
        print ("FAIL: expected regular output to be %s, but found %s"%
               (list(reg_expected), list(reg_output)))
        sys.exit(-1)
    else:
        print("PASSED test 1")

    config_output = enc_config.encode_api(pkt_obj)

    if (config_output != config_expected):
        print("FAIL: expected configured output to be %s, but found %s"%
              (list(config_expected), list(config_output)))
        sys.exit(-1)
    else:
        print("PASSED test 2")

    print("ALL TESTS PASSED!")

