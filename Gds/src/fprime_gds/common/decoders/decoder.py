'''
@brief Base class for all decoders. Defines the Decoder interface.

Decoders are responsible for taking in serialized data and parsing it into
objects. Decoders receive serialized data (that had a specific descriptor) from
a distributer that it has been registered to. The distributer will send the
binary data after removing any length and descriptor headers.

Example data that would be sent to a decoder that parses events or channels:
    +-------------------+---------------------+------------ - - -
    | ID (4 bytes) | Time Tag (11 bytes) | Data....
    +-------------------+---------------------+------------ - - -

This base class does not do any parsing, but instead acts as a pass through to
allow consumers to receive raw data.

@date Created June 29, 2018
@author R. Joseph Paetz

@bug No known bugs
'''
from __future__ import print_function


class Decoder(object):
    '''Base class for all decoder classes. Defines the interface for decoders'''

    def __init__(self):
        '''
        Decoder class constructor

        Returns:
            An initialized decoder object.
        '''
        # List of consumers to be notified of new data
        self.__consumers = []


    def data_callback(self, data):
        '''
        Function called to pass data to the decoder class

        Args:
            data: Binary data to decode and pass to registered consumers
        '''
        self.send_to_all(data)


    def register(self, consumer_obj):
        '''
        Function called to register a consumer to this decoder

        For each data item passed to and parsed by the decoder, the parsed data
        will be passed as an argument to the data_callback function of each
        registered consumer.

        Args:
            consumer_obj: Object to register to the decoder. Must implement a
                          data_callback function.
        '''
        self.__consumers.append(consumer_obj)
    
    def deregister(self, consumer_obj):
        '''
        Function called to remove a registered consumer from this decoder

        Once called, a registered item will no longer receive updates from this decoder.

        Args:
            consumer_obj: Object to deregister from the decoder. Must implement a
                          data_callback function.
        Return:
            a boolean indicating if the consumer was removed
        '''
        try:
            self.__consumers.remove(consumer_obj)
            return True
        except ValueError:
            return False


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
        # Base class just acts as a data passthrough:
        return data


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
