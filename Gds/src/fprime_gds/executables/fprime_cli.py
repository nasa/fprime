#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

"""
Parses the given CLI command for the F' GDS, and then executes the correct
command with the user-provided arguments on the GDS
"""

import abc
import argparse
from copy import deepcopy
import os
import sys
from typing import Callable, List

import argcomplete

"""
TODO: These modules are now only lazily loaded below as needed, due to slow
performance when importing them; if possible, find a way to speed up the actual
importing

import fprime_gds.common.gds_cli.channels as channels
import fprime_gds.common.gds_cli.commands as commands
import fprime_gds.common.gds_cli.command_send as command_send
import fprime_gds.common.gds_cli.events as events
"""
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.executables.cli import GdsParser


def add_connection_arguments(parser: argparse.ArgumentParser):
    """
    Adds the arguments needed to properly connect to the API, which the user may
    want to specify
    """
    parser.add_argument(
        "-d",
        "--dictionary",
        type=str,
        default=None,
        help='path from the current working directory to the "<project name>Dictionary.xml" file for the project you\'re using the API with; if unused, tries to search the current working directory for such a file',
    )
    parser.add_argument(
        "-ip",
        "--ip-address",
        type=str,
        default="127.0.0.1",
        help="connect to the GDS server using the given IP or hostname (default=127.0.0.1 (i.e. localhost))",
        metavar="IP",
    )
    parser.add_argument(
        "-p",
        "--port",
        type=int,
        default=50050,
        help="connect to the GDS server using the given port number (default=50050)",
        metavar="PORT",
    )


def add_history_arguments(parser: argparse.ArgumentParser, command_name: str):
    """
    Adds all the arguments/flags normally used by the Channels/Commands/Events
    commands with the given name used in the help text, due to the similarity
    of each of these commands (at least currently)
    """
    add_connection_arguments(parser)
    parser.add_argument(
        "-l",
        "--list",
        action="store_true",
        help="list all possible %s types the current F Prime instance could produce, based on the %s dictionary, sorted by %s type ID"
        % (command_name[:-1], command_name, command_name[:-1]),
    )
    parser.add_argument(
        "-f",
        "--follow",
        action="store_true",
        help="continue polling the API and printing out new %s to the console until the user exits (via CTRL+C)"
        % (command_name),
    )
    parser.add_argument(
        "-i",
        "--ids",
        nargs="+",
        type=int,
        help='only show %s matching the given type ID(s) "ID"; can provide multiple IDs to show all given types'
        % (command_name),
        metavar="ID",
    )
    parser.add_argument(
        "-c",
        "--components",
        nargs="+",
        type=str,
        help='only show %s from the given component name "COMP"; can provide multiple components to show %s from all components given'
        % (command_name, command_name),
        metavar="COMP",
    )
    parser.add_argument(
        "-s",
        "--search",
        type=str,
        help='only show %s whose name or output string exactly matches or contains the entire given string "STRING"'
        % (command_name),
        metavar="STRING",
    )
    parser.add_argument(
        "-j",
        "--json",
        action="store_true",
        help="return the JSON response of the API call, with %s filtered based on other flags provided"
        % (command_name),
    )


class CliCommandParserBase(abc.ABC):
    """
    An abstract class for CLI commands to implement; provides methods for
    adding a new parser for this command
    """

    @classmethod
    def add_subparser(cls, parent_parser: argparse.ArgumentParser):
        """
        Adds this command as a sub-command to an existing parser, so that it
        can be passed in as an argument (similar to git's CLI tool)
        """
        command_parser = cls.create_subparser(parent_parser)
        cls.add_arguments(command_parser)
        command_parser.set_defaults(func=cls.command_func)

    @classmethod
    @abc.abstractmethod
    def create_subparser(
        cls, parent_parser: argparse.ArgumentParser
    ) -> argparse.ArgumentParser:
        """
        Creates the parser for this command as a subparser of the given one,
        and then returns it
        """
        pass

    @classmethod
    @abc.abstractmethod
    def add_arguments(cls, parser: argparse.ArgumentParser):
        """
        Add all the required and optional arguments for this command to the
        given parser
        """
        pass

    @classmethod
    @abc.abstractmethod
    def command_func(cls, *args, **kwargs) -> Callable:
        """
        Executes the appropriate function when this command is called
        """
        pass


class ChannelsParser(CliCommandParserBase):
    """
    A parser for the "channels" CLI command, which lets users retrieve
    information about recent telemetry data from an F' instance
    """

    @classmethod
    def create_subparser(cls, parent_parser: argparse.ArgumentParser):
        """
        Creates the channels sub-command as a subparser, and then returns it
        """
        channels_parser = parent_parser.add_parser(
            "channels",
            description="print out new telemetry data that has been received from the F Prime instance, sorted by timestamp",
        )
        return channels_parser

    @classmethod
    def add_arguments(cls, parser: argparse.ArgumentParser):
        """
        Add all the required and optional arguments for this command to the
        given parser
        """
        add_history_arguments(parser, "channels")

    @classmethod
    def command_func(cls, *args, **kwargs):
        """
        Executes the appropriate function when "channels" is called
        """
        import fprime_gds.common.gds_cli.channels as channels

        channels.ChannelsCommand.handle_arguments(*args, **kwargs)


