"""
@brief Base class for all encoders. Defines the Encoder interface.

Encoders are responsible for taking data objects and serializing them into
binary data that can be sent to an fprime deployment.

Producers of encodable data (such as gui panels) will be passed an encoder
object for a specific descriptor type (cmd, file, etc). This will register the
encoder with the panel and the panel is responsible for calling the encoder's
data_callback function with data of the given descriptor type to send. In turn,
a sender (such as a tcp client) will be registered to the encoder using the
encoder's register_send function. The encoder will then call the sender's send
function with the serialized binary data.

This base class does not do any serialization. Giving data to it is a no-op. Its
purpose is to define the interface for an encoder.

@date Created July 9, 2018
@author R. Joseph Paetz

@bug No known bugs
"""
import abc
import logging

import fprime_gds.common.handlers
from fprime_gds.common.utils.config_manager import ConfigManager

LOGGER = logging.getLogger("encoder")


class Encoder(
    fprime_gds.common.handlers.DataHandler,
    fprime_gds.common.handlers.HandlerRegistrar,
    abc.ABC,
):
    """
    Base class for all encoder classes. This defines the "encode_api" function to allow for decoding of raw bytes. In
    addition it has a "data_callback" function implementation that decodes and sends out all results.
    """

    def __init__(self, config=None):
        """
        Encoder class constructor

        :param config: (ConfigManager, default=None): Object with configuration data for the sizes of fields in the
                       binary data. If None passed, defaults are used.
        """
        super().__init__()
        if config is None:
            # Retrieve defaults for the configs
            config = ConfigManager()
        self.config = config

    def data_callback(self, data, sender=None):
        """
        Data callback which calls the encode_api function exactly once. Then it passes the results to all registered
        consumer. This should only need to be overridden in extraordinary circumstances.

        :param data: data bytes to be decoded
        :param sender: (optional) sender id, otherwise None
        :return: returns the encoded data for reference
        """
        encoded = self.encode_api(data)
        if encoded is not None:
            self.send_to_all(encoded)
        else:
            LOGGER.warning("Encoder of type %s encoded 'None' type object", type(self))
        return encoded

    @abc.abstractmethod
    def encode_api(self, data):
        """
        Encodes the given data and returns the result.

        This function allows for non-registered code to utilize the same
        serialization functionality as is used to encode data passed to the
        data_callback function.

        :param data: data to be encoded as rae bytes
        :return: encoded data bytes
        """
