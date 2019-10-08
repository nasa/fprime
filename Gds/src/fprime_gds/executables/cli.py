"""
cli.py:

This file sets up the command line interface and argument parsing that is done to support the F prime executable tools
layer. It is designed to allow users to import standard sets of arguments that applied to the various aspects of the
code that they are importing.

@author mstarch
"""
import os
import re
import sys
import errno
import argparse
import datetime
import platform

import fprime_gds.common.utils.config_manager
import fprime_gds.common.adapters.base
# Include basic adapters
import fprime_gds.common.adapters.ip
try:
    import fprime_gds.common.adapters.uart
except ImportError:
    pass
# Try to import each GUI type, and if it can be imported
# it will be provided to the user as an option
GUIS = ["none", "html"]
try:
    import fprime_gds.wxgui.tools.gds
    GUIS.append("wx")
except ImportError as exc:
    pass




def find_in(token, deploy, is_file=True):
    """
    Find token in deploy directory by walking the directory looking for reg-ex
    :param token: token to search for
    :param deploy: directory to start with
    :param is_file: true if looking for file, otherwise false
    :return: full path to token
    """
    for dirpath, dirs, files in os.walk(deploy):
        for check in (files if is_file else dirs):
            if re.match("^" + str(token) + "$", check):
                return os.path.join(dirpath, check)
    return None


def add_safe_argument(parser, *args, **kwargs):
    """
    Wrap add argument call on parser to ensure we catch duplicate errors
    :param parser: parser to add argument to
    :param args: pass-through
    :param kwargs: pass-through
    """
    try:
        parser.add_argument(*args, **kwargs)
    except argparse.ArgumentError as arge:
        if not "conflicting option string" in str(arge):
            raise


ATTR_NAME = "fp_refineries"
def add_refinery(parser, method):
    """
    Adds or updates the "fp_refineries" member attribute to the supplied parser, int order to ensure that we can parse
    all the arguments out of the added arguments.
    Note: this is monkey-patching. Yes it is evil, but it allows for colocation of argument additions and usage.
    :param parser: parser to add or update the attribute
    :param method: argument refining method to call
    """
    if not hasattr(parser, ATTR_NAME):
        setattr(parser, ATTR_NAME, [])
    getattr(parser, ATTR_NAME).append(method)


def refine(parser, args):
    """
    Run all refineries on the arguments.
    :param parser: parser to release
    :param args: arguments to refine
    :return:
    """
    args_out = {}
    for refinery in getattr(parser, ATTR_NAME, []):
        args_out.update(refinery(args))
    return args_out


class LogDeployParser(object):
    """
    A parser that handles log files by reading in a '--logs' directory or a '--deploy' directory to put the logs into
    as a default.
    """
    # Class variable
    first_log = None

    @classmethod
    def add_args(claxx, parser):
        """
        Adds a log argument and deploy argument.
        :return: parser
        """
        add_safe_argument(parser, "-d", "--deployment", dest="deploy", action="store", required=False, type=str,
                          help="Path to deployment directory. Will be used to find dictionary and app automatically")
        add_safe_argument(parser, "-l", "--logs", dest="logs", action="store", default=None, type=str,
                          help="Logging directory. Created if non-existant.")
        add_safe_argument(parser, "--log-directly", dest="log_directly", action="store_true", default=False,
                          help="Logging directory is used directly, no extra dated directories created.")
        add_refinery(parser, claxx.refine_args)
        return parser

    @classmethod
    def refine_args(clazz, arguments):
        """
        Refine the arguments in order to produce the needed values.
        :param arguments: parsed arguments
        :return: values dictionary.
        """
        values = {}
        if arguments.deploy is not None and not os.path.isdir(arguments.deploy):
            raise ValueError("Deployment directory {} does not exist".format(arguments.deploy))
        # Find the log argument via the "logs" argument or the "deploy" argument
        if arguments.deploy is None and arguments.logs is None:
            raise ValueError("User must supply either the '--deployment' or '--logs' argument")
        elif arguments.logs is not None:
            values["logs"] = arguments.logs
        elif arguments.deploy is not None and os.path.isdir(arguments.deploy):
            values["logs"] = os.path.join(arguments.deploy, "logs")
        # Get logging dir
        if arguments.log_directly:
            clazz.first_log = values["logs"]
        elif clazz.first_log is None:
            values["logs"] = os.path.abspath(os.path.join(values["logs"],
                                                          datetime.datetime.now().strftime("%Y_%m_%d-%H_%M_%S")))
            clazz.first_log = values["logs"]
        else:
            values["logs"] = clazz.first_log
        # Make sure directory exists
        try:
            os.makedirs(values["logs"])
        except OSError as exc:
            if exc.errno != errno.EEXIST:
                raise
        return values