class CommandsParser(CliCommandParserBase):
    """
    A parser for the "commands" CLI command, which lets users retrieve
    information about what commands are available/have recently been run on an
    F' instance
    """

    @classmethod
    def create_subparser(cls, parent_parser: argparse.ArgumentParser):
        """
        Creates the commands sub-command as a subparser, and then returns it
        """
        commands_parser = parent_parser.add_parser(
            "commands",
            description="prints out new commands that have been received by the F Prime instance, sorted by timestamp (NOTE: listing available commands works currently, but command history is unimplemented; use 'events' instead to see received commands)",
        )
        return commands_parser

    @classmethod
    def add_arguments(cls, parser: argparse.ArgumentParser):
        """
        Add all the required and optional arguments for this command to the
        given parser
        """
        add_history_arguments(parser, "commands")

    @classmethod
    def command_func(cls, *args, **kwargs):
        """
        Executes the appropriate function when "commands" is called
        """
        import fprime_gds.common.gds_cli.commands as commands

        commands.CommandsCommand.handle_arguments(*args, **kwargs)


class CommandSendParser(CliCommandParserBase):
    """
    A parser for the "command-send" CLI command, which lets users send commands
    from the GDS to a running F' instance
    """

    @classmethod
    def create_subparser(cls, parent_parser: argparse.ArgumentParser):
        """
        Creates the command-send sub-command as a subparser, and then returns it
        """
        command_send_parser = parent_parser.add_parser(
            "command-send",
            description="sends the given command to the spacecraft via the GDS",
        )
        return command_send_parser

    # TODO: Move to a more general function?
    @staticmethod
    def get_dictionary_path(current_args: argparse.Namespace) -> str:
        """
        Returns the current project dictionary, either one provided by the user
        or the first one found in the current working directory. Raises an
        exception if neither one is found.
        """
        args = deepcopy(current_args)
        if not hasattr(args, "dictionary"):
            args.dictionary = None
        args.deploy = os.getcwd()
        args.config = None
        args = GdsParser.handle_arguments(args, kwargs={})
        return args.dictionary

    @staticmethod
    def complete_command_name(
        prefix: str, parsed_args: argparse.Namespace, **kwargs
    ) -> List[str]:
        """
        Returns a list of all command names that could possibly complete the
        given prefix
        """
        dict_path = ""
        try:
            dict_path = CommandSendParser.get_dictionary_path(parsed_args)
        except ValueError:
            argcomplete.warn("No dictionary found to get command names from")
            return []
        dictionary = Dictionaries()
        dictionary.load_dictionaries(dict_path, None)
        command_names = dictionary.command_name.keys()
        return [name for name in command_names if name.startswith(prefix)]

    @classmethod
    def add_arguments(cls, parser: argparse.ArgumentParser):
        """
        Add all the required and optional arguments for this command to the
        given parser
        """
        add_connection_arguments(parser)
        parser.add_argument(
            "command_name",
            help='the full name of the command you want to execute in "<component>.<name>" form',
            metavar="command-name",
        ).completer = cls.complete_command_name
        # NOTE: Type set to string because we don't know the type beforehand
        parser.add_argument(
            "-args",
            "--arguments",
            nargs="*",
            type=str,
            default=[],
            help="provide a space-separated set of arguments to the command being sent",
        )

    @classmethod
    def command_func(cls, *args, **kwargs):
        """
        Executes the appropriate function when "command_send" is called
        """
        import fprime_gds.common.gds_cli.command_send as command_send

        command_send.CommandSendCommand.handle_arguments(*args, **kwargs)


class EventsParser(CliCommandParserBase):
    """
    A parser for the "events" CLI command, which lets users retrieve
    information about recent events logged on an F' instance
    """

    @classmethod
    def create_subparser(cls, parent_parser: argparse.ArgumentParser):
        """
        Creates the events sub-command as a subparser, and then returns it
        """
        events_parser = parent_parser.add_parser(
            "events",
            description="print out new events that have occurred on the F Prime instance, sorted by timestamp",
        )
        return events_parser

    @classmethod
    def add_arguments(cls, parser: argparse.ArgumentParser):
        """
        Add all the required and optional arguments for this command to the
        given parser
        """
        add_history_arguments(parser, "events")

    @classmethod
    def command_func(cls, *args, **kwargs):
        """
        Executes the appropriate function when "events_parser" is called
        """
        import fprime_gds.common.gds_cli.events as events

        events.EventsCommand.handle_arguments(*args, **kwargs)


def create_parser():
    parser = argparse.ArgumentParser(
        description="provides utilities for interacting with the F' Ground Data System (GDS)"
    )
    parser.add_argument("-V", "--version", action="version", version="0.0.2")

    # Add subcommands to the parser
    subparsers = parser.add_subparsers(dest="func")

    ChannelsParser.add_subparser(subparsers)
    CommandsParser.add_subparser(subparsers)
    CommandSendParser.add_subparser(subparsers)
    EventsParser.add_subparser(subparsers)

    return parser


def add_valid_dictionary(args: argparse.Namespace) -> argparse.Namespace:
    """
    Takes in the given parsed arguments and, if no F' dictionary has been given,
    attempt to search for one in the current working directory. Throw an error
    if none can be found OR if the given dictionary is invalid.
    """
    args.dictionary = CommandSendParser.get_dictionary_path(args)
    return args


def parse_args(parser: argparse.ArgumentParser, arguments):
    """
    Parses the given arguments and returns the resulting namespace; having this
    separate allows for unit testing if needed
    """
    args = parser.parse_args(arguments)

    # TODO: In the future not all commands may need the dictionary, so this may
    # not be the right place
    try:
        args = add_valid_dictionary(args)
    except ValueError:
        print("No valid project dictionary found")
        sys.exit()

    return args


def main():
    # parse arguments, not including the name of this script
    parser = create_parser()
    argcomplete.autocomplete(parser)
    args = parse_args(parser, sys.argv[1:])

    if not args.func:
        # no argument provided, so print the help message
        parser.print_help()
        return

    # call the selected function with the args provided
    function = args.func
    argument_dict = vars(args)
    del argument_dict["func"]  # Remove function
    function(**argument_dict)


if __name__ == "__main__":
    main()
