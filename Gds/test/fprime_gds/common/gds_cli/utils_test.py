"""
A suite of unit tests for testing utilities the GDS CLI uses
"""

import pytest

from fprime_gds.common.data_types.sys_data import SysData
import fprime_gds.common.gds_cli.filtering_utils as filtering_utils
import fprime_gds.common.gds_cli.misc_utils as misc_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
from fprime_gds.common.templates.data_template import DataTemplate
from fprime_gds.common.testing_fw import predicates

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


# ==============================================================================
# Test test_api_utils.get_item_list
# ==============================================================================


@pytest.fixture
def item_template_dictionary():
    template_12 = DataTemplate()
    template_12.id = 12
    template_12.name = "OneTwo"

    template_34 = DataTemplate()
    template_34.id = 34
    template_34.name = "ThreeFour"

    template_56 = DataTemplate()
    template_56.id = 56
    template_56.name = "FiveSix"

    template_78 = DataTemplate()
    template_78.id = 78
    template_78.name = "SevenEight"

    template_sur = DataTemplate()
    template_sur.id = 0
    template_sur.name = "SURPRISE!"

    return {
        "12": template_12,
        "56": template_56,
        "34": template_34,
        "78": template_78,
        "SURPRISE!": template_sur,
    }


def t2d(template):
    # TODO: Currently unable to actually instantiate SysData (bug in its init?)
    data = SysData()
    data.id = template.get_id()
    data.template = template
    return data


@pytest.mark.gds_cli
def test_valid_get_item_list(item_template_dictionary):
    item_list = test_api_utils.get_item_list(
        item_dictionary=item_template_dictionary,
        search_filter=predicates.always_true(),
        template_to_data=t2d,
    )

    assert item_list == [
        t2d(item_template_dictionary["SURPRISE!"]),
        t2d(item_template_dictionary["12"]),
        t2d(item_template_dictionary["34"]),
        t2d(item_template_dictionary["56"]),
        t2d(item_template_dictionary["78"]),
    ]
    # Check nothing changed in the original dictionary
    assert item_template_dictionary == item_template_dictionary()


@pytest.mark.gds_cli
def test_get_item_list_with_id_filter(item_template_dictionary):
    item_list = test_api_utils.get_item_list(
        item_dictionary=item_template_dictionary,
        search_filter=filtering_utils.get_full_filter_predicate([12, 34], [], None),
        template_to_data=t2d,
    )

    assert item_list == [
        t2d(item_template_dictionary["12"]),
        t2d(item_template_dictionary["34"]),
    ]
    # Check nothing changed in the original dictionary
    assert item_template_dictionary == item_template_dictionary()


@pytest.mark.gds_cli
def test_get_item_list_with_empty_item_dictionary():
    item_list = test_api_utils.get_item_list(
        item_dictionary={}, search_filter=predicates.always_true(), template_to_data=t2d
    )

    assert item_list == []
