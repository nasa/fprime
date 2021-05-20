"""
cli.py:

This file sets up the command line interface and argument parsing that is done to support the F prime executable tools
layer. It is designed to allow users to import standard sets of arguments that applied to the various aspects of the
code that they are importing.

@author mstarch
"""
import abc
import argparse
import copy
import datetime
import errno
import importlib
import os
import re
import sys

import fprime_gds.common.communication.adapters.base

# Include basic adapters
import fprime_gds.common.communication.adapters.ip
import fprime_gds.common.utils.config_manager

try:
    import fprime_gds.common.communication.adapters.uart
except ImportError:
    pass
# Try to import each GUI type, and if it can be imported
# it will be provided to the user as an option
GUIS = ["none", "html"]
try:
    import fprime_gds.wxgui.tools.gds

    GUIS.append("wx")
except ImportError:
    pass


class ParserBase(abc.ABC):
    """
    Parsers must define several functions. They must define "get_parser", which will produce a parser to parse the
    arguments, and an optional "handle_arguments" function to do any necessary processing of the arguments. Note: when
    handling arguments, the implementer should copy the incoming namespace if the original values of arguments will be
    modified.
    """

    @staticmethod
    @abc.abstractmethod
    def get_parser():
        """
        Produce a parser that will handle the given arguments. These parsers can be combined for a CLI for a tool by
        assembling them as parent processors to a parser for the given tool.
        """

    @classmethod
    @abc.abstractmethod
    def handle_arguments(cls, args, **kwargs):
        """
        Handle arguments from the given parser. The expectation is that the "args" namespace is taken in, processed, and
        a new namespace object is returned with the processed variants of the arguments. Copy the namespace object when
        modifying existing arguments.

        :param args: arguments namespace of processed arguments
        :return: namespace with processed results of arguments.
        """

    @staticmethod
    def parse_args(
        parser_classes, description="No tool description provided", **kwargs
    ):
        """
        Create a parser for the given application using the description provided. This will then add all specified
        ParserBase subclasses' get_parser output as parent parses for the created parser. Then all of the handle
        arguments methods will be called, and the final namespace will be returned.

        :param parser_classes: a list of ParserBase subclasses that will be used to
        :param description: description passed ot the argument parser
        :return: namespace with all parsed arguments from all provided ParserBase subclasses
        """
        if kwargs is None:
            kwargs = {}
        created_parsers = [parser_base.get_parser() for parser_base in parser_classes]
        parser = argparse.ArgumentParser(
            description=description, parents=created_parsers
        )
        args = parser.parse_args()
        # Handle all arguments
        try:
            for parser_base in parser_classes:
                args = parser_base.handle_arguments(args, **kwargs)
        except ValueError as ver:
            print("[ERROR] Failed to parse arguments: {}".format(ver), file=sys.stderr)
            sys.exit(-1)
        return args, parser

    @staticmethod
    def find_in(token, deploy, is_file=True):
        """
        Find token in deploy directory by walking the directory looking for reg-ex. This effectively finds a file in a
        subtree and provides the path to it. Returns None when not found

        :param token: token to search for in the directory structure
        :param deploy: directory to start with
        :param is_file: true if looking for file, otherwise false
        :return: full path to token in tree
        """
        for dirpath, dirs, files in os.walk(deploy):
            for check in files if is_file else dirs:
                if re.match("^" + str(token) + "$", check):
                    return os.path.join(dirpath, check)
        return None


class ImportParser(ParserBase):
    """
    Parser used to import new modules into the F prime namespace. This is how F prime can handle new modules and plugins
    by allowing the user to import new Python modules in, which may define new adapters and other items.
    """

    @staticmethod
    def get_parser():
        """
        Creates a parser that reads '--import' arguments to import new modules.  Multiple '--import' are supported.

        :return: parser that can handle imports
        """
        parser = argparse.ArgumentParser(
            description="Process import arguments allowing for addition of modules",
            add_help=False,
        )
        parser.add_argument(
            "--include",
            action="append",
            default=[],
            help="Module name in PYTHONPATH to import. Users may specify multiple --include flags.",
        )
        return parser

    @classmethod
    def handle_arguments(cls, args, **kwargs):
        """
        Handle the import arguments by looping through them and importing each. Errors are printed to the console if an
        import failed, but no other effect is realized.

        :param args: parsed arguments namespace
        :return: args as input, has side effect of new imported modules
        """
        for module in args.include:
            try:
                globals()[module] = importlib.import_module(module)
            except ImportError as ime:
                print(
                    "[WARNING] Failed to import '{}' with error {}".format(module, ime),
                    file=sys.stderr,
                )
        return args

    @staticmethod
    def run_first():
        """
        Runs this parser once first so that other parsers can benefit from the existing imported libraries. If there is
        a problem, the help message will be minimal, but should work.
        """
        args, _ = ImportParser.get_parser().parse_known_args()
        ImportParser.handle_arguments(args)


