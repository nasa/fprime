"""
standard.py:

This file contains the necessary functions used to setup a standard pipeline that will pull data from the middle-ware
layer into the python system. This layer is designed to setup the base necessary to running Gds objects up-to the layer
of decoding. This enables users to generate add-in that function above the decoders, and run on a standard pipeline
below.

:author: lestarch
"""
import datetime
import os.path

import fprime.common.models.serialize.time_type

# History stores
import fprime_gds.common.history.ram
# Py Loaders
import fprime_gds.common.loaders.cmd_py_loader
import fprime_gds.common.loaders.event_py_loader
import fprime_gds.common.loaders.ch_py_loader
# XML Loaders
import fprime_gds.common.loaders.cmd_xml_loader
import fprime_gds.common.loaders.event_xml_loader
import fprime_gds.common.loaders.ch_xml_loader
# Encoders and Decoders
import fprime_gds.common.encoders.cmd_encoder
import fprime_gds.common.decoders.event_decoder
import fprime_gds.common.decoders.ch_decoder
import fprime_gds.common.decoders.pkt_decoder
# Misc system components
import fprime_gds.common.logger.data_logger
import fprime_gds.common.distributor.distributor
import fprime_gds.common.client_socket.client_socket
# Sendable commands
import fprime_gds.common.data_types.cmd_data

