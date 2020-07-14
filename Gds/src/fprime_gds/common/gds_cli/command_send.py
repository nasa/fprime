"""
The implementation code for the command-send GDS CLI commands
"""

import difflib
from typing import Iterable, List

from fprime.common.models.serialize.type_exceptions import NotInitializedException
from fprime_gds.common.data_types.cmd_data import CommandArgumentsException
from fprime_gds.common.gds_cli.base_commands import QueryHistoryCommand
import fprime_gds.common.gds_cli.misc_utils as misc_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.templates.cmd_template import CmdTemplate
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.testing_fw.api import IntegrationTestAPI


class CommandSendCommand(QueryHistoryCommand):
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

    @classmethod
    def _get_item_list(
        cls,
        project_dictionary: Dictionaries,
        filter_predicate: predicates.predicate,
        json: bool = False,
    ) -> Iterable[CmdTemplate]:
        """
        Gets a list of available commands in the system and return them in an
        ID-sorted list.

        :param project_dictionary: The dictionary object for the project
            containing the command definitions
        :param filter_predicate: Test API predicate used to filter shown
            channels
        :param json: Whether to print out each item in JSON format or not
        """
        # NOTE: Trying to create a blank CmdData causes errors, so currently
        # just using templates (i.e. this function does nothing)
        create_empty_command = lambda cmd_template: cmd_template

        command_list = test_api_utils.get_item_list(
            item_dictionary=project_dictionary.command_id,
            search_filter=filter_predicate,
            template_to_data=create_empty_command,
        )
        return command_list

    @classmethod
    def _get_upcoming_item(
        cls, api, filter_predicate, min_start_time="",
    ):
        """
        TODO: Doesn't use _get_upcoming_item; sign that this should not use QueryHistory as a base class?
        """
        pass

    @classmethod
    def _get_item_string(cls, item: CmdTemplate, json: bool = False,) -> str:
        """
        Converts the given command template into a human-readable string.

        :param item: The CmdTemplate to convert to a string
        :param json: Whether or not to return a JSON representation of "temp"
        :return: A readable string version of "item"
        """
        return misc_utils.get_cmd_template_string(item, json)

    # TODO: Cut down on the number of arguments here?
    @classmethod
    def handle_arguments(
        cls,
        dictionary: str,
        ip_address: str,
        port: int,
        command_name: str,
        arguments: List[str],
        list: bool,
        ids: Iterable[int],
        components: Iterable[str],
        search: str,
        json: bool,
        *args,
        **kwargs
    ):
        """
        Handle the command-send arguments to connect to the Test API correctly,
        then send the command via the Test API.

        For more details on these arguments, see the command-send definition at:
            Gds/src/fprime_gds/executables/fprime_cli.py
        """
        search_filter = cls._get_search_filter(ids, components, search, json)
        if list:
            cls._log(cls._list_all_possible_items(dictionary, search_filter, json))
            return

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
