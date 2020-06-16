"""
A collection of base classes used for the backend implementations of the GDS
CLI commands
"""

import abc
import types
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
    """

    @classmethod
    @abc.abstractmethod
    def print_items_list(
        cls,
        project_dictionary: Dictionaries,
        filter_predicate: predicates.predicate,
        json: bool = False,
    ):
        """
        Gets a sorted list of items from the dictionary, filtered by the given
        predicate, and prints put their details.

        :param project_dictionary: The dictionary object for the project
            containing the item type definitions
        :param filter_predicate: Test API predicate used to filter shown items
        :param json: Whether to print out each item in JSON format or not
        """
        pass

    @classmethod
    @abc.abstractmethod
    def print_upcoming_item(
        cls,
        api: IntegrationTestAPI,
        filter_predicate: predicates.predicate,
        min_start_time="NOW",
        json: bool = False,
    ):
        """
        Retrieves an F' item that's occurred and prints out its data. Must
        support "misc_utils.repeat_until_interrupt" usage.
        """
        pass

    @classmethod
    def get_filter_predicate(
        cls, ids: Iterable[int], components: Iterable[str], search: str
    ) -> predicates.predicate:
        """
        Returns a predicate that will be used for filtering out which objects to
        search for.
        """
        return filtering_utils.get_full_filter_predicate(ids, components, search)

    # TODO: Just use args/kwargs instead of this massive argument list? But I
    # kind of do want some coupling with the frontend code to keep these in sync
    # TODO: Should separate frontend printing code from backend data-getting
    # (this is basically frontend code right now, which isn't necessarily bad)
    @classmethod
    def handle_arguments(
        cls,
        dictionary_path: str,
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
            dictionary_path, server_ip=ip_address, server_port=port
        )
        # ======================================================================

        filter_predicate = cls.get_filter_predicate(ids, components, search)

        # TODO: Try and refactor this to avoid nested if statements?
        if list:
            cls.print_items_list(pipeline.dictionaries, filter_predicate, json)
        else:
            if follow:
                misc_utils.repeat_until_interrupt(
                    cls.print_upcoming_item, api, filter_predicate, "NOW", json
                )
            else:
                cls.print_upcoming_item(api, filter_predicate, json=json)

        # TODO: Disable Test API from also logging to console somehow?

        # ======================================================================
        pipeline.disconnect()
        api.teardown()
        # ======================================================================
