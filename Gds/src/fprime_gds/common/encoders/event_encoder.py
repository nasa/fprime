"""
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
"""

from .encoder import Encoder
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.utils.data_desc_type import DataDescType


class EventEncoder(Encoder):
    """Encoder class for event data"""

    def __init__(self, config=None):
        """
        Constructor

        Args:
            config (ConfigManager, default=None): Object with configuration data
                    for the sizes of fields in the binary data. If None passed,
                    defaults are used.

        Returns:
            An initialized EventEncoder object
        """
        super().__init__(config)

        self.len_obj = self.config.get_type("msg_len")
        self.desc_obj = self.config.get_type("msg_desc")
        self.id_obj = self.config.get_type("event_id")

    def encode_api(self, data):
        """
        Encodes the given EventData object as binary data and returns the result.

        Args:
            data (EventData obj): A non-empty EventData object to encode

        Returns:
            Encoded version of the data argument as binary data
        """
        assert isinstance(data, EventData), "Encoder handling incorrect type"
        event_temp = data.get_template()

        self.desc_obj.val = DataDescType["FW_PACKET_LOG"].value
        desc_bin = self.desc_obj.serialize()

        self.id_obj.val = event_temp.get_id()
        id_bin = self.id_obj.serialize()

        time_bin = data.get_time().serialize()

        arg_bin = b""
        for arg in data.get_args():
            arg_bin += arg.serialize()

        len_val = len(desc_bin) + len(id_bin) + len(time_bin) + len(arg_bin)
        self.len_obj.val = len_val
        len_bin = self.len_obj.serialize()

        binary_data = len_bin + desc_bin + id_bin + time_bin + arg_bin

        return binary_data
