"""
The implementation code for the command-send GDS CLI commands
"""

import difflib
from typing import Iterable, List

import fprime_gds.common.gds_cli.misc_utils as misc_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
from fprime.common.models.serialize.type_exceptions import NotInitializedException
from fprime_gds.common.data_types.cmd_data import CommandArgumentsException
from fprime_gds.common.gds_cli.base_commands import QueryHistoryCommand
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.templates.cmd_template import CmdTemplate
from fprime_gds.common.testing_fw import predicates


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
        return misc_utils.get_cmd_template_string(command_template)

    @classmethod
    def _get_item_list(
        cls,
        project_dictionary: Dictionaries,
        filter_predicate: predicates.predicate,
    ) -> Iterable[CmdTemplate]:
        """
        Gets a list of available commands in the system and return them in an
        ID-sorted list.

        :param project_dictionary: The dictionary object for the project
            containing the command definitions
        :param filter_predicate: Test API predicate used to filter shown
            channels
        """
        # NOTE: Trying to create a blank CmdData causes errors, so currently
        # just using templates (i.e. this function does nothing)
        def create_empty_command(cmd_template):
            return cmd_template

        command_list = test_api_utils.get_item_list(
            item_dictionary=project_dictionary.command_id,
            search_filter=filter_predicate,
            template_to_data=create_empty_command,
        )
        return command_list

    @classmethod
    def _get_upcoming_item(
        cls,
        api,
        filter_predicate,
        min_start_time="NOW",
        timeout=5.0,
    ):
        """
        NOTE: Doesn't use _get_upcoming_item; sign that this should not use QueryHistory as a base class,
        and should refactor when time's available
        """

    @classmethod
    def _get_item_string(
        cls,
        item: CmdTemplate,
        json: bool = False,
    ) -> str:
        """
        Converts the given command template into a human-readable string.

        :param item: The CmdTemplate to convert to a string
        :param json: Whether or not to return a JSON representation of "temp"
        :return: A readable string version of "item"
        """
        return misc_utils.get_cmd_template_string(item, json)

    @classmethod
    def _execute_command(
        cls,
        connection_info: misc_utils.ConnectionInfo,
        search_info: misc_utils.SearchInfo,
        command_name: str,
        arguments: List[str],
    ):
        """
        Handle the command-send arguments to connect to the Test API correctly,
        then send the command via the Test API.

        For more details on these arguments, see the command-send definition at:
            Gds/src/fprime_gds/executables/fprime_cli.py
        """
        dictionary, ip_address, port = tuple(connection_info)
        is_printing_list, ids, components, search, json = tuple(search_info)

        search_filter = cls._get_search_filter(ids, components, search, json)
        if is_printing_list:
            cls._log(cls._list_all_possible_items(dictionary, search_filter, json))
            return

        # ======================================================================
        pipeline, api = test_api_utils.initialize_test_api(
            dictionary, server_ip=ip_address, server_port=port
        )
        # ======================================================================

        try:
            api.send_command(command_name, arguments)
        except KeyError:
            cls._log(f"{command_name} is not a known command")
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

    @classmethod
    def handle_arguments(cls, *args, **kwargs):
        """
        Handle the given input arguments, then execute the command itself

        NOTE: This is currently just a pass-through method
        """
        connection_info = misc_utils.ConnectionInfo(
            kwargs["dictionary"], kwargs["ip_address"], kwargs["port"]
        )
        search_info = misc_utils.SearchInfo(
            kwargs["is_printing_list"],
            kwargs["ids"],
            kwargs["components"],
            kwargs["search"],
            kwargs["json"],
        )

        cls._execute_command(
            connection_info, search_info, kwargs["command_name"], kwargs["arguments"]
        )
