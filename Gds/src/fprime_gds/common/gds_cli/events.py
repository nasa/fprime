"""
Handles getting events from the GDS based on some user-specified criteria
"""


import time

import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
import fprime_gds.common.gds_cli.filtering_utils as filtering_utils

from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.testing_fw import predicates


# TODO: Refactor these methods into different utility files


def get_event_string(event, json: bool = False) -> str:
    """
    Takes in the given event object and prints out a human-readable string
    representation of its information.

    # TODO: possibly make this a generic "get SysData string" instead?
    :param event: The event to create a string for
    :param json: Return a JSON-string representation of the given event instead
    :return: A readable string of the event information
    """
    # TODO: Implement this properly!
    if not event:
        return "No matching item found"
    return event.get_str(verbose=True)


def get_events_list(
    project_dictionary: Dictionaries, search_filter: predicates.predicate
):
    """
    Returns an object containing all the possible event types that could occur
    on the running F' instance, and information about each one.
    """
    # Dictionary has EventTemplates by default, so convert them to EventData
    # so filtering will work properly
    event_dictionary = project_dictionary.event_id
    event_data_list = list(
        map(lambda x: EventData.get_empty_obj(x[1]), event_dictionary.items())
    )

    # Filter by using the given predicate on the event values
    event_data_list = list(filter(lambda x: search_filter(x), event_data_list))
    event_data_list.sort(key=lambda x: x.get_id())
    return event_data_list


# TODO: Find a better name for this, if possible?
def repeat_until_interrupt(func, *args):
    """
    Continues to call the input function with the given arguments until the
    user interrupts it.

    :param func: The function you want to call repeatedly. This MUST return a
        new, updated tuple of the arguments passed into it in the same order,
        which will be used as the new arguments in the next iteration. This is
        done to allow for persistent state between iterations; if needed, create
        a wrapper for your original function to do this.
    :param args: All keyword arguments you want to pass into "func"
    """
    try:
        while True:
            args = func(*args)
    except KeyboardInterrupt:
        pass


# TODO: Try to eliminate need for boolean flag "json" if possible?
def print_upcoming_event(api, filter_predicate, min_start_time="NOW", json=False):
    """
    Prints out the next upcoming event information that matches the given filter
    in a way usable by the "follow" function's API
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
# TODO: Figure out how to refactor this set of arguments to make it more simple?
def get_events_output(
    dictionary_path: str,
    ip_address: str,
    port: int,
    list: bool,
    follow: bool,
    ids,
    components,
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
    # TODO: Need to do user tests to find a better print format
    if list:
        event_objects = get_events_list(pipeline.dictionaries, filter_predicate)
        for event in event_objects:
            print(get_event_string(event, json))
    else:
        if follow:
            repeat_until_interrupt(print_upcoming_event, api, filter_predicate)
        else:
            print_upcoming_event(api, filter_predicate)

    # TODO: Disable Test API from also logging to console somehow?

    # ==========================================================================
    pipeline.disconnect()
    api.teardown()
    # ==========================================================================
