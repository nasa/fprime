"""
A set of utility classes and functions for filtering the items we want to return
to the user in the GDS CLI
"""

from typing import Any, Callable, Iterable

from fprime_gds.common.data_types.cmd_data import CmdData
from fprime_gds.common.data_types.sys_data import SysData
from fprime_gds.common.testing_fw import predicates


class id_predicate(predicates.predicate):
    def __init__(self, id_num: int):
        """
        A predicate that tests if the SysData or DataTemplate argument given to
        it is of a given ID type.

        :param id_num: The ID to compare the item against
        """
        self.id_num = id_num

    def __call__(self, item):
        """

        :param item: The object or value to evaluate
        """
        return hasattr(item, "get_id") and self.id_num == item.get_id()

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x.id == {}".format(self.id_num)


def get_id_predicate(ids: Iterable[int]) -> predicates.predicate:
    """
    Returns a Test API predicate that only accepts items with one of the given
    type IDs (if no IDs are given, accept all items).

    :param ids: A list of possible ID values to accept (empty if we should
        accept SysData with any ID)
    :return: A predicate that checks if a SysData object has one of the given
        IDs
    """
    if ids:
        id_preds = [id_predicate(id_num) for id_num in ids]
        return predicates.satisfies_any(id_preds)
    return predicates.always_true()


class component_predicate(predicates.predicate):
    def __init__(self, component: str):
        """
        A predicate that tests if the SysData or DataTemplate argument given is
        from the given component. If there is no component information found,
        returns True.

        :param component: The component name to check for
        """
        self.comp = component

    def __call__(self, item):
        """

        :param item: the object or value to evaluate
        """
        # NOTE: Always returns true if no component found
        item_component = self.comp
        if hasattr(item, "get_comp_name"):
            item_component = item.get_comp_name()
        elif hasattr(item.get_template(), "get_comp_name"):
            # Technically law of Minerva violation (acceptable?)
            item_component = item.get_template().get_comp_name()
        return self.comp == item_component

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return 'x is in component "{}"'.format(self.comp)


def get_component_predicate(components: Iterable[str]) -> predicates.predicate:
    """
    Returns a Test API predicate that only accepts items from one of the given
    components (if no components are given, accept all items).

    :param components: A list of possible components to accept (empty if we
        should accept SysData from any component)
    :return: A predicate that checks if a SysData object come from one of the
        given components
    """
    if components:
        component_preds = [component_predicate(comp) for comp in components]
        return predicates.satisfies_any(component_preds)
    return predicates.always_true()


class contains_search_string(predicates.predicate):
    def __init__(self, search_string: str, to_string_func: Callable[[Any], str] = str):
        """
        A predicate that tests if the argument given to it contains the
        passed-in string (after the argument is converted to a string via
        __str__).

        :param search_string: The exact text to check for inside the object
        :param to_string_func: An optional method for converting the given
            object to a string
        """
        self.search_string = str(search_string)
        self.to_str = to_string_func

    def __call__(self, item):
        """

        :param item: the object or value to evaluate
        """
        return self.search_string in self.to_str(item)

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return 'str(x) contains "{}"'.format(self.search_string)


def get_search_predicate(
    search_string: str, to_str: Callable[[Any], str] = str
) -> predicates.predicate:
    """
    Returns a Test API predicate that only accepts items whose string
    representation contains the exact given term.

    :param search_string: The substring to look for in the given object
    :param to_str: An optional function for converting a given object to a
        string
    :return: A predicate that checks if an object contains "search_string" when
        it's converted to a string
    """
    if search_string:
        return contains_search_string(search_string, to_str)
    return predicates.always_true()


def get_full_filter_predicate(
    ids: Iterable[int],
    components: Iterable[str],
    search_string: str,
    to_str: Callable[[Any], str] = str,
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
    :param to_str: An optional function for converting a given object to a
        string

    :return: A predicate that only accepts SysData objects with the given
        IDs, components, and search term
    """
    return_all = predicates.always_true()

    id_pred = get_id_predicate(ids)
    comp_pred = get_component_predicate(components)
    search_pred = get_search_predicate(search_string, to_str)

    return predicates.satisfies_all([return_all, id_pred, comp_pred, search_pred])


class cmd_predicate(predicates.predicate):
    def __init__(self):
        """
        A predicate for specifying a CmdData object from data_types.cmd_data.
        This predicate can be used to search a history.
        """

    def __call__(self, cmd):
        """
        The cmd_predicate checks that the object is an instance of CmdData.

        :param cmd: an object to check for being a CmdData object
        """
        return isinstance(cmd, CmdData)

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "True IFF: x is a CmdData object"


class time_to_data_predicate(predicates.predicate):
    def __init__(self, time_predicate: predicates.predicate):
        """
        Converts the given predicate from one called on an F' TimeType to one
        that can be called on a SysData type
        :param time_predicate: a predicate that expects a TimeType object to be
            passed in
        """
        self.time_pred = time_predicate

    def __call__(self, item: SysData):
        """
        Gets the given item's time and checks it using the passed-in predicate

        :param item: an object to check for being a CmdData object
        """
        return self.time_pred(item.get_time())

    def __str__(self):
        """
        Returns a string outlining the evaluation done by the predicate.
        """
        return "x = x.get_time(), {}".format(self.time_pred)