class StandardPipeline:
    """
    Class used to encapsulate all of the components of a standard pipeline. The life-cycle of this class follows the
    basic steps:
       1. setup: creates objects needed to read from middle-ware layer and provide data up the stack
       2. register: consumers from this pipeline should register to receive decoder callbacks
       3. run: this pipeline should either take over the standard thread of execution, or be executed on another thread
       4. terminate: called to shutdown the pipeline
    This class provides for basic log files as a fallback for storing events as well. These logs are stored in a given
    directory, which is created on the initialization of this class.
    """
    def __init__(self):
        """
        Set core variables to None
        """
        self.distributor = None
        self.client_socket = None
        self.logger = None
        # Dictionary items
        self.command_id_dict = None
        self.event_id_dict = None
        self.channel_id_dict = None
        self.command_name_dict = None
        self.event_name_dict = None
        self.channel_name_dict = None
        self.packet_dict = None
        # Encoder and decoder items
        self.command_encoder = None
        self.event_decoder = None
        self.channel_decoder = None
        self.packet_decoder = None
        # History stores for each above type
        self.command_hist = None
        self.event_hist = None
        self.channel_hist = None
        self.command_subscribers = []

    def setup(self, config, dictionary, logging_prefix=os.path.expanduser("~"), packet_spec=None):
        """
        Setup the standard pipeline for moving data from the middleware layer through the GDS layers using the standard
        patterns. This allows just registering the consumers, and invoking 'setup' all other of the GDS support layer.
        :param config: config object used when constructing the pipeline.
        :param dictionary: dictionary path. Used to setup loading of dictionaries.
        :param logging_prefix: logging prefix. Logs will be placed in a dated directory under this prefix
        :param packet_spec: location of packetized telemetry XML specification.
        """
        # Loads the distributor and client socket
        self.distributor = fprime_gds.common.distributor.distributor.Distributor(config)
        self.client_socket = fprime_gds.common.client_socket.client_socket.ThreadedTCPSocketClient()

        # Setup dictionaries encoders and decoders
        self.load_dictionaries(dictionary, packet_spec)
        self.setup_coders()
        self.setup_history()

        # Register distributor to client socket
        self.client_socket.register_distributor(self.distributor)

        # Register client socket to encoder
        self.command_encoder.register(self.client_socket)

        # Register the event and channel decoders to the distributor for their
        # respective data types
        self.distributor.register("FW_PACKET_LOG", self.event_decoder)
        self.distributor.register("FW_PACKET_TELEM", self.channel_decoder)
        if self.packet_dict is not None:
            self.distributor.register("FW_PACKET_PACKETIZED_TLM", self.packet_dict)
        # Final setup step is to make a logging directory, and register in the logger
        self.setup_logging(logging_prefix)

    def setup_coders(self):
        """
        Sets up the encoder layer of the GDS pipeline.
        :return:
        """
        # Create encoders and decoders using dictionaries
        self.command_encoder = fprime_gds.common.encoders.cmd_encoder.CmdEncoder()
        self.event_decoder = fprime_gds.common.decoders.event_decoder.EventDecoder(self.event_id_dict)
        self.channel_decoder = fprime_gds.common.decoders.ch_decoder.ChDecoder(self.channel_id_dict)
        if self.packet_dict is not None:
            self.packet_decoder = fprime_gds.common.loaders.pkt_decoder.PktDecoder(self.packet_dict, self.channel_id_dict)
        else:
            self.packet_decoder = None

    def setup_logging(self, prefix):
        """
        Setup logging based on the logging prefix supplied
        :param prefix: logging prefix to use
        """
        # Setup log file location
        dts = datetime.datetime.now()
        log_dir = os.path.join(prefix, dts.strftime("%Y-%m-%dT%H:%M:%S.%f"))
        # Make the directories if they do not exit
        if not os.path.exists(log_dir):
            os.makedirs(log_dir)
        # Setup the logging pipeline (register it to all its data sources)
        logger = fprime_gds.common.logger.data_logger.DataLogger(log_dir, verbose=True, csv=True)
        self.command_encoder.register(logger)
        self.event_decoder.register(logger)
        self.channel_decoder.register(logger)
        if self.packet_decoder is not None:
            self.packet_decoder.register(logger)
        self.client_socket.register_distributor(logger)
        self.logger = logger

    def setup_history(self):
        """
        Setup a set of history objects in order to store the events of the decoders.
        """
        # Create histories
        self.command_hist = fprime_gds.common.history.ram.RamHistory()
        self.event_hist = fprime_gds.common.history.ram.RamHistory()
        self.channel_hist = fprime_gds.common.history.ram.RamHistory()
        # Register histories
        self.event_decoder.register(self.event_hist)
        self.channel_decoder.register(self.channel_hist)
        # Register
        if self.packet_decoder is not None:
            self.packet_decoder.register(self.channel_hist)

    def load_dictionaries(self, dictionary, packet_spec):
        """
        Loads the dictionaries based on the dictionary path supplied
        :param dictionary: dictionary path used for loading dictionaries
        :param packet_spec: specification for packets, or None, for packetized telemetry
        """
        # Loading the dictionaries from a directory. A directory indicates heritage python dicts.
        if os.path.isdir(dictionary):
            # Events
            event_loader = fprime_gds.common.loaders.event_py_loader.EventPyLoader()
            self.event_id_dict = event_loader.get_id_dict(os.path.join(dictionary, "events"))
            self.event_name_dict = event_loader.get_name_dict(os.path.join(dictionary, "events"))
            # Commands
            command_loader = fprime_gds.common.loaders.cmd_py_loader.CmdPyLoader()
            self.command_id_dict = command_loader.get_id_dict(os.path.join(dictionary, "commands"))
            self.command_name_dict = command_loader.get_name_dict(os.path.join(dictionary, "commands"))
            # Channels
            channel_loader = fprime_gds.common.loaders.ch_py_loader.ChPyLoader()
            self.channel_id_dict = channel_loader.get_id_dict(os.path.join(dictionary, "channels"))
            self.channel_name_dict = channel_loader.get_name_dict(os.path.join(dictionary, "channels"))
        # XML dictionaries
        elif os.path.isfile(dictionary):
            # Events
            event_loader = fprime_gds.common.loaders.event_xml_loader.EventXmlLoader()
            self.event_id_dict = event_loader.get_id_dict(dictionary)
            self.event_name_dict = event_loader.get_name_dict(dictionary)
            # Commands
            command_loader = fprime_gds.common.loaders.cmd_xml_loader.CmdXmlLoader()
            self.command_id_dict = command_loader.get_id_dict(dictionary)
            self.command_name_dict = command_loader.get_name_dict(dictionary)
            # Channels
            channel_loader = fprime_gds.common.loaders.ch_xml_loader.ChXmlLoader()
            self.channel_id_dict = channel_loader.get_id_dict(dictionary)
            self.channel_name_dict = channel_loader.get_name_dict(dictionary)
        else:
            raise Exception("[ERROR] Dictionary '{}' does not exist.".format(dictionary))
        # Check for packet specification
        if packet_spec is not None:
            packet_loader = fprime_gds.common.loaders.pkt_xml_loader.PktXmlLoader()
            self.packet_dict = packet_loader.get_id_dict(packet_spec, self.channel_name_dict)
        else:
            self.packet_dict = None

    def connect(self, address, port):
        """
        Connects to the middleware layer
        :param address: address of middleware
        :param port: port of middleware
        """
        self.client_socket.connect(address, port)
        self.client_socket.register_to_server(fprime_gds.common.client_socket.client_socket.GUI_TAG)

    def send_command(self, command, args):
        """
        Sends commands to the encoder and history.
        :param command: command id from dictionary to get command template
        :param args: arguments to process
        """
        command_template = self.command_id_dict[command]
        cmd_data = fprime_gds.common.data_types.cmd_data.CmdData(tuple(args), command_template)
        cmd_data.time = fprime.common.models.serialize.time_type.TimeType()
        cmd_data.time.set_datetime(datetime.datetime.now(), 2)
        self.command_hist.data_callback(cmd_data)
        for hist in self.command_subscribers:
            hist.data_callback(cmd_data)
        self.command_encoder.data_callback(cmd_data)
        self.logger.data_callback(cmd_data)

    def disconnect(self):
        """
        Disconnect from socket
        """
        self.client_socket.disconnect()

    def get_event_id_dictionary(self):
        """
        Getter for event dictionary.
        :return: event dictionary keyed by event IDs
        """
        return self.event_id_dict

    def get_event_name_dictionary(self):
        """
        Getter for event dictionary.
        :return: event dictionary keyed by event mnemonics
        """
        return self.event_name_dict

    def get_channel_id_dictionary(self):
        """
        Getter for channel dictionary.
        :return: channel dictionary keyed by channel IDs
        """
        return self.channel_id_dict

    def get_channel_name_dictionary(self):
        """
        Getter for channel dictionary.
        :return: channel dictionary keyed by channel mnemonics
        """
        return self.channel_name_dict

    def get_command_id_dictionary(self):
        """
        Getter for command dictionary.
        :return: command dictionary keyed by command IDs
        """
        return self.command_id_dict

    def get_command_name_dictionary(self):
        """
        Getter for command dictionary.
        :return: command dictionary keyed by command mnemonics
        """
        return self.command_name_dict

    def get_event_history(self):
        """
        Getter for event history.
        :return: event history
        """
        return self.event_hist

    def get_channel_history(self):
        """
        Getter for channel history.
        :return: channel history
        """
        return self.channel_hist

    def get_command_history(self):
        """
        Getter for command history.
        :return: command history
        """
        return self.command_hist

    ###########################################################################
    #   Subscriber functions
    ###########################################################################
    def register_event_consumer(self, history):
        """
        Registers a history with the event decoder.
        """
        self.event_decoder.register(history)

    def remove_event_consumer(self, history):
        """
        Removes a history from the event decoder. Will raise an error if the history was not
        previously registered.
        Returns:
            a boolean indicating if the history was removed.
        """
        return self.event_decoder.deregister(history)

    def register_telemetry_consumer(self, history):
        """
        Registers a history with the telemetry decoder.
        """
        self.channel_decoder.register(history)

    def remove_telemetry_consumer(self, history):
        """
        Removes a history from the telemetry decoder. Will raise an error if the history was not
        previously registered.
        Returns:
            a boolean indicating if the history was removed.
        """
        return self.channel_decoder.deregister(history)

    def register_command_consumer(self, history):
        """
        Registers a history with the standard pipeline.
        """
        self.command_subscribers.append(history)

    def remove_command_consumer(self, history):
        """
        Removes a history that is subscribed to command data. Will raise an error if the history
        was not previously registered.
        Returns:
            a boolean indicating if the history was removed.
        """
        try:
            self.command_subscribers.remove(history)
            return True
        except ValueError:
            return False
