"""
Handles executing the "commands" CLI command for the GDS
"""

from typing import Iterable

from fprime_gds.common.data_types.cmd_data import CmdData
from fprime_gds.common.gds_cli.base_commands import QueryHistoryCommand
import fprime_gds.common.gds_cli.misc_utils as misc_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.templates.cmd_template import CmdTemplate
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.testing_fw.api import IntegrationTestAPI


class CommandsCommand(QueryHistoryCommand):
    """
    Takes in the given arguments and prints an appropriate formatted string of
    recently executed commands that match the user's criteria.

    For descriptions of these arguments, and more function details, see the
    commands command definition at:
        Gds/src/fprime_gds/executables/fprime_cli.py
    """

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
        cls,
        api: IntegrationTestAPI,
        filter_predicate: predicates.predicate,
        min_start_time="NOW",
    ) -> CmdData:
        """
        Returns the most recent command happening after the given time that
        matches the given filter.
        """
        return test_api_utils.get_upcoming_command(
            api, filter_predicate, min_start_time
        )

    @classmethod
    def _get_item_string(cls, item: CmdTemplate, json: bool = False,) -> str:
        """
        Converts the given command template into a human-readable string.

        :param item: The CmdTemplate to convert to a string
        :param json: Whether or not to return a JSON representation of "temp"
        :return: A readable string version of "item"
        """
        if not item:
            return "No matching commands found"
        if json:
            return misc_utils.get_item_json_string(item)

        cmd_string = "%s (%d) | Takes %d arguments.\n" % (
            item.get_full_name(),
            item.get_id(),
            len(item.get_args()),
        )

        for arg in item.get_args():
            arg_name, arg_description, arg_type = arg
            if not arg_description:
                arg_description = "--no description--"
            # TODO: Compare against actual module, not just the name (but
            # how, since EnumType is a serializable type from the dictionary?)
            if type(arg_type).__name__ == "EnumType":
                # TODO: Find good way to combine this w/ description, if one exists?
                arg_description = str(arg_type.keys())

            cmd_string += "\t%s (%s): %s\n" % (
                arg_name,
                type(arg_type).__name__,
                arg_description,
            )

        return cmd_string
