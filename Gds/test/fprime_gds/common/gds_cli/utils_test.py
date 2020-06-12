"""
A suite of unit tests for testing utilities the GDS CLI uses
"""

import pytest

import fprime_gds.common.gds_cli.misc_utils as misc_utils

# ==============================================================================
# Test misc_utils.increment_with_interrupt
# ==============================================================================


@pytest.mark.gds_cli
def test_repeat_exits_at_counter():
    """
    Test basic functionality works as intended, can pass updated arguments, and
    exits on KeyboardInterrupt
    """
    count = 0

    def increment_count():
        nonlocal count
        count += 1

    def increment_with_interrupt(counter, exit_num, external_func):
        if counter == exit_num:
            raise KeyboardInterrupt
        external_func()
        return (counter + 1, exit_num, external_func)

    misc_utils.repeat_until_interrupt(increment_with_interrupt, 0, 3, increment_count)

    assert count == 3


@pytest.mark.gds_cli
def test_repeat_with_void_function_valid():
    """
    Test repeating with a function that doesn't return anything still works
    """
    count = 0

    def exit_at_value(exit_num):
        nonlocal count
        count += 1
        if count == exit_num:
            raise KeyboardInterrupt

    misc_utils.repeat_until_interrupt(exit_at_value, 5)

    assert count == 5


@pytest.mark.gds_cli
def test_repeat_with_no_arg_void_function_valid():
    """
    Test repeating with a function that takes no arguments doesn't throw an
    exception
    """
    count = 0

    def exit_when_3():
        nonlocal count
        count += 1
        if count == 3:
            raise KeyboardInterrupt

    misc_utils.repeat_until_interrupt(exit_when_3)

    assert count == 3


@pytest.mark.gds_cli
def test_repeat_with_no_arg_returning_function_invalid():
    """
    Test repeating with a function that takes no arguments but still returns a
    value is invalid
    """
    count = 0

    def error_before_exit():
        nonlocal count
        count += 1
        if count == 2:
            raise KeyboardInterrupt
        return "this should cause an error"

    with pytest.raises(TypeError):
        misc_utils.repeat_until_interrupt(error_before_exit)


@pytest.mark.gds_cli
def test_repeat_with_function_returning_naked_string_problematic():
    """
    Test that returning a string without encapsulating it in a tuple will fail,
    since every character will be counted as a different argument
    """
    count = 0

    def error_before_exit():
        nonlocal count
        count += 1
        if count == 2:
            raise KeyboardInterrupt
        return "this will return 49 arguments, because characters"

    with pytest.raises(TypeError):
        misc_utils.repeat_until_interrupt(error_before_exit)


@pytest.mark.gds_cli
def test_repeat_with_function_returning_enclosed_string_valid():
    """
    Test that returning a string when encapsulating it in a tuple will count
    it as just 1 argument
    """
    count = 0

    def error_before_exit(my_string):
        nonlocal count
        count += 1
        if count == 2:
            raise KeyboardInterrupt
        return ("this is only 1 argument, since it's in a tuple",)

    misc_utils.repeat_until_interrupt(error_before_exit, "let's do this")
