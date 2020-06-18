"""
A collection of base classes used for the backend implementations of the GDS
CLI commands
"""

import abc
from typing import Iterable

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
    @abc.abstractmethod
    def handle_arguments(cls, *args, **kwargs):
        """
        Do something to handle the input arguments given
        """
        pass


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
    def _get_items_list(
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
        pass

    @classmethod
    def _get_items_list_string(cls, items: Iterable, json: bool = False,) -> str:
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
    ):
        """
        Retrieves an F' item that's occurred since the given time and returns
        its data.
        """
        pass

    # TODO: Need to do user tests to find a better print format
    @classmethod
    def _get_item_string(cls, item, json: bool = False,) -> str:
        """
        Takes an F' item and returns a human-readable string of its data.

        :param item: The F' item to convert to a string
        :param json: Whether to print out each item in JSON format or not
        :return: A string representation of "item"
        """
        return misc_utils.get_item_string(item, json)

    # TODO: Just use args/kwargs instead of this massive argument list? But I
    # kind of do want some coupling with the frontend code to keep these in sync
    @classmethod
    def handle_arguments(
        cls,
        dictionary: str,
        ip_address: str,
        port: int,
        list: bool,
        follow: bool,
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
        # ======================================================================
        pipeline, api = test_api_utils.initialize_test_api(
            dictionary, server_ip=ip_address, server_port=port
        )
        # ======================================================================

        # Link printing code to filtering, so that searching ALWAYS searches
        # the same strings as are printed
        # TODO: Possibly make separate filters for item/item_list strings?
        item_to_string = lambda x: cls._get_item_string(x, json)
        search_filter = filtering_utils.get_full_filter_predicate(
            ids, components, search, to_str=item_to_string
        )

        # TODO: If combinatorial explosion w/ options becomes an issue,
        # refactor this to avoid if/else structure?
        if list:
            items = cls._get_items_list(pipeline.dictionaries, search_filter)
            print(cls._get_items_list_string(items, json))
        elif follow:

            def print_upcoming_item(min_start_time="NOW"):
                item = cls._get_upcoming_item(api, search_filter, min_start_time)
                print(item_to_string(item))
                # Update time so we catch the next item since the last one
                if item:
                    min_start_time = predicates.greater_than(item.get_time())
                return (min_start_time,)

            misc_utils.repeat_until_interrupt(print_upcoming_item, "NOW")
        else:
            item = cls._get_upcoming_item(api, search_filter, "NOW")
            print(item_to_string(item))

        # ======================================================================
        pipeline.disconnect()
        api.teardown()
        # ======================================================================
