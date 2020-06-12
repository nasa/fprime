"""
TODO: Find more appropriate places for this.

This is the Isle of Misfit Functions used by the GDS CLI that I wasn't sure
where else to put. Please find a better home for them when you have time (i.e.
a more organized file).
"""

import types


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
