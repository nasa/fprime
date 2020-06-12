"""
Handles executing the "channels" CLI command for the GDS
"""

import types
from typing import List

from fprime_gds.common.gds_cli.base_commands import QueryHistoryCommand
import fprime_gds.common.gds_cli.misc_utils as misc_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils

from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.testing_fw import predicates


class ChannelsCommand(QueryHistoryCommand):
    """
    Takes in the given arguments and prints an appropriate formatted string of
    recent event data that matches the user's criteria.

    For descriptions of these arguments, and more function details, see the
    events command definition at:
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
        Gets a list of open telemetry channels in the system and prints their
        details out in an ID-sorted list.

        :param project_dictionary: The dictionary object for the project
            containing the channel type definitions
        :param filter_predicate: Test API predicate used to filter shown
            channels
        :param json: Whether to print out each item in JSON format or not
        """
        channel_list = test_api_utils.get_item_list(
            project_dictionary.channel_id, filter_predicate, ChData.get_empty_obj
        )
        for channel in channel_list:
            print(misc_utils.get_item_string(channel, json))

    @classmethod
    def print_upcoming_item(
        cls,
        api,
        filter_predicate: predicates.predicate,
        min_start_time="NOW",
        json: bool = False,
    ):
        """
        Prints out the next upcoming channel data after the given time that
        matches the given filter (in a way usable by the
        "repeat_until_interrupt" function).

        # TODO: Possibly move this implementation to the base class? Not sure if
        # other query commands will do anything significantly different besides
        # different print/event-getting functions
        """
        channel_object = test_api_utils.get_upcoming_channel(
            api, filter_predicate, min_start_time
        )
        print(misc_utils.get_item_string(channel_object, json))
        # Update time so we catch all events since the last one
        if channel_object:
            min_start_time = predicates.greater_than(channel_object.get_time())
        return (api, filter_predicate, min_start_time, json)
