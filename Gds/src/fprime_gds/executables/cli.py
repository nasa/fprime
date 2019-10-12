"""
cli.py:

This file sets up the command line interface and argument parsing that is done to support the F prime executable tools
layer. It is designed to allow users to import standard sets of arguments that applied to the various aspects of the
code that they are importing.

@author mstarch
"""
import os
import re
import errno
import argparse
import datetime

import fprime_gds.common.utils.config_manager


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

class LogDeployParser(object):
    """
    A parser that handles log files by reading in a '--logs' directory or a '--deploy' directory to put the logs into
    as a default.
    """
    # Class variable
    first_log = None

    def add_args(self, parser):
        """
        Adds a log argument and deploy argument.
        :return: parser
        """
        add_safe_argument(parser, "-d", "--deployment", dest="deploy", action="store", required=False, type=str,
                          help="Path to deployment directory. Will be used to find dictionary and app automatically")
        add_safe_argument(parser, "-l", "--logs", dest="logs", action="store", default=None, type=str,
                          help="Logging directory. Created if non-existant.")
        return parser

    def refine_args(self, arguments):
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
        # Make log directory for this run
        if LogDeployParser.first_log is None:
            values["logs"] = os.path.abspath(os.path.join(values["logs"],
                                                          datetime.datetime.now().strftime("%Y_%m_%d-%H_%M_%S")))
            try:
                os.makedirs(values["logs"])
            except OSError as exc:
                if exc.errno != errno.EEXIST:
                    raise
            first_log = values["logs"]
        else:
            values["logs"] = LogDeployParser.first_log
        return values

class MiddleWareParser(LogDeployParser):
    """
    Class defining add_args and validate_args methods for finding middleware CLI inputs. Needed values produced from
    this parser must be the following:

    - port: port to run on.
    - address: address to run on.
    """
    def add_args(self, parser):
        """
        Sets up and parses the arguments required to run the data middleware layer. At this time, the data middleware is
        the threaded TCP server and thus the arguments are the ip address and port to listen to. May also be used in
        clients connecting to the middleware layer.
        :param parser: parser to fill in with arguments
        :return: parser after arguments added
        """
        super(MiddleWareParser, self).add_args(parser)
        add_safe_argument(parser, "-p", "--port", dest="port", action="store", type=int,
                          help="Set the threaded TCP socket server port [default: %(default)s]", default=50000)
        add_safe_argument(parser, "-a", "--addr", dest="addr", action="store", type=str,
                          help="set the threaded TCP socket server address [default: %(default)s]", default="0.0.0.0")
        return parser

    def refine_args(self, arguments):
        """
        Refine the arguments in order to produce the needed values.
        :param arguments: parsed arguments
        :return:
        """
        values = super(MiddleWareParser, self).refine_args(arguments)
        values.update({
            "port": arguments.port,
            "address": arguments.addr
        })
        return values

class GdsParser(LogDeployParser):
    """
    Class defining add_args and validate_args methods for finding GDS CLI inputs. This must produce the following needed
    arguments:

    - dictionary: path to dictionary, either a folder for py_dicts, or a file for XML dicts
    - logs: path to logging path
    - config: configuration for GDS.

    Note: deployment can help in setting both dictionary and logs, but isn't strictly required.
    """
    def __init__(self, is_gds=True):
        """
        Set the type of GDS.
        :param is_gds: is it the new style gds
        """
        self.is_gds = is_gds

    def add_args(self, parser):
        """
        Function to parse GDS required arguments like configuration files, deployment package to read, and a logging
        path to write out
        :param parser: parser to add args to
        :return: parser
        """
        super(GdsParser, self).add_args(parser)
        add_safe_argument(parser, "--dictionary", dest="dictionary", action="store", required=False, type=str,
                          help="Path to dictionary. Overrides deploy if both are set")

        add_safe_argument(parser, "-c", "--config", dest="config", action="store", default=None, type=str,
                          help="Configuration for wx GUI. Ignored if not using wx.")

    def refine_args(self, arguments):
        """
        Takes the arguments from the parser, and processes them into the needed map of key to dictionaries for the
        program. This will throw if there is an error.
        :param arguments: parsed arguments, ready for return.
        :return: values dictionaries
        """
        values = super(GdsParser, self).refine_args(arguments)
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


class BinaryDeployment(MiddleWareParser):
    """
    Class defining add_args and validate_args methods for finding Binary inputs.
    """
    def add_args(self, parser):
        """
        Add arguments used to run the binary application. Uses the middleware layer, so the arguments are also inherited.
        :param parser: parser to add arguments to.
        :return:
        """
        parser = super(BinaryDeployment, self).add_args(parser)
        add_safe_argument(parser, "--app", dest="app", action="store", required=False, type=str,
                          help="Path to app to run. Overrides deploy if both are set.")

    def refine_args(self, arguments):
        """
        Takes the arguments from the parser, and processes them into the needed map of key to dictionaries for the
        program. This will throw if there is an error.
        :param arguments: parsed arguments, ready for return.
        :return: values dictionaries
        """
        values = super(BinaryDeployment, self).refine_args(arguments)
        if arguments.app is not None and os.path.isfile(arguments.app):
            values["app"] = arguments.app
        elif arguments.app is not None and not os.path.isfile(arguments.app):
            raise ValueError("App {} does not exist".format(arguments.app))
        elif arguments.deploy is not None:
            basename = os.path.basename(os.path.normpath(arguments.deploy))
            values["app"] = find_in(basename, arguments.deploy, is_file=True)
            if values["app"] is None:
                raise ValueError("App {} not found in {}. Specify with '--app'.".format(basename, arguments.deploy))
        else:
            raise ValueError("User must supply either the '--dictionary' or '--deployment' argument")
        return values
