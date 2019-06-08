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
import enum

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
import fprime_gds.common.logger
import fprime_gds.common.distributor.distributor
import fprime_gds.common.client_socket.client_socket


class StandardPipeline:
    """
    Class used to encapsulate all of the components of a standard pipeline. The life-cycle of this class follows the
    basic steps:
       1. setup: creates objects needed to read from middle-ware layer and provide data up the stack
       2. register: consumers from this pipeline should register to recieve decoder callbacks
       3. run: this pipeline should either take over the standard thread of execution, or be executed on another thread
       4. terminate: called to shutdown the piepline
    This class provides for basic log files as a fallback for storing events as well. These logs are stored in a given
    directory, which is created on the initialization of this class.
    """
    def __init__(self):
        """
        Set core variables to None
        """
        self.distributor = None
        self.client_socket = None
        # Dictionary items
        self.command_dict = None
        self.event_dict = None
        self.channel_dict = None
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

    def setup(self, config, dictionary, logging_prefix=os.path.expanduser("~"), packet_spec=None):
        """
        Setup the standard pipeline for moving data from the middleware layer through the GDS layers using the standard
        patterns. This allows just registering the consumers, and invoking 'setup' all other of the GDS support layer.
        :param config: config object used when constructing the pipeline.
        :param dictionary: dictionary path. Used to setup loading of dictsionaries.
        :param logging_prefix: logging prefix. Logs will be placed in a dated directory under this prefix
        :param packet_spec: location of packetized telemetry XML specification.
        """
        # Loads the distributor and client socket
        self.distributor = fprime_gds.common.distributor.distributor.Distributor(config)
        self.client_socket = fprime_gds.common.client_socket.client_socket.ThreadedTCPSocketClient()

        # Setup dictionaries encoders and decoders
        self.load_dictionaries(dictionary, packet_spec)
        self.setup_coders()

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
        self.event_decoder = fprime_gds.common.encoders.event_decoder.EventDecoder(self.event_dict)
        self.channel_decoder = fprime_gds.common.encoders.ch_decoder.ChDecoder(self.channel_dict)
        if self.packet_dict is not None:
            self.packet_decoder = fprime_gds.common.loaders.pkt_decoder.PktDecoder(self.packet_dict, self.channel_dict)
        else:
            self.packet_decoder = None

    def setup_logging(self, prefix):
        """
        Setup logging based on the logging prefix supplied
        :param prefix: logging prefix to use
        """
        # Setup log file location
        dts = datetime.datetime.now()
        log_dir = os.path.join(prefix, "{:4d}-{:2d}-{:2d}T{:2d}:{:2}:{:2}.{:3d}"
                               .format(dts.year, dts.month, dts.day, dts.hour, dts.minute, dts.second,
                                       int(dts.microsecond/1000)))
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

    def setup_history(self):
        """
        Setup a set of history objects in order to store the events of the decoders.
        """
        # Create histories
        self.command_hist = fprime_gds.common.history.ram.RamHistory()
        self.event_hist = fprime_gds.common.history.ram.RamHistory()
        self.channel_hist = fprime_gds.common.history.ram.RamHistory()
        # Register histories
        self.event_decider.register(self.event_hist)
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
            self.event_dict = event_loader.get_id_dict(os.path.join(dictionary, "events"))
            # Commands
            command_loader = fprime_gds.common.loaders.cmd_py_loader.CmdPyLoader()
            self.command_dict = command_loader.get_name_dict(os.path.join(dictionary, "commands"))
            # Channels
            channel_loader = fprime_gds.common.loaders.ch_py_loader.ChPyLoader()
            self.channel_dict = channel_loader.get_id_dict(os.path.join(dictionary, "channels"))
            channel_name_dict = channel_loader.get_name_dict(os.path.join(dictionary, "channels"))
        # XML dictionaries
        elif os.path.isfile(dictionary):
            # Events
            event_loader = fprime_gds.common.loaders.event_xml_loader.EventXmlLoader()
            self.event_dict = event_loader.get_id_dict(dictionary)
            # Commands
            command_loader = fprime_gds.common.loaders.cmd_xml_loader.CmdXmlLoader()
            self.command_dict = command_loader.get_name_dict(dictionary)
            # Channels
            channel_loader = fprime_gds.common.loaders.ch_xml_loader.ChXmlLoader()
            self.channel_dict = channel_loader.get_id_dict(dictionary)
            channel_name_dict = channel_loader.get_name_dict(dictionary)
        else:
            raise Exception("[ERROR] Dictionary '{}' does not exist.".format(dictionary))
        # Check for packet specification
        if packet_spec is not None:
            packet_loader = fprime_gds.common.loaders.pkt_xml_loader.PktXmlLoader()
            self.packet_dict = packet_loader.get_id_dict(packet_spec, channel_name_dict)
        else:
            self.packet_dict = None

    def get_event_dictionary(self):
        """
        Getter for event dictionary.
        :return: event dictionary
        """
        return self.event_dict

    def get_channel_dictionary(self):
        """
        Getter for channel dictionary.
        :return: channel dictionary
        """
        return self.channel_dict

    def get_command_dictionary(self):
        """
        Getter for command dictionary.
        :return: command dictionary
        """
        return self.cmd_dict

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
