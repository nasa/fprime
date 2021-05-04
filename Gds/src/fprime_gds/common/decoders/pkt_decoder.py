"""
@brief Packet Decoder class used to parse binary packetized telemetry data

Decoders are responsible for taking in serialized data and parsing it into
objects. Decoders receive serialized data (that had a specific descriptor) from
a distributer that it has been registered to. The distributer will send the
binary data after removing any length and descriptor headers.

Example data that would be sent to a decoder that parses events or channels:
    +-------------------+---------------------+------------ - - -
    | ID (2 bytes) | Time Tag (11 bytes) | Data....
    +-------------------+---------------------+------------ - - -

@date Created July 12, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

from fprime.common.models.serialize.time_type import TimeType
from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.data_types.pkt_data import PktData
from fprime_gds.common.decoders.ch_decoder import ChDecoder
from fprime_gds.common.utils import config_manager


class PktDecoder(ChDecoder):
    """Decoder class for Packetized Telemetry data"""

    def __init__(self, pkt_name_dict, ch_dict, config=None):
        """
        Constructor

        Args:
            pkt_name_dict: (dict) Packet dictionary. Pkt names should be keys
                                  and PktTemplate objects should be values
            ch_dict: (dict) Channel dictionary. Channel ids should be keys and
                            ChTemplate objects should be values

        Returns:
            An initialized PktDecoder object
        """
        if config is None:
            config = config_manager.ConfigManager().get_instance()
        super().__init__(ch_dict, config)

        self.__dict = pkt_name_dict
        self.id_obj = config.get_type("pkt_id")

    def decode_api(self, data):
        """
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        Args:
            data: (bytearray) Binary packetized telemetry data to decode

        Returns:
            Parsed version of the input data in the form of a PktData object
            or None if the data is not decodable
        """
        ptr = 0

        # Decode Pkt ID here...
        self.id_obj.deserialize(data, ptr)
        ptr += self.id_obj.getSize()
        pkt_id = self.id_obj.val

        # Decode time...
        pkt_time = TimeType()
        pkt_time.deserialize(data, ptr)
        ptr += pkt_time.getSize()

        if pkt_id not in self.__dict:
            # Don't crash if can't find pkt. Just notify and keep going
            print(
                "Packet decode error: id %d not in dictionary. Time=%s"
                % (pkt_id, pkt_time.to_readable())
            )
            print("Full pkt = \n")
            for i in data:
                print("0x%02x" % ord(i))

            return None

        # Retrieve the template instance for this channel
        pkt_temp = self.__dict[pkt_id]

        ch_temps = pkt_temp.get_ch_list()

        ch_data_objs = []
        for ch_temp in ch_temps:
            val_obj = self.decode_ch_val(data, ptr, ch_temp)
            ptr += val_obj.getSize()
            ch_data_objs.append(ChData(val_obj, pkt_time, ch_temp))

        return PktData(ch_data_objs, pkt_time, pkt_temp)
