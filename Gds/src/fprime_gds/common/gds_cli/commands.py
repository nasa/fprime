"""
Handles executing the "commands" CLI command for the GDS
"""

import types
from typing import List

from fprime_gds.common.gds_cli.base_commands import QueryHistoryCommand
import fprime_gds.common.gds_cli.misc_utils as misc_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils

from fprime_gds.common.data_types.cmd_data import CmdData
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.testing_fw import predicates


def get_cmd_template_string(temp):
    cmd_string = "%s (%d) | Takes %d arguments.\n" % (
        temp.get_full_name(),
        temp.get_id(),
        len(temp.get_args()),
    )

    for arg in temp.get_args():
        arg_description = "--no description--"
        # TODO: Compare against actual module, not just the name (but EnumType
        # is a serializable type from the dictionary?
        if type(arg[2]).__name__ == "EnumType":
            arg_description = str(arg[2].keys())

        cmd_string += "\t%s (%s): %s\n" % (
            arg[0],
            type(arg[2]).__name__,
            arg_description,
        )

    return cmd_string


class CommandsCommand(QueryHistoryCommand):
    """
    Takes in the given arguments and prints an appropriate formatted string of
    recently executed commands that match the user's criteria.

    For descriptions of these arguments, and more function details, see the
    commands command definition at:
        Gds/src/fprime_gds/executables/fprime_cli.py
    """

    @classmethod
    def print_items_list(
        cls,
        project_dictionary: Dictionaries,
        filter_predicate: predicates.predicate,
        json: bool = False,
    ):
        """
        Gets a list of available in the system and prints their details out in
        an ID-sorted list.

        :param project_dictionary: The dictionary object for the project
            containing the command definitions
        :param filter_predicate: Test API predicate used to filter shown
            channels
        :param json: Whether to print out each item in JSON format or not
        """
        # TODO: Trying to create a blank command causes errors; find a more
        # robust method?
        def create_empty_command(cmd_template):
            return cmd_template

        command_list = test_api_utils.get_item_list(
            project_dictionary.command_id, filter_predicate, create_empty_command
        )

        # TODO: Actually use JSON?
        for command in command_list:
            print(get_cmd_template_string(command))

    @classmethod
    def print_upcoming_item(
        cls,
        api,
        filter_predicate: predicates.predicate,
        min_start_time="NOW",
        json: bool = False,
    ):
        """
        Prints out the next upcoming command data after the given time that
        matches the given filter (in a way usable by the
        "repeat_until_interrupt" function).
        """
        command_object = test_api_utils.get_upcoming_command(
            api, filter_predicate, min_start_time
        )
        print(misc_utils.get_item_string(command_object, json))
        # Update time so we catch all events since the last one
        if command_object:
            min_start_time = predicates.greater_than(command_object.get_time())
        return (api, filter_predicate, min_start_time, json)
