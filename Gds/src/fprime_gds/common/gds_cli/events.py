"""
Handles getting events from the GDS based on some user-specified criteria
"""

import os
import time

from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.testing_fw import predicates


def get_event_string(event):
    """
    Takes in the given event object and prints out a human-readable string
    representation of its information
    """
    # TODO: Implement this!
    return None


def get_recent_events(session, url):
    """
    Returns an object containing recent events that have occurred on the
    F' instance (i.e. events that have occurred since the last call with this
    session ID)
    """
    # TODO: Implement this!
    return None


def get_events_list(url):
    """
    Returns an object containing all the possible event types that could occur
    on the running F' instance, and information about each one
    """
    # TODO: Implement this!
    return None


def filter_by_id(object, ids):
    """
    Takes in the given JSON object and filters all the items by their ID number;
    returns a JSON object with only objects of the given ID(s) left

    TODO: Currently assumes the JSON object passed in is a channel/command/event
    object in one of the formats returned by the GDS REST API
    """
    # TODO: Implement this!
    # TODO: Possibly replace with Test API queries?
    return None


def filter_by_component(object, components):
    """
    Takes in the given JSON object and filters all the items by their component
    name; returns a JSON object with only objects from the given component(s)
    left

    TODO: Currently assumes the JSON object passed in is a channel/command/event
    object in one of the formats returned by the GDS REST API
    """
    # TODO: Implement this!
    # TODO: Possibly replace with Test API queries?
    return None


def filter_by_search(object, search_string):
    """
    Takes in the given JSON object and removes any items whose name or string
    description doesn't include the given search term

    TODO: Currently assumes the JSON object passed in is a channel/command/event
    object in one of the formats returned by the GDS REST API
    """
    # TODO: Implement this!
    # TODO: Possibly replace with Test API queries?
    return None


def update_session_id(current_session_id: int):
    """
    Returns a correct session ID if the given one is not correct
    """
    if current_session_id == None:
        # Just use the Unix time as a new, likely-unique session ID
        return time.time()
    return current_session_id


def initialize_test_api(
    app_dictionary_path: str,
    log_path: str = None,
    server_ip: str = "127.0.0.1",
    server_port: int = 50050,
):
    """
    Initializes the Integration Test API for use; returns a tuple of
    (pipeline, api)

    Note that these MUST be manually disconnected when you're done using them,
    by calling "pipeline.disconnect()" and "api.teardown()", respectively
    """
    pipeline = StandardPipeline()
    pipeline.setup(ConfigManager(), app_dictionary_path, "/tmp")
    pipeline.connect(server_ip, server_port)

    # instantiate Test API (log_path of "None" will disable Test API logging)
    api = IntegrationTestAPI(pipeline, log_path)

    return (pipeline, api)


# TODO: Might need to update interface to use Test API correctly
def get_events_output(
    list: bool,
    session: int,
    follow: float,
    id,
    component,
    search: str,
    url: str,
    json: bool,
):
    """
    Takes in the given arguments and returns an appropriate formatted string of
    recent event data that matches the user's criteria

    For descriptions of these arguments, and more function details, see:
    Gds/src/fprime_gds/executables/fprime_cli.py
    """
    # ==========================================================================
    # TODO: Somehow find this dynamically?
    local_dir = os.path.dirname(__file__)
    dict_path = os.path.join(
        local_dir, "../../../../../Ref/Top/RefTopologyAppDictionary.xml"
    )

    pipeline, api = initialize_test_api(dict_path)
    # ==========================================================================

    event_objects = None
    if list:
        event_objects = get_events_list(url)
    else:
        session = update_session_id(session)
        event_objects = get_recent_events(session, url)

    # TODO: Test API may be able to do filtering at search time instead
    event_objects = filter_by_id(event_objects, id)
    event_objects = filter_by_component(event_objects, component)
    event_objects = filter_by_search(event_objects, search)

    # ==========================================================================
    pipeline.disconnect()
    api.teardown()
    # ==========================================================================

    if json:
        # TODO: convert into a JSON object string
        return

    return get_event_string(event_objects)
