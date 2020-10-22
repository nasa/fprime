"""
Handles executing the "events" CLI command for the GDS
"""

from typing import Iterable

import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.gds_cli.base_commands import QueryHistoryCommand
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.testing_fw.api import IntegrationTestAPI


class EventsCommand(QueryHistoryCommand):
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
    ) -> Iterable[EventData]:
        """
        Gets a list of possible events that can occur in the system and returns
        their details in an ID-sorted list.

        :param project_dictionary: The dictionary object for the project
            containing the event type definitions
        :param filter_predicate: Test API predicate used to filter shown events
        :return: List of EventData items that passed the filter
        """
        event_list = test_api_utils.get_item_list(
            item_dictionary=project_dictionary.event_id,
            search_filter=filter_predicate,
            template_to_data=EventData.get_empty_obj,
        )
        return event_list

    @classmethod
    def _get_upcoming_item(
        cls,
        api: IntegrationTestAPI,
        filter_predicate: predicates.predicate,
        min_start_time="NOW",
        timeout: float = 5,
    ) -> EventData:
        """
        Retrieves an F' item that's occurred since the given time and returns
        its data.
        """
        return test_api_utils.get_upcoming_event(
            api, filter_predicate, min_start_time, timeout
        )
