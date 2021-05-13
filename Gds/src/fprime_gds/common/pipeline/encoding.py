"""
encoding.py:

This file sets up the encoding and decoding handlers for use with the standard pipeline. This encapsulates the encoding
and decoding into a single component that the be composed into the standard pipeline.

@mstarch
"""
import fprime_gds.common.decoders.ch_decoder
import fprime_gds.common.decoders.event_decoder
import fprime_gds.common.decoders.file_decoder
import fprime_gds.common.decoders.pkt_decoder
import fprime_gds.common.encoders.cmd_encoder

# Encoders and Decoders
import fprime_gds.common.encoders.file_encoder


class EncodingDecoding:
    """
    Sets up and runs the encoding and decoding for the standard pipeline. This include the following encoders and
    decoders for standard setups:

    1. Events decoding
    2. Channel decoding
    3. File decoding
    4. Command encoding
    5. File encoding
    """

    def __init__(self):
        """
        Setup the encoders and decoder member variables.
        """
        # Encoder and decoder items
        self.file_encoder = None
        self.command_encoder = None
        self.event_decoder = None
        self.channel_decoder = None
        self.file_decoder = None
        self.packet_decoder = None
        self.command_subscribers = []

    def setup_coders(self, dictionaries, distributor, sender, config):
        """
        Sets up the encoder and decoder layer of the GDS pipeline. This requires a dictionary set that has loaded the
        dictionaries needed for the decoders to work correctly. This will register then register the decoders with a
        supplied distributor to handle known types with the known decoders. Lastly, the sender will be registered to the
        encoder to handle the encoded data going out.

        :param dictionaries: a dictionaries handling object holding dictionaries
        :param distributor: distributor of data to register to
        :param sender: is used to send the bytes created by the command encoder
        :param config: config object used to describe types used for different field encodings
        """
        # Create encoders and decoders using dictionaries
        self.file_encoder = fprime_gds.common.encoders.file_encoder.FileEncoder()
        self.command_encoder = fprime_gds.common.encoders.cmd_encoder.CmdEncoder(
            config=config
        )
        self.event_decoder = fprime_gds.common.decoders.event_decoder.EventDecoder(
            dictionaries.event_id, config=config
        )
        self.channel_decoder = fprime_gds.common.decoders.ch_decoder.ChDecoder(
            dictionaries.channel_id, config=config
        )
        self.file_decoder = fprime_gds.common.decoders.file_decoder.FileDecoder()
        self.packet_decoder = None
        if dictionaries.packet is not None:
            self.packet_decoder = fprime_gds.common.decoders.pkt_decoder.PktDecoder(
                dictionaries.packet, dictionaries.channel_id
            )
        # Register client socket to encoder
        self.command_encoder.register(sender)
        self.file_encoder.register(sender)
        # Register the event and channel decoders to the distributor for their
        # respective data types
        distributor.register("FW_PACKET_LOG", self.event_decoder)
        distributor.register("FW_PACKET_TELEM", self.channel_decoder)
        distributor.register("FW_PACKET_FILE", self.file_decoder)
        if self.packet_decoder is not None:
            distributor.register("FW_PACKET_PACKETIZED_TLM", self.packet_decoder)

    def send_command(self, command):
        """
        Sends a command to the registered command encoder, and further down the stream. Note: this contains a local
        loopback to any command consumers to ensure that histories and logging are updated.

        :param command: command object to send
        """
        for loopback in self.command_subscribers:
            loopback.data_callback(command)
        self.command_encoder.data_callback(command)

    def register_event_consumer(self, consumer):
        """
        Registers a history with the event decoder.

        :param consumer: consumer of events
        """
        self.event_decoder.register(consumer)

    def remove_event_consumer(self, consumer):
        """
        Removes a history from the event decoder. Will raise an error if the history was not
        previously registered.

        :param consumer: consumer of events
        :return: a boolean indicating if the consumer was removed.
        """
        return self.event_decoder.deregister(consumer)

    def register_channel_consumer(self, consumer):
        """
        Registers a history with the telemetry decoder.

        :param consumer: consumer of channels
        """
        self.channel_decoder.register(consumer)

    def remove_channel_consumer(self, consumer):
        """
        Removes a history from the telemetry decoder. Will raise an error if the history was not
        previously registered.

        :param consumer: consumer of channels
        :return: a boolean indicating if the consumer was removed.
        """
        return self.channel_decoder.deregister(consumer)

    def register_command_consumer(self, consumer):
        """
        Registers a history with the standard pipeline.

        :param consumer: consumer of commands
        """
        self.command_subscribers.append(consumer)

    def remove_command_consumer(self, consumer):
        """
        Removes a history that is subscribed to command data. Will raise an error if the history
        was not previously registered.

        :param consumer: consumer of commands
        :return: a boolean indicating if the consumer was removed.
        """
        try:
            self.command_subscribers.remove(consumer)
            return True
        except ValueError:
            return False

    def register_packet_consumer(self, consumer):
        """
        Registers a history with the standard pipeline.

        :param consumer: consumer of packets
        """
        if self.packet_decoder is not None:
            self.packet_decoder.register(consumer)

    def deregister_packet_consumer(self, consumer):
        """
        Removes a history that is subscribed to command data. Will raise an error if the history
        was not previously registered.

        :param consumer: consumer of packets
        :return: a boolean indicating if the consumer was removed.
        """
        if self.packet_decoder is not None:
            return self.packet_decoder.deregister(consumer)
