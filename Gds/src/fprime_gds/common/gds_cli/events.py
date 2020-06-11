"""
Handles getting events from the GDS based on some user-specified criteria
"""

import os
import time

from fprime_gds.common.data_types.sys_data import SysData
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.utils.config_manager import ConfigManager


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


# TODO: Possibly move this to the API as a method that can take arbitrary predicates for awaiting events?
def get_upcoming_event(
    test_api: IntegrationTestAPI,
    search_filter: predicates.predicate,
    start_time="NOW",
    timeout: int = 5,
) -> EventData:
    """
    Returns the next event matching the given search filter that occurs after
    this is called. Times out after the given amount if no matching new events
    are found.

    :param test_api: An API instance that will be called to find the next event
    :param search_filter: A predicate each found event is tested against; if
        the event doesn't test "True" against this, we ignore it and keep
        searching
    :param start_time: An optional index or predicate to specify the earliest
        event to search for; defaults to only accepting events after the time
        this function is called
    :param timeout: The maximum time (in seconds) to wait for an event; if no
        event is found that passes the search filter within this time, we stop
        and return None

    :return: The first "EventData" found that passes the filter, or "None" if no
        such event is found
    """
    event_filter = predicates.satisfies_all(
        [search_filter, predicates.event_predicate()]
    )
    return test_api.find_history_item(
        event_filter, test_api.get_event_test_history(), start_time, timeout
    )


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


class id_predicate(predicates.predicate):
    def __init__(self, id):
        """
        A predicate that tests if the SysData argument given to it is of a given
        ID type.

        :param id: The ID to compare the item against
        """
        self.id = id

    def __call__(self, item: SysData):
        """
        :param item: The object or value to evaluate
        """
        return self.id == item.get_id()

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x.id == {}".format(self.id)


def get_id_predicate(ids) -> predicates.predicate:
    """
    Returns a Test API predicate that only accepts items with one of the given
    type IDs (if no IDs are given, accept all items).

    :param ids: A list of possible ID values to accept (empty if we should
        accept SysData with any ID)
    :return: A predicate that checks if a SysData object has one of the given
        IDs
    """
    if ids:
        id_preds = [id_predicate(id) for id in ids]
        return predicates.satisfies_any(id_preds)
    return predicates.always_true()


class component_predicate(predicates.predicate):
    def __init__(self, component: str):
        """
        A predicate that tests if the SysData argument given is from the given
        component. If there is no component information found, returns True.

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


def get_component_predicate(components) -> predicates.predicate:
    """
    Returns a Test API predicate that only accepts items from one of the given
    components (if no components are given, accept all items).

    :param components: A list of possible components to accept (empty if we
        should accept SysData from any component)
    :return: A predicate that checks if a SysData object come from one of the
        given components
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
        __str__).

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


def get_search_predicate(search_string: str) -> predicates.predicate:
    """
    Returns a Test API predicate that only accepts items whose string
    representation contains the exact given term.

    :param search_string: The substring to look for in the given object
    :return: A predicate that checks if an object contains "search_string" when
        it's converted to a string
    """
    if search_string:
        return contains_search_string(search_string)
    return predicates.always_true()


# TODO: Predicates currently act on Data objects; might make more sense to
# act on templates to be more generic?
def get_full_filter_predicate(
    ids, components, search_string: str
) -> predicates.predicate:
    """
    Returns a Test API predicate to only get recent data from the specified
    IDs/components, and containing the given search string. If any of these
    are left blank, no restrictions are assumed for that field.

    :param ids: A list of possible ID values to accept (empty if we should
        accept SysData with any ID)
    :param components: A list of possible components to accept (empty if we
        should accept SysData from any component)
    :param search_string: The substring to look for in the given object

    :return: A predicate that only accepts SysData objects with the given
        IDs, components, and search term
    """
    return_all = predicates.always_true()

    id_pred = get_id_predicate(ids)
    comp_pred = get_component_predicate(components)
    search_pred = get_search_predicate(search_string)

    return predicates.satisfies_all([return_all, id_pred, comp_pred, search_pred])


def initialize_test_api(
    app_dictionary_path: str,
    log_path: str = None,
    server_ip: str = "127.0.0.1",
    server_port: int = 50050,
) -> IntegrationTestAPI:
    """
    Initializes an Integration Test API instance for use.

    Note that these MUST be manually disconnected when you're done using them,
    by calling "pipeline.disconnect()" and "api.teardown()", respectively.

    :param app_dictionary_path: A string path from the current working directory
        to the "<project name>AppDictionary.xml" file for the project you're
        using the API with
    :param log_path: A string path to where files should be logged, or "None" if
        you only want console output
    :param server_ip: The IP of the GDS server you want to connect to
    :param server_port: The port for the Test API on the GDS server
    """
    if not app_dictionary_path:
        # TODO: Somehow search for this dynamically? Or is there no way around?
        local_dir = os.path.dirname(__file__)
        app_dictionary_path = os.path.join(
            local_dir, "../../../../../Ref/Top/RefTopologyAppDictionary.xml"
        )

    pipeline = StandardPipeline()
    pipeline.setup(ConfigManager(), app_dictionary_path, "/tmp")
    pipeline.connect(server_ip, server_port)

    # instantiate Test API (log_path of "None" will disable Test API logging)
    api = IntegrationTestAPI(pipeline, log_path)

    return (pipeline, api)


# TODO: Should separate frontend printing code from backend data-getting
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
    pipeline, api = initialize_test_api(
        dictionary_path, server_ip=ip_address, server_port=port
    )
    # ==========================================================================

    filter_predicate = get_full_filter_predicate(ids, components, search)
    event_objects = None

    # TODO: Follow currently just prints all new events as they come, with no
    # set printing frequency; update interface/API to reflect this?

    # Get objects at least once, and continue looping if follow is set and we're
    # not listing events
    min_start_time = "NOW"
    try:
        while True:
            if list:
                event_objects = get_events_list(pipeline.dictionaries, filter_predicate)
            else:
                event_objects = [
                    get_upcoming_event(api, filter_predicate, min_start_time)
                ]

            # TODO: Print returned objects properly, instead of leaving it up to Test API
            for event in event_objects:
                print(get_event_string(event, json))

            if not follow or list:
                break

            # Update time so we catch all events since the last one
            if event_objects and event_objects[0]:
                min_object = min(event_objects, key=lambda x: x.get_time())
                min_start_time = predicates.greater_than(min_object.get_time())
    except KeyboardInterrupt:
        # Break the loop and teardown the API/pipeline
        pass

    # ==========================================================================
    pipeline.disconnect()
    api.teardown()
    # ==========================================================================