class CommParser(ParserBase):
    """
    Handles parsing of all of the comm-layer arguments. This means selecting a comm adapter, and passing the arguments
    required to setup that comm adapter. In addition, this parser uses the import parser to import modules such that a
    user may import other adapter implementation files.
    """

    @staticmethod
    def get_parser():
        """
        Produce a parser that will handle the given arguments. These parsers can be combined for a CLI for a tool by
        assembling them as parent processors to a parser for the given tool.
        """
        ImportParser.run_first()
        adapters = (
            fprime_gds.common.communication.adapters.base.BaseAdapter.get_adapters().keys()
        )
        adapter_parents = []
        for adapter_name in adapters:
            adapter = fprime_gds.common.communication.adapters.base.BaseAdapter.get_adapters()[
                adapter_name
            ]
            # Check adapter real quick before moving on
            if not hasattr(adapter, "get_arguments") or not callable(
                getattr(adapter, "get_arguments", None)
            ):
                print(
                    "[WARNING] '{}' does not have 'get_arguments' method, skipping.".format(
                        adapter_name
                    ),
                    file=sys.stderr,
                )
                continue
            comm_parser = argparse.ArgumentParser(
                description="'{}' parser".format(adapter_name), add_help=False
            )
            # Add arguments for the parser
            for argument in adapter.get_arguments().keys():
                comm_parser.add_argument(*argument, **adapter.get_arguments()[argument])
            adapter_parents.append(comm_parser)
        parser = argparse.ArgumentParser(
            description="Process arguments needed to specify a comm-adapter",
            add_help=False,
            parents=adapter_parents,
        )
        parser.add_argument(
            "--comm-adapter",
            dest="adapter",
            action="store",
            type=str,
            help="Adapter for communicating to flight deployment. [default: %(default)s]",
            choices=adapters,
            default="ip",
        )
        return parser

    @classmethod
    def handle_arguments(cls, args, **kwargs):
        """
        Handle the input arguments for the the parser. This will help setup the adapter with its expected arguments.

        :param args: parsed arguments in namespace format
        :return: namespace with "comm_adapter" value added
        """
        args = copy.copy(args)
        args.comm_adapter = (
            fprime_gds.common.communication.adapters.base.BaseAdapter.construct_adapter(
                args.adapter, args
            )
        )
        return args


class LogDeployParser(ParserBase):
    """
    A parser that handles log files by reading in a '--logs' directory or a '--deploy' directory to put the logs into
    as a default. This is useful as a parsing fragment for any application that produces log files and needs these logs
    to end up in the proper place.
    """

    # Class variable to store logs on first encounter of the logging call. This helps ensure that the logs all end up in
    # a consistent directory, opposed to multiple directories across the system.
    first_log = None

    @staticmethod
    def get_parser():
        """
        Creates a new parser that can process "--deployment" and "--logs" arguments for use with any CLI producing log
        files. Can be used to construct parent arguments

        :return: parser with logging and deploy arguments
        """
        parser = argparse.ArgumentParser(
            description="Process arguments needed to specify a logging", add_help=False
        )
        parser.add_argument(
            "-l",
            "--logs",
            dest="logs",
            action="store",
            default=os.path.join(os.getcwd(), "logs"),
            type=str,
            help="Logging directory. Created if non-existent. [default: %(default)s]",
        )
        parser.add_argument(
            "--log-directly",
            dest="log_directly",
            action="store_true",
            default=False,
            help="Logging directory is used directly, no extra dated directories created.",
        )
        return parser

    @classmethod
    def handle_arguments(cls, args, **kwargs):
        """
        Read the arguments specified in this parser and validate the expected inputs.

        :param args: parsed arguments as namespace
        :return: args namespace
        """
        args = copy.copy(args)
        # Get logging dir
        if not args.log_directly:
            args.logs = os.path.abspath(
                os.path.join(
                    args.logs, datetime.datetime.now().strftime("%Y_%m_%d-%H_%M_%S")
                )
            )

        # Make sure directory exists
        try:
            os.makedirs(args.logs)
        except OSError as osexc:
            if osexc.errno != errno.EEXIST:
                raise
        return args


class MiddleWareParser(ParserBase):
    """
    Middleware (ThreadedTcpServer) interface that looks for an address and a port. The argument handling will attempt to
    connect to the socket to ensure that it is a valid address/port and report any errors. This is then immediately
    closes the port after use. There is a minor race-condition between this check and the actual usage, however; it
    should be close enough.
    """

    @staticmethod
    def get_parser():
        """
        Sets up and parses the arguments required to run the data middleware layer. At this time, the data middleware is
        the threaded TCP server and thus the arguments are the ip address and port to listen to. May also be used in
        clients connecting to the middleware layer.

        :return: parser after arguments added
        """
        parser = argparse.ArgumentParser(
            description="Process arguments needed to specify a tool using the middleware",
            add_help=False,
        )
        parser.add_argument(
            "--tts-port",
            dest="tts_port",
            action="store",
            type=int,
            help="Set the threaded TCP socket server port [default: %(default)s]",
            default=50050,
        )
        parser.add_argument(
            "--tts-addr",
            dest="tts_addr",
            action="store",
            type=str,
            help="set the threaded TCP socket server address [default: %(default)s]",
            default="0.0.0.0",
        )
        return parser

    @classmethod
    def handle_arguments(cls, args, **kwargs):
        """
        Checks to ensure that the specified port and address is available before connecting. This prevents user from
        attempting to run on a port that is unavailable.

        :param args: parsed argument namespace
        :return: args namespace
        """
        if "client" not in kwargs or not kwargs["client"]:
            fprime_gds.common.communication.adapters.ip.check_port(
                args.tts_addr, args.tts_port
            )
        return args


