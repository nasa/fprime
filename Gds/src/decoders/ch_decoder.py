'''
@brief Channel Decoder class used to parse binary channel telemetry data

Decoders are responsible for taking in serialized data and parsing it into
objects. Decoders receive serialized data (that had a specific descriptor) from
a distributer that it has been registered to. The distributer will send the
binary data after removing any length and descriptor headers.

Example data that would be sent to a decoder that parses channels:
    +-------------------+---------------------+------------ - - -
    | Lenghth (4 bytes) | Time Tag (11 bytes) | Data....
    +-------------------+---------------------+------------ - - -

@date Created July 2, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

import decoder


class ChDecoder(decoder.Decoder):
    '''Decoder class for Channel data'''

    def __init__(self, ch_dict):
        '''
        ChDecoder class constructor

        Args:
            ch_dict: Channel telemetry dictionary. Channel IDs should be keys
                     and ChTemplate objects should be values

        Returns:
            An initialized channel decoder object.
        '''
        super(ChDecoder, self).__init__()

        self.__dict = ch_dict


    def data_callback(self, data):
        '''
        Function called to pass data to the decoder class

        Args:
            data: Binary data to decode and pass to registered consumers
        '''
        self.send_to_all(self.decode_api(data))


    def decode_api(self, data):
        '''
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        Args:
            data: Binary data to decode

        Returns:
            Parsed version of the channel telemetry data in the form of a
            ChData object or None if the data is not decodable
        '''
        ptr = 0

        # Decode Ch ID here...
        id_obj = u32_type.u32Type()
        id_obj.deserialize(data, ptr)
        ptr += id_obj.getSize()
        ch_id = id_obj.val

        # Decode time...
        ch_time = time_type.TimeType()
        ch_time.deserialize(data, ptr)
        ptr += ch_time.getSize()

        if ch_id in self.__dict:
            # Retrieve the template instance for this channel
            ch_temp = self.__dict[ch_id]

            (size, val) = self.decode_ch_val(data, ptr, ch_temp)

            return ch_data.ChData(val, ch_time, ch_temp)
        else:
            print("Channel decode error: id %d not in dictionary"%ch_id)
            return None


    def decode_ch_val(self, val_data, offset, template):
        '''
        Decodes the given channel's value from the given data

        Args:
            val_data: Data to parse the value out of
            offset: Location in val_data to start the parsing
            template: Channel Template object for the channel

        Returns:
            A tuple of the form (len, val) where len is the size in bytes of
            the channel's value and val is the channel's value.
        '''
        type_obj = template.get_type_obj()
        type_obj.deserialize(val_data, offset)

        return (type_obj.getSize(), type_obj.val)


if __name__ == "__main__":
    pass

