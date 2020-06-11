"""
A file containing utilities for interacting with the Integration Test API
"""

import os

from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.pipeline.standard import StandardPipeline
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.testing_fw.api import IntegrationTestAPI
from fprime_gds.common.utils.config_manager import ConfigManager


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


# TODO: Possibly convert to function that can take arbitrary predicates for awaiting data?
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