class GdsParser(ParserBase):
    """
    Provides a parser for the following arguments:

    - dictionary: path to dictionary, either a folder for py_dicts, or a file for XML dicts
    - logs: path to logging path
    - config: configuration for GDS.

    Note: deployment can help in setting both dictionary and logs, but isn't strictly required.
    """

    @staticmethod
    def get_parser():
        """
        Creates a parser to handle the arguments required to run the GDS layer. This allows the system to start up the
        GDS and connect into the middleware layer.

        :return: parser for arguments
        """
        parser = argparse.ArgumentParser(
            description="Process arguments needed to specify a tool using the GDS",
            add_help=False,
        )
        parser.add_argument(
            "-g",
            "--gui",
            choices=GUIS,
            dest="gui",
            type=str,
            help="Set the desired GUI system for running the deployment. [default: %(default)s]",
            default="html",
        )
        parser.add_argument(
            "--dictionary",
            dest="dictionary",
            action="store",
            default=None,
            required=False,
            type=str,
            help="Path to dictionary. Overrides automatic dictionary detection.",
        )
        parser.add_argument(
            "-c",
            "--config",
            dest="config",
            action="store",
            default=None,
            type=str,
            help="Configuration for wx GUI. Ignored if not using wx.",
        )
        parser.add_argument(
            "--gui-addr",
            dest="gui_addr",
            action="store",
            default="127.0.0.1",
            required=False,
            type=str,
            help="Set the GUI server address [default: %(default)s]",
        )
        parser.add_argument(
            "--gui-port",
            dest="gui_port",
            action="store",
            default="5000",
            required=False,
            type=str,
            help="Set the GUI server address [default: %(default)s]",
        )

        return parser

    @classmethod
    def handle_arguments(cls, args, **kwargs):
        """
        Takes the arguments from the parser, and processes them into the needed map of key to dictionaries for the
        program. This will throw if there is an error.

        :param args: parsed args into a namespace
        :return: args namespace
        """
        args = copy.copy(args)
        # Find dictionary setting via "dictionary" argument or the "deploy" argument
        if args.dictionary is not None and not os.path.exists(args.dictionary):
            raise ValueError(
                "Dictionary file {} does not exist".format(args.dictionary)
            )

        # Handle configuration arguments
        config = fprime_gds.common.utils.config_manager.ConfigManager()
        if args.config is not None and os.path.isfile(args.config):
            config.set_configs(args.config)
        elif args.config is not None:
            raise ValueError("Configuration file {} not found".format(args.config))
        args.config = config
        return args


class BinaryDeployment(ParserBase):
    """
    Parsing subclass used to read the arguments of the binary application. This derives functionality from a comm parser
    and represents the flight-side of the equation.
    """

    @staticmethod
    def get_parser():
        """
        Creates a parser to handle the arguments required to run the binary application. This allows the system to
        start up the F prime binary deployment, or ignore it.

        :return: parser for arguments
        """
        parser = argparse.ArgumentParser(
            description="Process arguments needed for running F prime binary",
            add_help=False,
        )
        parser.add_argument(
            "-n",
            "--no-app",
            dest="noapp",
            action="store_true",
            default=False,
            help="Do not run deployment binary. Overrides --app.",
        )
        parser.add_argument(
            "--app",
            dest="app",
            action="store",
            required=False,
            type=str,
            help="Path to app to run. Overrides automatic app detection.",
        )
        parser.add_argument(
            "-r",
            "--root",
            dest="root_dir",
            action="store",
            required=False,
            type=str,
            help="Root directory of build artifacts, used to automatically find app and dictionary. [default: install_dest field in settings.ini]",
        )
        return parser

    @classmethod
    def handle_arguments(cls, args, **kwargs):
        """
        Takes the arguments from the parser, and processes them into the needed map of key to dictionaries for the
        program. This will throw if there is an error.

        :param args: parsed arguments in namespace
        :return: args namespaces
        """
        # No app, stop processing now
        if args.noapp:
            return args
        args = copy.copy(args)
        if args.app is not None and not os.path.isfile(args.app):
            raise ValueError("F prime binary '{}' does not exist".format(args.app))
        if args.root_dir is not None and not os.path.isdir(args.root_dir):
            raise ValueError(
                "F prime artifacts root directory '{}' does not exist".format(
                    args.root_dir
                )
            )
        return args