class MiddleWareParser(object):
    """
    Class defining add_args and validate_args methods for finding middleware CLI inputs. Needed values produced from
    this parser must be the following:

    - port: port to run on.
    - address: address to run on.
    """
    @classmethod
    def add_args(claxx, parser):
        """
        Sets up and parses the arguments required to run the data middleware layer. At this time, the data middleware is
        the threaded TCP server and thus the arguments are the ip address and port to listen to. May also be used in
        clients connecting to the middleware layer.
        :param parser: parser to fill in with arguments
        :return: parser after arguments added
        """
        LogDeployParser.add_args(parser) # All middleware parsers are log-deploy parsers
        add_safe_argument(parser, "--tts-port", dest="tts_port", action="store", type=int,
                          help="Set the threaded TCP socket server port [default: %(default)s]", default=50050)
        add_safe_argument(parser, "--tts-addr", dest="tts_addr", action="store", type=str,
                          help="set the threaded TCP socket server address [default: %(default)s]", default="0.0.0.0")
        add_refinery(parser, claxx.refine_args)
        return parser

    @classmethod
    def refine_args(claxx, arguments):
        """
        Refine the arguments in order to produce the needed values.
        :param arguments: parsed arguments
        :return:
        """
        values = {}
        values.update({
            "tts_port": arguments.tts_port,
            "tts_address": arguments.tts_addr
        })
        return values


class CommParser(object):
    """
    Class defining add_args and validate_args methods for finding communication CLI inputs. Needed values produced from
    this parser must be the following:

    Adapter type and adapter arguments.

    """
    ADAPTER_PARSERS = {}
    @classmethod
    def add_args(claxx, parser):
        """
        Sets up and parses the arguments required to run the data middleware layer. At this time, the data middleware is
        the threaded TCP server and thus the arguments are the ip address and port to listen to. May also be used in
        clients connecting to the middleware layer.
        :param parser: parser to fill in with arguments
        :return: parser after arguments added
        """
        adapters = fprime_gds.common.adapters.base.BaseAdapter.get_adapters().keys()
        LogDeployParser.add_args(parser)  # All middleware parsers are log-deploy parsers
        add_safe_argument(parser, "--comm-adapter", dest="adapter", action="store", type=str,
                          help="Choose an adapter from the available adapters.", choices=adapters, default="ip")
        for adapter_name in adapters:
            adapter = fprime_gds.common.adapters.base.BaseAdapter.get_adapters()[adapter_name]
            # Check adapter real quick before moving on
            if not hasattr(adapter, "get_arguments") or not callable(getattr(adapter, "get_arguments", None)):
                #LOGGER.error("'{}' does not have 'get_arguments' method, skipping.".format(adapter_name))
                continue
            comm_parser = argparse.ArgumentParser(description='Run F prime communication arguments.')
            # Add arguments for the parser
            for argument in adapter.get_arguments().keys():
                add_safe_argument(comm_parser, *argument, **adapter.get_arguments()[argument])
            CommParser.ADAPTER_PARSERS[adapter_name] = comm_parser
        add_refinery(parser, claxx.refine_args)
        return parser

    @classmethod
    def refine_args(claxx, arguments):
        """
        Refine the arguments in order to produce the needed values.
        :param arguments: parsed arguments
        :return:
        """
        values = {}
        values.update({
            "comm_adapter": arguments.adapter
        })
        if values["comm_adapter"] not in CommParser.ADAPTER_PARSERS:
            raise ValueError("Could not comm adapter in {}"
                             .format(fprime_gds.common.adapters.base.BaseAdapter.get_adapters().keys()))
        args, unknown = CommParser.ADAPTER_PARSERS.get(values["comm_adapter"]).parse_known_args(sys.argv)
        adapter = fprime_gds.common.adapters.base.BaseAdapter.get_adapters()[values["comm_adapter"]]
        values.update(fprime_gds.common.adapters.base.BaseAdapter.process_arguments(adapter, args))
        return values


