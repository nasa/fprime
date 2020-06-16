"""
TODO: Find more appropriate places for these.

This is the Isle of Misfit Functions used by the GDS CLI, ones that I wasn't
sure where else to put. Please find a better home for them when you have time
(i.e. a more organized file).
"""

import json
import types

from fprime_gds.common.data_types.sys_data import SysData
from fprime_gds.flask.json import GDSJsonEncoder


def repeat_until_interrupt(func: types.FunctionType, *args):
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
            new_args = func(*args)
            if new_args:
                args = new_args
    except KeyboardInterrupt:
        pass


# TODO: Need to do user tests to find a better print format
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
        return "No matching item found"

    if as_json:
        return json.dumps(item, indent=2, cls=GDSJsonEncoder)
    # TODO: "get_str" isn't on the base sys_data class, but is on all the query
    # items we care about so far (i.e. EventData, ChannelData, CommandData)
    return item.get_str(verbose=True)
