'''
@brief Decoder for event data

This decoder takes in serialized events, parses them, and packages the results
in event_data objects.

Example data structure:
    +-------------------+---------------------+---------------------- - - -
    | Lenghth (4 bytes) | Time Tag (11 bytes) | Event argument data....
    +-------------------+---------------------+---------------------- - - -

@date Created June 29, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

class EventDecoder(decoder.Decoder):
    '''Decoder class for event data'''

    def __init__(self, event_dict):
        '''
        EventDecoder class constructor

        Args:
            event_dict: Event dictionary. Event IDs should be keys and
                        event_template objects should be values

        Returns:
            An initialized EventDecoder object.
        '''
        self.__dict = event_dict

        # List of consumers to be notified of new data
        self.__consumers = []


    def data_callback(self, data):
        '''
        Function called to pass data to the decoder class

        Args:
            data: Binary data to decode and pass to registered consumers
        '''
        self.send_to_all(self.decode_api(data))


    def register(self, consumer_obj):
        '''
        Function called to register a consumer to this decoder

        For each data item passed to and parsed by the decoder, the parsed data
        will be passed as an argument to the data_callback function of each
        registered consumer.

        Args:
            consumer_obj: Object to regiser to the decoder. Must implement a
                          data_callback function.
        '''
        self.__consumers.append(consumer_obj)


    def decode_api(self, data):
        '''
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        Args:
            data: Binary data to decode

        Returns:
            Parsed version of the argument data
        '''
        ptr = 0

        # Decode log event ID here...
        id_obj = u32_type.U32Type()
        id_obj.deserialize(data, ptr)
        ptr += id_obj.getSize()
        event_id = id_obj.val

        # Decode time...
        event_time = time_type.TimeType()
        event_time.deserialize(data, ptr)
        ptr += event_time.getSize()

        if event_id in self.__dict:
            event_template = self.__dict[event_id]

            #TODO finish


    def send_to_all(self, parsed_data):
        '''
        Sends the parsed_data object to all registered consumers

        This function is not intended to be called from outside a decoder class

        Args:
            parsed_data: object to send to all registered consumers
        '''
        for obj in self.__consumers:
            obj.data_callback(parsed_data)


if __name__ == "__main__":
    # Unit tests
    # (don't check functionality, just test code path's for exceptions)

    try:
        decoder1 = Decoder()
        decoder2 = Decoder()
        decoder3 = Decoder()

        decoder1.register(decoder2)
        decoder1.register(decoder3)

        decoder1.data_callback("hello")

        if (decoder1.decode_api("hello") != "hello"):
            print("Decoder Unit tests failed")
        else:
            print("Decoder Unit tests passed")
    except:
        print("Decoder Unit tests failed")
