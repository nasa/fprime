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

    # TODO: possibly make this a generic "get SysData string" instead?
    :param event: The event to create a string for
    :param as_json: Return a JSON-string representation of the given event
        instead
    :return: A readable string of the event information
    """
    # TODO: Implement this properly!
    if not event:
        return "No matching item found"
    if as_json:
        # TODO: "json.dumps" doesn't seem to work correctly yet?
        pass
    return event.get_str(verbose=True)


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
    :param filter_predicate:
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


# TODO: Should I try and make a generic version of this to share with commands
# and telemetry, or keep their functionality separate so they're not
# co-dependent?
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
    # ==========================================================================
    pipeline, api = test_api_utils.initialize_test_api(
        dictionary_path, server_ip=ip_address, server_port=port
    )
    # ==========================================================================

    filter_predicate = filtering_utils.get_full_filter_predicate(
        ids, components, search
    )

    # TODO: Try and refactor this to avoid nested if statements?
    if list:
        print_events_list(pipeline.dictionaries, filter_predicate, json)
    else:
        if follow:
            misc_utils.repeat_until_interrupt(
                print_upcoming_event, api, filter_predicate, "NOW", json
            )
        else:
            print_upcoming_event(api, filter_predicate, json=json)

    # TODO: Disable Test API from also logging to console somehow?

    # ==========================================================================
    pipeline.disconnect()
    api.teardown()
    # ==========================================================================
