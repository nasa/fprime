"""
Handles executing the "channels" CLI command for the GDS
"""

from typing import Iterable

import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
from fprime_gds.common.data_types.ch_data import ChData
from fprime_gds.common.gds_cli.base_commands import QueryHistoryCommand
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.testing_fw.api import IntegrationTestAPI


class ChannelsCommand(QueryHistoryCommand):
    """
    Takes in the given arguments and prints an appropriate formatted string of
    recent event data that matches the user's criteria.

    For descriptions of these arguments, and more function details, see the
    events command definition at:
        Gds/src/fprime_gds/executables/fprime_cli.py
    """

    @classmethod
    def _get_item_list(
        cls,
        project_dictionary: Dictionaries,
        filter_predicate: predicates.predicate,
    ) -> Iterable[ChData]:
        """
        Gets a list of open telemetry channels in the system and prints their
        details out in an ID-sorted list.

        :param project_dictionary: The dictionary object for the project
            containing the channel type definitions
        :param filter_predicate: Test API predicate used to filter shown
            channels
        :return: List of channels that passed the filter
        """
        channel_list = test_api_utils.get_item_list(
            item_dictionary=project_dictionary.channel_id,
            search_filter=filter_predicate,
            template_to_data=ChData.get_empty_obj,
        )
        return channel_list

    @classmethod
    def _get_upcoming_item(
        cls,
        api: IntegrationTestAPI,
        filter_predicate: predicates.predicate,
        min_start_time="NOW",
        timeout: float = 5,
    ) -> ChData:
        """
        Returns the next upcoming channel data after the given time that
        matches the given filter.
        """
        return test_api_utils.get_upcoming_channel(
            api, filter_predicate, min_start_time, timeout
        )
