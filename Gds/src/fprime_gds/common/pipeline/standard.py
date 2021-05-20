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
import fprime_gds.common.client_socket.client_socket
import fprime_gds.common.data_types.cmd_data
import fprime_gds.common.distributor.distributor
import fprime_gds.common.logger.data_logger

# Local imports for the sake of composition
from . import dictionaries, encoding, files, histories


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
        Set core variables to None or their composition handlers.
        """
        self.distributor = None
        self.client_socket = None
        self.logger = None

        self.__dictionaries = dictionaries.Dictionaries()
        self.__coders = encoding.EncodingDecoding()
        self.__histories = histories.Histories()
        self.__filing = files.Filing()

    def setup(
        self, config, dictionary, down_store, logging_prefix=None, packet_spec=None
    ):
        """
        Setup the standard pipeline for moving data from the middleware layer through the GDS layers using the standard
        patterns. This allows just registering the consumers, and invoking 'setup' all other of the GDS support layer.

        :param config: config object used when constructing the pipeline.
        :param dictionary: dictionary path. Used to setup loading of dictionaries.
        :param down_store: downlink storage directory
        :param logging_prefix: logging prefix. Defaults to not logging at all.
        :param packet_spec: location of packetized telemetry XML specification.
        """
        # Loads the distributor and client socket
        self.distributor = fprime_gds.common.distributor.distributor.Distributor(config)
        self.client_socket = (
            fprime_gds.common.client_socket.client_socket.ThreadedTCPSocketClient()
        )
        # Setup dictionaries encoders and decoders
        self.dictionaries.load_dictionaries(dictionary, packet_spec)
        self.coders.setup_coders(
            self.dictionaries, self.distributor, self.client_socket, config
        )
        self.histories.setup_histories(self.coders)
        self.files.setup_file_handling(
            down_store,
            self.coders.file_encoder,
            self.coders.file_decoder,
            self.distributor,
            logging_prefix,
        )
        # Register distributor to client socket
        self.client_socket.register_distributor(self.distributor)
        # Final setup step is to make a logging directory, and register in the logger
        if logging_prefix:
            self.setup_logging(logging_prefix)

    @classmethod
    def get_dated_logging_dir(cls, prefix=os.path.expanduser("~")):
        """
        Sets up the dated subdirectory based upon a given prefix

        :param prefix:
        :return: Path to new directory where logs will be stored for this pipeline
        """
        # Setup log file location
        dts = datetime.datetime.now()
        log_dir = os.path.join(prefix, dts.strftime("%Y-%m-%dT%H:%M:%S.%f"))
        # Make the directories if they do not exit
        if not os.path.exists(log_dir):
            os.makedirs(log_dir)
        return log_dir

    def setup_logging(self, log_dir):
        """
        Setup logging based on the logging prefix supplied

        :param log_dir: logging output directory
        """
        # Setup the logging pipeline (register it to all its data sources)
        logger = fprime_gds.common.logger.data_logger.DataLogger(
            log_dir, verbose=True, csv=True
        )
        self.logger = logger
        self.coders.register_channel_consumer(self.logger)
        self.coders.register_event_consumer(self.logger)
        self.coders.register_command_consumer(self.logger)
        self.coders.register_packet_consumer(self.logger)
        self.client_socket.register_distributor(self.logger)

    def connect(self, address, port):
        """
        Connects to the middleware layer

        :param address: address of middleware
        :param port: port of middleware
        """
        self.client_socket.connect(address, port)
        self.client_socket.register_to_server(
            fprime_gds.common.client_socket.client_socket.GUI_TAG
        )

    def disconnect(self):
        """
        Disconnect from socket
        """
        self.client_socket.disconnect()
        self.files.uplinker.exit()

    def send_command(self, command, args):
        """
        Sends commands to the encoder and history.

        :param command: command id from dictionary to get command template
        :param args: arguments to process
        """
        if isinstance(command, str):
            command_template = self.dictionaries.command_name[command]
        else:
            command_template = self.dictionaries.command_id[command]
        cmd_data = fprime_gds.common.data_types.cmd_data.CmdData(
            tuple(args), command_template
        )
        cmd_data.time = fprime.common.models.serialize.time_type.TimeType()
        cmd_data.time.set_datetime(datetime.datetime.now(), 2)
        self.coders.send_command(cmd_data)

    @property
    def dictionaries(self):
        """
        Get a dictionaries object

        :return: dictionaries composition
        """
        return self.__dictionaries

    @property
    def coders(self):
        """
        Get a coders object

        :return: coders composition
        """
        return self.__coders

    @property
    def histories(self):
        """
        Get a histories object

        :return: histories composition
        """
        return self.__histories

    @property
    def files(self):
        """
        Files member property

        :return: filing compositions
        """
        return self.__filing
