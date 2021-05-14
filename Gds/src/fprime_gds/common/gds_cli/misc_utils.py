"""
TODO: Find more appropriate places for these.

This is the Isle of Misfit Functions used by the GDS CLI, ones that I wasn't
sure where else to put. Please find a better home for them when you have time
(i.e. a more organized file).
"""

import json
from typing import Callable, Iterable, NamedTuple

from fprime_gds.common.data_types.sys_data import SysData
from fprime_gds.common.templates.cmd_template import CmdTemplate

# NOTE: 'GDSJsonEncoder' module is now lazy-loaded below for performance
# reasons


# Define some tuples containing sets of arguments used by GDS CLI commands;
# For more details on these arguments, see argument definitions at:
#           Gds/src/fprime_gds/executables/fprime_cli.py
ConnectionInfo = NamedTuple(
    "ConnectionInfo", [("dictionary", str), ("ip_address", str), ("port", int)]
)
SearchInfo = NamedTuple(
    "SearchInfo",
    [
        ("is_printing_list", bool),
        ("ids", Iterable[int]),
        ("components", Iterable[str]),
        ("search", str),
        ("json", bool),
    ],
)


def repeat_until_interrupt(func: Callable, *args):
    """
    Continues to call the input function with the given arguments until the
    user interrupts it.

    :param func: The function you want to call repeatedly. If the function
        returns anything, it MUST return a new, updated tuple of the arguments
        passed into it in the same order, which will be used as the new
        arguments in the next iteration. This is done to allow for persistent
        state between iterations; if needed, create a wrapper for your original
        function to do this. If the function does NOT return anything, the
        original input arguments will continue to be used
    :param args: All keyword arguments you want to pass into "func"
    """
    try:
        while True:
            new_args = func(*args)  # lgtm [py/call/wrong-arguments]
            if new_args:
                args = new_args
    except KeyboardInterrupt:
        pass


def get_item_json_string(gds_item, tab_spaces: int = 2) -> str:
    """
    Converts the given F Prime GDS data item into a JSON-formatted string.

    :param gds_item: The item to convert to JSON
    :param tab_spaces: The (number of spaces to use for indented lines
    :return: A string of the gds_item in JSON format
    """
    from fprime_gds.flask.json import GDSJsonEncoder

    return json.dumps(gds_item, indent=tab_spaces, cls=GDSJsonEncoder)


def get_item_string(item: SysData, as_json: bool = False) -> str:
    """
    Takes in the given SysData and prints out a human-readable string
    representation of its information.

    :param item: The item to create a string for
    :param as_json: Return a JSON-string representation of the given item
        instead
    :return: A readable string of the item information
    """
    if not item:
        return ""

    if as_json:
        return get_item_json_string(item)
    # NOTE: "get_str" isn't on the base sys_data class, but is on all the query
    # items we care about so far (i.e. EventData, ChannelData, CommandData)
    return item.get_str(verbose=True)


def get_cmd_template_string(
    item: CmdTemplate,
    as_json: bool = False,
) -> str:
    """
    Converts the given command template into a human-readable string.

    :param item: The CmdTemplate to convert to a string
    :param as_json: Whether or not to return a JSON representation of "temp"
    :return: A readable string version of "item"
    """
    if not item:
        return get_item_string(item)
    if as_json:
        return get_item_json_string(item)

    cmd_string = "%s (%d) | Takes %d arguments.\n" % (
        item.get_full_name(),
        item.get_id(),
        len(item.get_args()),
    )

    cmd_description = item.get_description()
    if cmd_description:
        cmd_string += "Description: %s\n" % (cmd_description)

    for arg in item.get_args():
        arg_name, arg_description, arg_type = arg
        if not arg_description:
            arg_description = "--no description--"
        # Can't compare against actual module, since EnumType is a serializable
        # type from the dictionary
        if type(arg_type).__name__ == "EnumType":
            arg_description = "%s | %s " % (str(arg_type.keys()), arg_description)

        cmd_string += "\t{} ({}): {}\n".format(
            arg_name,
            type(arg_type).__name__,
            arg_description,
        )

    return cmd_string
