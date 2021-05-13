"""
@brief Channel Decoder class used to parse binary channel telemetry data

Decoders are responsible for taking in serialized data and parsing it into
objects. Decoders receive serialized data (that had a specific descriptor) from
a distributer that it has been registered to. The distributer will send the
binary data after removing any length and descriptor headers.

Example data that would be sent to a decoder that parses channels:
    +-------------------+---------------------+------------ - - -
    | ID (4 bytes) | Time Tag (11 bytes) | Data....
    +-------------------+---------------------+------------ - - -

@date Created July 11, 2018
@author R. Joseph Paetz

@bug No known bugs
"""

import copy

from fprime.common.models.serialize.time_type import TimeType
from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.decoders.decoder import Decoder
from fprime_gds.common.utils import config_manager


class ChDecoder(Decoder):
    """Decoder class for Channel data"""

    def __init__(self, ch_dict, config):
        """
        ChDecoder class constructor

        Args:
            ch_dict: Channel telemetry dictionary. Channel IDs should be keys
                     and ChTemplate objects should be values

        Returns:
            An initialized channel decoder object.
        """
        super().__init__()

        if config is None:
            # Retrieve singleton for the configs
            config = config_manager.ConfigManager().get_instance()

        self.__dict = ch_dict
        self.id_obj = config.get_type("ch_id")

    def decode_api(self, data):
        """
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        Args:
            data: Binary telemetry channel data to decode

        Returns:
            Parsed version of the channel telemetry data in the form of a
            ChData object or None if the data is not decodable
        """
        ptr = 0

        # Decode Ch ID here...
        self.id_obj.deserialize(data, ptr)
        ptr += self.id_obj.getSize()
        ch_id = self.id_obj.val

        # Decode time...
        ch_time = TimeType()
        ch_time.deserialize(data, ptr)
        ptr += ch_time.getSize()

        if ch_id in self.__dict:
            # Retrieve the template instance for this channel
            ch_temp = self.__dict[ch_id]

            val_obj = self.decode_ch_val(data, ptr, ch_temp)

            return ChData(val_obj, ch_time, ch_temp)
        else:
            print("Channel decode error: id %d not in dictionary" % ch_id)
            return None

    @staticmethod
    def decode_ch_val(val_data, offset, template):
        """
        Decodes the given channel's value from the given data

        Args:
            val_data: Data to parse the value out of
            offset: Location in val_data to start the parsing
            template: Channel Template object for the channel

        Returns:
            The channel's value as an instance of a class derived from
            the BaseType class. The val_data has been deserialized using this
            object, and so the channel value can be retrieved from the obj's
            val field.
        """
        # This line creates a new object of the same type as the template's
        # type_obj. This allows us to use the new object to deserialize and
        # store the data value. If we did not do this, the template's object
        # would be used to deserialize multiple times and channel objects
        # referencing the template's type object would seem to have their value
        # changed randomly
        val_obj = copy.deepcopy(template.get_type_obj())

        val_obj.deserialize(val_data, offset)

        return val_obj