class GdsParser(object):
    """
    Class defining add_args and validate_args methods for finding GDS CLI inputs. This must produce the following needed
    arguments:

    - dictionary: path to dictionary, either a folder for py_dicts, or a file for XML dicts
    - logs: path to logging path
    - config: configuration for GDS.

    Note: deployment can help in setting both dictionary and logs, but isn't strictly required.
    """
    @classmethod
    def add_args(claxx, parser):
        """
        Function to parse GDS required arguments like configuration files, deployment package to read, and a logging
        path to write out
        :param parser: parser to add args to
        :return: parser
        """
        MiddleWareParser.add_args(parser)
        add_safe_argument(parser,"-g", "--gui", choices=GUIS, dest="gui", type=str,
                          help="Set the desired GUI system for running the deployment. [default: %(default)s]",
                          default="html")
        add_safe_argument(parser, "--dictionary", dest="dictionary", action="store", required=False, type=str,
                          help="Path to dictionary. Overrides deploy if both are set")

        add_safe_argument(parser, "-c", "--config", dest="config", action="store", default=None, type=str,
                          help="Configuration for wx GUI. Ignored if not using wx.")
        add_refinery(parser, claxx.refine_args)
        return parser

    @classmethod
    def refine_args(claxx, arguments):
        """
        Takes the arguments from the parser, and processes them into the needed map of key to dictionaries for the
        program. This will throw if there is an error.
        :param arguments: parsed arguments, ready for return.
        :return: values dictionaries
        """
        values = {"gui": arguments.gui}
        # Find dictionary setting via "dictionary" argument or the "deploy" argument
        if arguments.dictionary is not None and os.path.exists(arguments.dictionary):
            values["dictionary"] = arguments.dictionary
        elif arguments.dictionary is not None and not os.path.exists(arguments.dictionary):
            raise ValueError("Dictionary path {} is not valid 'py_dict' nor XML file".format(arguments.dictionary))
        elif arguments.deploy is not None:
            xml_dict = find_in(".*Dictionary.xml", arguments.deploy, True)
            py_dict = find_in("py_dict", arguments.deploy, False)
            if xml_dict is None and py_dict is None:
                raise ValueError("Could not find python dictionary no XML dictionary in {}".format(arguments.deploy))
            values["dictionary"] = py_dict if xml_dict is None else xml_dict
        else:
            raise ValueError("User must supply either the '--dictionary' or '--deployment' argument")
        # Handle configuration arguments
        values["config"] = fprime_gds.common.utils.config_manager.ConfigManager()
        if arguments.config is not None and os.path.isfile(arguments.config):
            values["config"] = fprime_gds.common.utils.config_manager.ConfigManager()
            values["config"].set_configs(arguments.config)
        elif arguments.config is not None:
            raise ValueError("Configuration file {} not found".format(arguments.config))
        else:
            values["config"] = fprime_gds.common.utils.config_manager.ConfigManager()
        return values


class BinaryDeployment(object):
    """
    Class defining add_args and validate_args methods for finding Binary inputs.
    """
    @classmethod
    def add_args(claxx, parser):
        """
        Add arguments used to run the binary application. Uses the middleware layer, so the arguments are also inherited.
        :param parser: parser to add arguments to.
        :return:
        """
        CommParser.add_args(parser)
        add_safe_argument(parser, "-n", "--no-app", dest="noapp", action="store_true", default=False,
                          help="Do not run deployment binary. Overrides --app.")
        add_safe_argument(parser, "--app", dest="app", action="store", required=False, type=str,
                          help="Path to app to run. Overrides deploy if both are set.")
        add_refinery(parser, claxx.refine_args)

    @classmethod
    def refine_args(claxx, arguments):
        """
        Takes the arguments from the parser, and processes them into the needed map of key to dictionaries for the
        program. This will throw if there is an error.
        :param arguments: parsed arguments, ready for return.
        :return: values dictionaries
        """
        values = {}
        # No app, stop processing now
        if arguments.noapp:
            return values
        if arguments.app is not None and os.path.isfile(arguments.app):
            values["app"] = arguments.app
        elif arguments.app is not None and not os.path.isfile(arguments.app):
            raise ValueError("App {} does not exist".format(arguments.app))
        elif arguments.deploy is not None:
            basename = os.path.basename(os.path.abspath(arguments.deploy))
            values["app"] = find_in(basename, os.path.join(arguments.deploy, "bin", platform.system()), is_file=True)
            if values["app"] is None:
                raise ValueError("App {} not found in {}. Specify with '--app'.".format(basename, arguments.deploy))
        else:
            raise ValueError("User must supply either the '--dictionary' or '--deployment' argument")
        return values
