"""
A collection of base classes used for the backend implementations of the GDS
CLI commands
"""

import abc
from typing import Iterable
import sys

import fprime_gds.common.gds_cli.filtering_utils as filtering_utils
import fprime_gds.common.gds_cli.misc_utils as misc_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
from fprime_gds.common.pipeline.dictionaries import Dictionaries
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.testing_fw.api import IntegrationTestAPI


class BaseCommand(abc.ABC):
    """
    The base class for implementing a GDS CLI command's functionality, once
    arguments are parsed and passed into this command.
    """

    @classmethod
    def _log(cls, log_text: str):
        """
        Takes the given string and logs it (by default, logs all output to the
        console). Will ignore empty strings.

        :param log_text: The string to print out
        """
        if log_text:
            print(log_text)
            sys.stdout.flush()

    @classmethod
    @abc.abstractmethod
    def handle_arguments(cls, *args, **kwargs):
        """
        Do something to handle the input arguments given
        """


class QueryHistoryCommand(BaseCommand):
    """
    The base class for a set of related GDS CLI commands that need to query and
    display received data from telemetry channels, F' events, command histories,
    or similar functionalities with closely-related interfaces.

    TODO: Should separate frontend printing code from backend data-getting
    (this is basically frontend code right now, which isn't necessarily bad)
    """

    @classmethod
    @abc.abstractmethod
    def _get_item_list(
        cls, project_dictionary: Dictionaries, filter_predicate: predicates.predicate,
    ) -> Iterable:
        """
        Gets a sorted list of items from the dictionary, filtered by the given
        predicate, and return it.

        :param project_dictionary: The dictionary object for the project
            containing the item type definitions
        :param filter_predicate: Test API predicate used to filter shown items
        :return: An iterable collection of items that passed the filter
        """

    @classmethod
    def _get_item_list_string(cls, items: Iterable, json: bool = False,) -> str:
        """
        Takes a list of items from the dictionary and returns a human-readable
        string containing their details.

        :param items: An iterable collection of F' data objects
        :param json: Whether to print out each item in JSON format or not
        :return: A string containing the details of each item
        """
        return "\n".join(cls._get_item_string(item, json) for item in items)

    @classmethod
    @abc.abstractmethod
    def _get_upcoming_item(
        cls,
        api: IntegrationTestAPI,
        filter_predicate: predicates.predicate,
        min_start_time="NOW",
        timeout: float = 5.0,
    ):
        """
        Retrieves an F' item that's occurred since the given time and returns
        its data.
        """

    @classmethod
    def _get_item_string(cls, item, json: bool = False,) -> str:
        """
        Takes an F' item and returns a human-readable string of its data.

        :param item: The F' item to convert to a string
        :param json: Whether to print out each item in JSON format or not
        :return: A string representation of "item"
        """
        return misc_utils.get_item_string(item, json)

    @classmethod
    def _get_search_filter(
        cls,
        ids: Iterable[int],
        components: Iterable[str],
        search: str,
        json: bool = False,
    ) -> predicates.predicate:
        """
        Returns a predicate that can be used to filter any received messages.
        This is done to link printing code to filtering, so that filtering ALWAYS filters the same strings as are printed.

        TODO: Possibly make separate filters for item/item_list strings, since
        there's currently no guarantee they'll be the same if
        _get_item_list_string is overridden?

        :param item: The F' item to convert to a string
        :param json: Whether to convert each item to a JSON representation
            before filtering them
        :return: A string representation of "item"
        """
        item_to_string = lambda x: cls._get_item_string(x, json)
        return filtering_utils.get_full_filter_predicate(
            ids, components, search, to_str=item_to_string
        )

    @classmethod
    def _list_all_possible_items(
        cls, dictionary_path: str, search_filter: predicates.predicate, json: bool
    ) -> str:
        """
        Returns a string of the the information for all relevant possible items
        for this command, filtered using the given predicate.

        :param dictionary_path: The string path to the dictionary file we should
            look for possible items in
        :param search_filter: The predicate to filter the items with
        :param json: Whether to print out each item in JSON format or not
        :return: A string describing each item relevant to this command that
            passes the given filter
        """
        project_dictionary = Dictionaries()
        project_dictionary.load_dictionaries(dictionary_path, packet_spec=None)
        items = cls._get_item_list(project_dictionary, search_filter)
        return cls._get_item_list_string(items, json)

    # Explicit rgument list instead of args/kwargs used to have some coupling
    # with the frontend code to keep these in sync
    @classmethod
    def handle_arguments(
        cls,
        dictionary: str,
        ip_address: str,
        port: int,
        list: bool,
        timeout: float,
        ids: Iterable[int],
        components: Iterable[str],
        search: str,
        json: bool,
        *args,
        **kwargs
    ):
        """
        Takes in the given arguments and uses them to print out a formatted
        string of items the user wants to see from the Test API; if the option
        is present, repeat doing this until the user exits the program.

        For descriptions of these arguments, and more function details, see:
            Gds/src/fprime_gds/executables/fprime_cli.py
        """
        search_filter = cls._get_search_filter(ids, components, search, json)

        if list:
            cls._log(cls._list_all_possible_items(dictionary, search_filter, json))
            return

        # ======================================================================
        # Set up Test API
        pipeline, api = test_api_utils.initialize_test_api(
            dictionary, server_ip=ip_address, server_port=port
        )
        # ======================================================================

        if timeout:
            item = cls._get_upcoming_item(api, search_filter, "NOW", timeout)
            cls._log(cls._get_item_string(item, json))
        else:

            def print_upcoming_item(min_start_time="NOW"):
                item = cls._get_upcoming_item(api, search_filter, min_start_time)
                cls._log(cls._get_item_string(item, json))
                # Update time so we catch the next item since the last one
                if item:
                    min_start_time = predicates.greater_than(item.get_time())
                    min_start_time = filtering_utils.time_to_data_predicate(
                        min_start_time
                    )
                return (min_start_time,)

            misc_utils.repeat_until_interrupt(print_upcoming_item, "NOW")

        # ======================================================================
        # Tear down Test API
        pipeline.disconnect()
        api.teardown()
        # ======================================================================
