#!/usr/bin/env python3

"""
Parses the given CLI command for the F' GDS, and then executes the correct
command with the user-provided arguments on the GDS
"""

import abc
import argparse
import sys

import fprime_gds.common.gds_cli.events as events


def PLACEHOLDER_FUNC(**kwargs):
    """
    TODO: REMOVE!
    A placeholder function that just prints out the arguments it's given
    """
    print(kwargs)


def add_connection_arguments(parser: argparse.ArgumentParser):
    """
    Adds the arguments needed to properly connect to the API, which the user may
    want to specify
    """
    parser.add_argument(
        "-d",
        "--dictionary-path",
        type=str,
        help="path from the current working directory to the \"<project name>AppDictionary.xml\" file for the project you're using the API with",
        metavar="PATH",
    )
    parser.add_argument(
        "-ip",
        "--ip-address",
        type=str,
        default="127.0.0.1",
        help='connect to the GDS server using the given IP or hostname (default=127.0.0.1 (i.e. localhost))',
        metavar="IP",
    )
    parser.add_argument(
        "-p",
        "--port",
        type=int,
        default=50050,
        help='connect to the GDS server using the given port number (default=50050)',
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
        help="list all possible %s types the current F Prime instance could produce, based on the %s dictionary, sorted by %s type ID; ignores the session ID"
        % (command_name[:-1], command_name, command_name[:-1]),
    )
    parser.add_argument(
        "-f",
        "--follow",
        action="store_true",
        help='continue polling the API and printing out new %s to the console until the user exits (via CTRL+C)'
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
        "-S",
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
        command_parser.set_defaults(func=cls.get_command_func())

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
    def add_arguments(cls, parser):
        """
        Add all the required and optional arguments for this command to the
        given parser
        """
        pass

    @classmethod
    @abc.abstractmethod
    def get_command_func(cls):
        """
        Returns the function that should be executed when this command is called
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
    def get_command_func(cls):
        """
        Returns the function that should be executed when "channels" is called
        """
        return PLACEHOLDER_FUNC


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
            description="prints out the history of new commands that have been received by the F Prime instance, sorted by timestamp",
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
    def get_command_func(cls):
        """
        Returns the function that should be executed when "commands" is called
        """
        return PLACEHOLDER_FUNC


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

    @classmethod
    def add_arguments(cls, parser: argparse.ArgumentParser):
        """
        Add all the required and optional arguments for this command to the
        given parser
        """
        parser.add_argument(
            "command_name",
            help='the full name of the command you want to execute in "<component>.<name>" form',
        )
        add_connection_arguments(parser)
        parser.add_argument(
            "-l",
            "--list",
            action="store_true",
            help="list all commands available on the current F Prime instance; identical to `commands --list`",
        )
        parser.add_argument(
            "-k",
            "--key",
            type=str,
            help='the sanity-check key to prevent accidental commands from being sent; "K" should be given as "0xfeedcafe" to send an actual command, otherwise no command will be sent',
            metavar="K",
        )
        # NOTE: Type set to string because we don't know the type beforehand
        parser.add_argument(
            "-args",
            "--arguments",
            nargs="*",
            type=str,
            help="provide a space-separated set of arguments to the command being sent",
        )

    @classmethod
    def get_command_func(cls):
        """
        Returns the function that should be executed when "command-send" is
        called
        """
        return PLACEHOLDER_FUNC


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
    def get_command_func(cls):
        """
        Returns the function that should be executed when "events" is called
        """
        return events.get_events_output


def create_parser():
    parser = argparse.ArgumentParser(
        description="provides utilities for dealing with random numbers"
    )
    parser.add_argument("-V", "--version", action="version", version="1.0.0")

    # Add subcommands to the parser
    subparsers = parser.add_subparsers(dest="func")
    ChannelsParser.add_subparser(subparsers)
    CommandsParser.add_subparser(subparsers)
    CommandSendParser.add_subparser(subparsers)
    EventsParser.add_subparser(subparsers)

    return parser


def parse_args(parser, arguments):
    """
    Parses the given arguments and returns the resulting namespace; having this
    separate allows for unit testing if needed
    """
    return parser.parse_args(arguments)


def main():
    # parse arguments, not including the name of this script
    parser = create_parser()
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
