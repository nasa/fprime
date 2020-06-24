"""
The implementation code for the command-send GDS CLI commands
"""

import difflib
from typing import List

from fprime.common.models.serialize.type_exceptions import NotInitializedException
from fprime_gds.common.data_types.cmd_data import CommandArgumentsException
from fprime_gds.common.gds_cli.base_commands import BaseCommand
import fprime_gds.common.gds_cli.misc_utils as misc_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.templates.cmd_template import CmdTemplate


class CommandSendCommand(BaseCommand):
    """
    The implementation for sending a command via the GDS to the spacecraft
    """

    @staticmethod
    def get_closest_commands(
        project_dictionary: Dictionaries, command_name: str, num: int = 3
    ) -> List[str]:
        """
        Searches for the closest matching known command(s) to the given command
        name.

        :param project_dictionary: The dictionary object for this project
            containing the item type definitions
        :param command_name: The full string name of the command to search for
        :param num: The maximum number of near-matches to return
        :return: A list of the closest matching commands (potentially empty)
        """
        known_commands = project_dictionary.command_name.keys()
        closest_matches = difflib.get_close_matches(command_name, known_commands, n=num)
        return closest_matches

    # TODO: Make this a method on one of the pipeline classes instead?
    @staticmethod
    def get_command_template(
        project_dictionary: Dictionaries, command_name: str
    ) -> CmdTemplate:
        """
        Retrieves the command template for the given command name

        :param project_dictionary: The dictionary object for this project
            containing the item type definitions
        :param command_name: The full string name of the command to return a
            template for
        :return: The CmdTemplate object for the given command
        """
        return project_dictionary.command_name[command_name]

    @staticmethod
    def get_command_help_message(
        project_dictionary: Dictionaries, command_name: str
    ) -> str:
        """
        Returns a string showing a help message for the given GDS command.

        :param project_dictionary: The dictionary object for this project
            containing the item type definitions
        :param command_name: The full string name of the command to return a
            help message for
        :return: A help string for the command
        """
        command_template = CommandSendCommand.get_command_template(
            project_dictionary, command_name
        )
        # TODO: Refactor CommandsCommand's method into a common class, since
        # this is technically a private method?
        return misc_utils.get_cmd_template_string(command_template)

    # TODO: Cut down on the number of arguments here?
    @classmethod
    def handle_arguments(
        cls,
        dictionary: str,
        ip_address: str,
        port: int,
        command_name: str,
        arguments: List[str],
        *args,
        **kwargs
    ):
        """
        Handle the command-send arguments to connect to the Test API correctly,
        then send the command via the Test API.

        For more details on these arguments, see the command-send definition at:
            Gds/src/fprime_gds/executables/fprime_cli.py
        """
        # TODO: Make this api setup part of a decorator somehow, since it
        # recurs in several places?
        # ======================================================================
        pipeline, api = test_api_utils.initialize_test_api(
            dictionary, server_ip=ip_address, server_port=port
        )
        # ======================================================================

        try:
            api.send_command(command_name, arguments)
        except KeyError:
            cls._log("'%s' is not a known command" % (command_name))
            close_matches = CommandSendCommand.get_closest_commands(
                pipeline.dictionaries, command_name
            )
            if close_matches:
                cls._log("Similar known commands: {}".format(close_matches))
        except NotInitializedException:
            temp = CommandSendCommand.get_command_template(
                pipeline.dictionaries, command_name
            )
            cls._log(
                "'%s' requires %d arguments (%d given)"
                % (command_name, len(temp.get_args()), len(arguments))
            )
            cls._log(cls.get_command_help_message(pipeline.dictionaries, command_name))
        except CommandArgumentsException as err:
            cls._log("Invalid arguments given; %s" % (str(err)))
            cls._log(cls.get_command_help_message(pipeline.dictionaries, command_name))

        # ======================================================================
        pipeline.disconnect()
        api.teardown()
        # ======================================================================
