"""
Handles getting events from the GDS based on some user-specified criteria
"""

import json
import types
from typing import List

import fprime_gds.common.gds_cli.filtering_utils as filtering_utils
import fprime_gds.common.gds_cli.misc_utils as misc_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils

from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.testing_fw import predicates


# TODO: Need to do user tests to find a better print format
def get_event_string(event, as_json: bool = False) -> str:
    """
    Takes in the given event object and prints out a human-readable string
    representation of its information.

    :param event: The event to create a string for
    :param as_json: Return a JSON-string representation of the given event
        instead
    :return: A readable string of the event information
    """
    # TODO: Implement this properly!
    return misc_utils.get_item_string(event)


def print_events_list(
    project_dictionary: Dictionaries,
    filter_predicate: predicates.predicate,
    json: bool = False,
):
    """
    Gets a list of possible events that can occur in the system and prints their
    details out in an ID-sorted list

    :param project_dictionary: The dictionary object for the project containing
        the event type definitions
    :param filter_predicate: Test API predicate used to filter shown events
    :param json: Whether to print out each item in JSON format or not
    """
    event_list = test_api_utils.get_item_list(
        project_dictionary.event_id, filter_predicate, EventData.get_empty_obj
    )
    for event in event_list:
        print(get_event_string(event, json))


def print_upcoming_event(
    api,
    filter_predicate: predicates.predicate,
    min_start_time="NOW",
    json: bool = False,
):
    """
    Prints out the next upcoming event information after the given time that
    matches the given filter (in a way usable by the "repeat_until_interrupt"
    function)
    """
    event_object = test_api_utils.get_upcoming_event(
        api, filter_predicate, min_start_time
    )
    print(get_event_string(event_object, json))
    # Update time so we catch all events since the last one
    if event_object:
        min_start_time = predicates.greater_than(event_object.get_time())
    return (api, filter_predicate, min_start_time, json)


# TODO: Should separate frontend printing code from backend data-getting
# ...at the same time, might be a good idea to keep this strongly coupled to the
# CLI interface code so they stay in sync? (This is essentially frontend code
# right now, but might not have sufficient separation of concerns)
def get_events_output(
    dictionary_path: str,
    ip_address: str,
    port: int,
    list: bool,
    follow: bool,
    ids: List[int],
    components: List[str],
    search: str,
    json: bool,
):
    """
    Takes in the given arguments and prints an appropriate formatted string of
    recent event data that matches the user's criteria.

    For descriptions of these arguments, and more function details, see:
    Gds/src/fprime_gds/executables/fprime_cli.py
    """
    args = misc_utils.QueryCommandArgs(
        dictionary_path, ip_address, port, list, follow, ids, components, search, json
    )
    misc_utils.query_command_template(print_events_list, print_upcoming_event, args)
