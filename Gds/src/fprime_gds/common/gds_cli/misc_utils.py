"""
TODO: Find more appropriate places for these.

This is the Isle of Misfit Functions used by the GDS CLI that I wasn't sure
where else to put. Please find a better home for them when you have time (i.e.
a more organized file).
"""

from collections import namedtuple
import types

from fprime_gds.common.data_types.sys_data import SysData
import fprime_gds.common.gds_cli.filtering_utils as filtering_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils


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


QueryCommandArgs = namedtuple(
    "QueryCommandArgs",
    "dictionary_path ip_address port list follow ids components search json",
)


def query_command_template(
    print_item_list_func: types.FunctionType,
    print_upcoming_item_func: types.FunctionType,
    args: QueryCommandArgs,
):
    """
    Takes in the given arguments and uses them to print out a formatted string
    of items the user wants to see from the Test API; if the option is present,
    repeat doing this until the user exits the program.

    :param print_item_list_func: A function that, given an F' dictionary, a
        Test API predicate, and some print options, can get a sorted list of
        items from the dictionary and print them
    :param print_upcoming_item_func: A function that given a Test API instance,
        a Test API predicate, and printing options, will retrieve an item and
        print it. Must support "misc_utils.repeat_until_interrupt" usage
    :param args: The command-line arguments passed in by the user. For
        descriptions of these arguments, and more function details, see:
        Gds/src/fprime_gds/executables/fprime_cli.py
    """
    # ==========================================================================
    pipeline, api = test_api_utils.initialize_test_api(
        args.dictionary_path, server_ip=args.ip_address, server_port=args.port
    )
    # ==========================================================================

    filter_predicate = filtering_utils.get_full_filter_predicate(
        args.ids, args.components, args.search
    )

    # TODO: Try and refactor this to avoid nested if statements?
    if args.list:
        print_item_list_func(pipeline.dictionaries, filter_predicate, args.json)
    else:
        if args.follow:
            repeat_until_interrupt(
                print_upcoming_item_func, api, filter_predicate, "NOW", args.json
            )
        else:
            print_upcoming_item_func(api, filter_predicate, json=args.json)

    # TODO: Disable Test API from also logging to console somehow?

    # ==========================================================================
    pipeline.disconnect()
    api.teardown()
    # ==========================================================================


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
        # TODO: "json.dumps" doesn't seem to work correctly yet?
        pass
    # TODO: "get_str" isn't on the base sys_data class, but is on all the query
    # items we care about so far (i.e. EventData, ChannelData, CommandData)
    return item.get_str(verbose=True)
