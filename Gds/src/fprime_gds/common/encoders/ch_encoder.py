"""
@brief Encoder for channel data

This encoder takes in ChData objects, serializes them, and sends the results to
all registered senders.

Serialized Ch format:
    +--------------------------------+          -
    | Header = "A5A5 "               |          |
    | (5 byte string)                |          |
    +--------------------------------+      Added by
    | Destination = "GUI " or "FSW " |       Sender
    | (4 byte string)                |          |
    +--------------------------------+          -
    | Length of descriptor, ID,      |
    | and value data                 |
    | (variable bytes, check config) |
    +--------------------------------+
    | Descriptor type = 1            |
    | (4 bytes)                      |
    +--------------------------------+
    | ID                             |
    | (4 bytes)                      |
    +--------------------------------+
    | Value                          |
    | (variable bytes, based on type)|
    +--------------------------------+

@date Created August 9, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from .encoder import Encoder
from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.utils.data_desc_type import DataDescType


class ChEncoder(Encoder):
    """Encoder class for channel data"""

    def __init__(self, config=None):
        """
        Constructor

        Args:
            config (ConfigManager, default=None): Object with configuration data
                    for the sizes of fields in the binary data. If None passed,
                    defaults are used.

        Returns:
            An initialized ChEncoder object
        """
        # sets up config
        super().__init__(config)

        self.len_obj = self.config.get_type("msg_len")
        self.desc_obj = self.config.get_type("msg_desc")
        self.id_obj = self.config.get_type("ch_id")

    def encode_api(self, data):
        """
        Encodes the given ChData object as binary data and returns the result.

        Args:
            data: A non-empty ChData object to encode

        Returns:
            Encoded version of the data argument as binary data
        """
        assert isinstance(data, ChData), "Encoder handling incorrect type"
        ch_temp = data.get_template()

        self.desc_obj.val = DataDescType["FW_PACKET_TELEM"].value
        desc_bin = self.desc_obj.serialize()

        self.id_obj.val = ch_temp.get_id()
        id_bin = self.id_obj.serialize()

        time_bin = data.get_time().serialize()

        val_bin = data.get_val_obj().serialize()

        len_val = len(desc_bin) + len(id_bin) + len(time_bin) + len(val_bin)
        self.len_obj.val = len_val
        len_bin = self.len_obj.serialize()

        binary_data = len_bin + desc_bin + id_bin + time_bin + val_bin

        return binary_data
