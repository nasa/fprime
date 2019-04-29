'''
@brief Base class for all encoders. Defines the Encoder interface.

Encoders are responsible for taking data objects and serializing them into
binary data that can be sent to an fprime deployment.

Producers of encodable data (such as gui panels) will be passed an encoder
object for a specific descriptor type (cmd, file, etc). This will register the
encoder with the panel and the panel is responsible for calling the encoder's
data_callback function with data of the given descriptor type to send. In turn,
a sender (such as a tcp client) will be regsitered to the encoder using the
encoder's register_send function. The encoder will then call the sender's send
function with the serialized binary data.

This base class does not do any serialization. Giving data to it is a no-op. Its
purpose is to define the interface for an encoder.

@date Created July 9, 2018
@author R. Joseph Paetz

@bug No known bugs
'''

from fprime_gds.common.utils.config_manager import ConfigManager

class Encoder(object):
    '''Base class for all encoder classes. Defines the interface for encoders'''

    def __init__(self, dest="FSW", config=None):
        '''
        Encoder class constructor

        Args:
            dest (string, "FSW" or "GUI", default="FSW"): Destination for binary
                  data produced by encoder.
            config (ConfigManager, default=None): Object with configuration data
                    for the sizes of fields in the binary data. If None passed,
                    defaults are used.

        Returns:
            An initialized encoder object.
        '''
        # List of senders to be notified of new data
        self.__senders = []

        if config==None:
            # Retrieve defaults for the configs
            config = ConfigManager()

        self.config = config

        self.dest = dest


    def data_callback(self, data):
        '''
        Function called to pass data to the encoder

        Args:
            data: Data object (of type sys_data) to serialize and send to all
                  senders.
        '''
        pass


    def register(self, sender_obj):
        '''
        Function called to register a sender to this encoder

        For each data item passed to and serialized by the encoder, the
        resulting binary data will be passed as an argument to the send function
        of each registered consumer.

        Args:
            sender_obj: Object to regiser to the encoder. Must implement a
                        send function.
        '''
        self.__senders.append(sender_obj)


    def encode_api(self, data):
        '''
        Encodes the given data and returns the result.

        This function allows for non-registered code to utilize the same
        serialization functionality as is used to encode data passed to the
        data_callback function.

        Args:
            data: Data object (of a sys_data type) to encode

        Returns:
            Binary version of the data argument
        '''
        pass


    def send_to_all(self, binary_data):
        '''
        Sends the binary_data object to all registered senders

        This function is not intended to be called from outside a decoder class

        Args:
            binary_data (bytearray): object to send to all registered senders
        '''
        for obj in self.__senders:
            obj.send(binary_data, self.dest)


if __name__ == "__main__":
    pass
