"""
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
"""

import abc
import logging

import fprime_gds.common.handlers

LOGGER = logging.getLogger("decoder")


class Decoder(
    fprime_gds.common.handlers.DataHandler,
    fprime_gds.common.handlers.HandlerRegistrar,
    abc.ABC,
):
    """
    Base class for all decoder classes. This defines the "decode_api" function to allow for decoding of raw bytes. In
    addition it has a "data_callback" function implementation that decodes and sends out all results.
    """

    def data_callback(self, data, sender=None):
        """
        Data callback which calls the decode_api function exactly once. Then it passes the results to all registered
        consumer. This should only need to be overridden in extraordinary circumstances.

        :param data: data bytes to be decoded
        :param sender: (optional) sender id, otherwise None
        """
        decoded = self.decode_api(data)
        if decoded is not None:
            self.send_to_all(decoded)
            return
        LOGGER.warning("Decoder of type %s produced 'None' decoded object", type(self))

    @abc.abstractmethod
    def decode_api(self, data):
        """
        Decodes the given data and returns the result.

        This function allows for non-registered code to call the same decoding
        code as is used to parse data passed to the data_callback function.

        :param data: binary data to decode
        :return: decoded data object
        """
