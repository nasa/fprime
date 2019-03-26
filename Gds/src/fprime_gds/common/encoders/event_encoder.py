'''
@brief Encoder for Event data

This encoder takes in EventData objects, serializes them, and sends the results
to all registered senders.

Serialized Event format:
    +--------------------------------+          -
    | Header = "A5A5 "               |          |
    | (5 byte string)                |          |
    +--------------------------------+      Added by
    | Destination = "GUI " or "FSW " |       Sender
    | (4 byte string)                |          |
    +--------------------------------+          -
    | Length of descriptor, ID,      |
    | and argument data              |
    | (variable bytes, check config) |
    +--------------------------------+
    | Descriptor type = 2            |
    | (4 bytes)                      |
    +--------------------------------+
    | ID                             |
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

@date Created August 9, 2018
@author R. Joseph Paetz

@bug No known bugs
'''
from __future__ import print_function
from __future__ import absolute_import

from .encoder import Encoder
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.utils.data_desc_type import DataDescType

# For unit tests
import sys
from fprime_gds.common.templates.event_template import EventTemplate
from fprime.common.models.serialize.time_type import TimeType
from fprime.common.models.serialize.u8_type import U8Type
from fprime.common.models.serialize.u16_type import U16Type
from fprime.common.models.serialize.u32_type import U32Type
from fprime_gds.common.utils.event_severity import EventSeverity


class EventEncoder(Encoder):
    '''Encoder class for event data'''

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
            An initialized EventEncoder object
        '''
        super(EventEncoder, self).__init__(dest, config)

        self.len_obj = self.config.get_type("msg_len")
        self.desc_obj = self.config.get_type("msg_desc")
        self.id_obj = self.config.get_type("event_id")


    def data_callback(self, data):
        '''
        Function called to pass data through the encoder.

        Objects that the encoder is registered to will call this function to
        pass data to the encoder. This function will then send the encoded
        data to all registered senders.

        Args:
            data (EventData obj): A non-empty EventData obj to encode to binary
        '''
        self.send_to_all(self.encode_api(data))


    def encode_api(self, data):
        '''
        Encodes the given EventData object as binary data and returns the result.

        Args:
            data (EventData obj): A non-empty EventData object to encode

        Returns:
            Encoded version of the data argument as binary data
        '''
        # TODO Should we verify that it is a EventData object? Or is that too
        #      much overhead. Also, should we verify the object is non-empty
        #      (aka its arguments field is non-None)
        event_temp = data.get_template()

        self.desc_obj.val = DataDescType["FW_PACKET_LOG"].value
        desc_bin = self.desc_obj.serialize()

        self.id_obj.val = event_temp.get_id()
        id_bin = self.id_obj.serialize()

        time_bin = data.get_time().serialize()

        arg_bin = ""
        for arg in data.get_args():
            arg_bin += arg.serialize()

        len_val = len(desc_bin) + len(id_bin) + len(time_bin) + len(arg_bin)
        self.len_obj.val = len_val
        len_bin = self.len_obj.serialize()

        binary_data = (len_bin + desc_bin + id_bin + time_bin + arg_bin)

        return binary_data

if __name__ == "__main__":
    # Unit Tests
    config = ConfigManager()
    config.set('types', 'msg_len', 'U16')

    enc = EventEncoder()
    enc_config = EventEncoder("GUI", config)

    temp = EventTemplate(101, "test_ch", "test_comp",
                         [("a1", "a1", U32Type()), ("a2", "a2", U32Type())],
                         EventSeverity["DIAGNOSTIC"], "%d %d")

    time_obj = TimeType(2, 0, 1533758629, 123456)

    event_obj = EventData((U32Type(42), U32Type(10)), time_obj, temp)

    desc_bin = "\x00\x00\x00\x02"
    id_bin = "\x00\x00\x00\x65"
    time_bin = "\x00\x02\x00\x5b\x6b\x4c\xa5\x00\x01\xe2\x40"
    arg_bin = "\x00\x00\x00\x2a\x00\x00\x00\x0a"
    long_len_bin = "\x00\x00\x00\x1b"
    short_len_bin = "\x00\x1b"

    reg_expected = (long_len_bin + desc_bin + id_bin + time_bin + arg_bin)
    config_expected = (short_len_bin + desc_bin + id_bin + time_bin + arg_bin)

    reg_output = enc.encode_api(event_obj)

    if (reg_output != reg_expected):
        print ("FAIL: expected regular output to be %s, but found %s"%
               (list(reg_expected), list(reg_output)))
        sys.exit(-1)
    else:
        print("PASSED test 1")

    config_output = enc_config.encode_api(event_obj)

    if (config_output != config_expected):
        print("FAIL: expected configured output to be %s, but found %s"%
              (list(config_expected), list(config_output)))
        sys.exit(-1)
    else:
        print("PASSED test 2")


    temp = EventTemplate(102, "test_ch2", "test_comp2",
                        [("a1", "a1", U8Type()), ("a2", "a2", U16Type())],
                         EventSeverity["DIAGNOSTIC"], "%d %d")

    time_obj = TimeType(2, 0, 1533758628, 123457)

    event_obj = EventData((U8Type(128), U16Type(40)), time_obj, temp)

    desc_bin = "\x00\x00\x00\x02"
    id_bin = "\x00\x00\x00\x66"
    time_bin = "\x00\x02\x00\x5b\x6b\x4c\xa4\x00\x01\xe2\x41"
    arg_bin = "\x80\x00\x28"
    long_len_bin = "\x00\x00\x00\x16"
    short_len_bin = "\x00\x16"

    reg_expected = (long_len_bin + desc_bin + id_bin + time_bin + arg_bin)
    config_expected = (short_len_bin + desc_bin + id_bin + time_bin + arg_bin)

    reg_output = enc.encode_api(event_obj)

    if (reg_output != reg_expected):
        print ("FAIL: expected regular output to be %s, but found %s"%
               (list(reg_expected), list(reg_output)))
        sys.exit(-1)
    else:
        print("PASSED test 3")

    config_output = enc_config.encode_api(event_obj)

    if (config_output != config_expected):
        print("FAIL: expected configured output to be %s, but found %s"%
              (list(config_expected), list(config_output)))
        sys.exit(-1)
    else:
        print("PASSED test 4")

    print("ALL TESTS PASSED!")

