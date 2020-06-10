"""
Handles getting events from the GDS based on some user-specified criteria
"""

import os
import time

from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.utils.config_manager import ConfigManager
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.data_types.sys_data import SysData


# TODO: Refactor these methods into different utility files


def get_event_string(event):
    """
    Takes in the given event object and prints out a human-readable string
    representation of its information
    """
    # TODO: Implement this!
    return str(event)


def get_recent_events(session, url):
    """
    Returns an object containing recent events that have occurred on the
    F' instance (i.e. events that have occurred since the last call with this
    session ID)
    """
    # TODO: Implement this!
    return None


# TODO: Possibly move this to the API as a method that can take arbitrary predicates for awaiting events?
def get_upcoming_event(
    test_api: IntegrationTestAPI,
    search_filter: predicates.predicate,
    start_time = "NOW",
    timeout: int = 5,
):
    """
    Returns the next event matching the given search filter that occurs after
    this is called. Times out after the given amount of time if no matching
    new events are found.
    """
    event_filter = predicates.satisfies_all([search_filter, predicates.event_predicate()])
    return test_api.find_history_item(
        event_filter, test_api.get_event_test_history(), start_time, timeout
    )


def get_events_list(url):
    """
    Returns an object containing all the possible event types that could occur
    on the running F' instance, and information about each one
    """
    # TODO: Implement this!
    return None


class id_predicate(predicates.predicate):
    def __init__(self, id):
        """
        A predicate that tests if the SysData argument given to it is of a given
        ID type
        :param id: The ID to compare the item against
        """
        self.id = id

    def __call__(self, item: SysData):
        """
        :param item: the object or value to evaluate
        """
        return self.id == item.get_id()

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x.id == {}".format(self.id)


def get_id_predicate(ids):
    """
    Returns a Test API predicate that only accepts items with one of the given
    type IDs (if no IDs are given, accept all items)
    """
    if ids:
        id_preds = [id_predicate(id) for id in ids]
        return predicates.satisfies_any(id_preds)
    return predicates.always_true()


class component_predicate(predicates.predicate):
    def __init__(self, component: str):
        """
        A predicate that tests if the SysData argument given is from the given
        component. If there is no component information, returns True
        :param component: The component name to check for
        """
        self.comp = component

    def __call__(self, item: SysData):
        """
        :param item: the object or value to evaluate
        """
        # TODO: Apparently, only commands have components stored in their SysData? Find a better way around this than returning true?
        if not item.get_comp_name:
            return True
        return self.comp == item.get_comp_name()

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return 'x is in component "{}"'.format(self.comp)


def get_component_predicate(components):
    """
    Returns a Test API predicate that only accepts items from one of the given
    components (if no components are given, accept all items)
    """
    # TODO: Implement this!
    if components:
        component_preds = [component_predicate(comp) for comp in components]
        return predicates.satisfies_any(component_preds)
    return predicates.always_true()


class contains_search_string(predicates.predicate):
    def __init__(self, search_string: str):
        """
        A predicate that tests if the argument given to it contains the
        passed-in string (after the argument is converted to a string via
        __str__)
        :param search_string: The exact text to check for inside the object
        """
        self.search_string = str(search_string)

    def __call__(self, item):
        """
        :param item: the object or value to evaluate
        """
        return self.search_string in str(item)

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return 'str(x) contains "{}"'.format(self.search_string)


def get_search_predicate(search_string: str):
    """
    Returns a Test API predicate that only accepts items whose string
    representation contains the exact given term
    """
    if search_string:
        return contains_search_string(search_string)
    return predicates.always_true()


def get_full_filter_predicate(ids, components, search_string):
    """
    Returns a Test API predicate to only get recent data from the specified
    IDs/components, and containing the given search string. If any of these
    are left blank, no restrictions are assumed for that field
    """
    return_all = predicates.always_true()

    id_pred = get_id_predicate(ids)
    comp_pred = get_component_predicate(components)
    search_pred = get_search_predicate(search_string)

    return predicates.satisfies_all([return_all, id_pred, comp_pred, search_pred])


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
# TODO: Should separate frontend printing code from backend data-getting
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
    Takes in the given arguments and prints an appropriate formatted string of
    recent event data that matches the user's criteria

    For descriptions of these arguments, and more function details, see:
    Gds/src/fprime_gds/executables/fprime_cli.py
    """
    # ==========================================================================
    # TODO: Somehow find this dynamically, or via an arg?
    local_dir = os.path.dirname(__file__)
    dict_path = os.path.join(
        local_dir, "../../../../../Ref/Top/RefTopologyAppDictionary.xml"
    )

    pipeline, api = initialize_test_api(dict_path)
    # ==========================================================================

    filter_predicate = get_full_filter_predicate(id, component, search)
    event_objects = None

    # TODO: Follow currently just prints all new events as they come, with no
    # set printing frequency; update interface/API to reflect this?

    # Get objects at least once, and continue looping if follow is set and we're
    # not listing events
    min_start_time = "NOW"
    try:
        while True:
            if list:
                event_objects = get_events_list(url)
            else:
                session = update_session_id(session)
                event_objects = get_upcoming_event(api, filter_predicate, min_start_time)

            # TODO: Print returned objects properly, instead of leaving it up to Test API
            if json:
                # TODO: convert into a JSON object string
                pass
            else:
                print(get_event_string(event_objects))

            if list or not follow:
                break

            # Update time so we catch all events since the last one
            if event_objects:
                min_start_time = predicates.greater_than(event_objects.get_time())
    except KeyboardInterrupt:
        # Break the loop and teardown the API/pipeline
        pass

    # ==========================================================================
    pipeline.disconnect()
    api.teardown()
    # ==========================================================================
