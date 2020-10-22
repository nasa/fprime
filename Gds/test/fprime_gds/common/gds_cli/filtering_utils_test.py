"""
A suite of unit tests for testing the predicates and filtering utilities the
GDS CLI uses
"""

import pytest

import fprime_gds.common.gds_cli.filtering_utils as filtering_utils
from fprime.common.models.serialize import time_type
from fprime_gds.common.data_types.event_data import EventData
from fprime_gds.common.templates.event_template import EventTemplate
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity

# Pytest fixtures work by reusing outer names, so disable this warning
# pylint: disable=redefined-outer-name
@pytest.fixture
def sample_template():
    return EventTemplate(
        event_id=12345,
        name="Luggage_Combination",
        component="helmet",
        args=[],  # Currently, no arguments given for simplicity's sake
        severity=EventSeverity.DIAGNOSTIC,
        format_str="I've got the same combination on mine!",
    )


@pytest.fixture
def sample_event(sample_template):
    event = EventData([], time_type.TimeType(), sample_template)
    return event


# A set of valid input filters that should accept the sample event
@pytest.mark.parametrize(
    "accepting_input_filter",
    [
        filtering_utils.id_predicate(12345),
        filtering_utils.get_id_predicate([]),
        filtering_utils.get_id_predicate([12345]),
        filtering_utils.get_id_predicate([451, 12345, 54321, 8]),
        filtering_utils.component_predicate("helmet"),
        filtering_utils.get_component_predicate([]),
        filtering_utils.get_component_predicate(["helmet"]),
        filtering_utils.get_component_predicate(["dark", "helmet", "luggage"]),
        filtering_utils.contains_search_string(""),
        # SysData string should contain ID, hex opcode, severity; only contains
        # the format string if args isn't None
        # Won't test this too thoroughly, since it's more based on SysData than
        # this implementation
        filtering_utils.contains_search_string("the same combination"),
        filtering_utils.get_search_predicate(""),
        filtering_utils.get_search_predicate("the same combination"),
    ],
)
@pytest.mark.gds_cli
def test_valid_accepting_filter(sample_event, accepting_input_filter):
    assert accepting_input_filter(sample_event)


@pytest.mark.parametrize(
    "rejecting_input_filter",
    [
        filtering_utils.id_predicate(54321),
        filtering_utils.get_id_predicate([123, 451, 54321, 12344]),
        filtering_utils.component_predicate("luggage"),
        filtering_utils.get_component_predicate(["dark", "luggage"]),
        filtering_utils.contains_search_string("won't be found"),
        filtering_utils.get_search_predicate("won't be found"),
    ],
)
@pytest.mark.gds_cli
def test_valid_rejecting_filter(sample_event, rejecting_input_filter):
    assert not rejecting_input_filter(sample_event)


@pytest.mark.gds_cli
def test_event_string_has_component(sample_event):
    component_filter = filtering_utils.contains_search_string("helmet")
    assert component_filter(sample_event)


@pytest.mark.gds_cli
def test_string_filter_valid_function_passing(sample_event):
    crazy_string = "tHaT'sAmAzInG..."

    def get_crazy_string(eventData) -> str:
        return crazy_string + str(eventData)

    default_filter = filtering_utils.contains_search_string(crazy_string)
    function_filter = filtering_utils.contains_search_string(
        crazy_string, get_crazy_string
    )
    assert not default_filter(sample_event)
    assert function_filter(sample_event)


@pytest.mark.gds_cli
def test_string_filter_case_sensitive(sample_event):
    upper_filter = filtering_utils.contains_search_string("Luggage_Combination")
    lower_filter = filtering_utils.contains_search_string("luggage_combination")
    assert upper_filter(sample_event)
    assert not lower_filter(sample_event)


@pytest.mark.gds_cli
def test_search_predicate_case_sensitive(sample_event):
    upper_filter = filtering_utils.get_search_predicate("Luggage_Combination")
    lower_filter = filtering_utils.get_search_predicate("luggage_combination")
    assert upper_filter(sample_event)
    assert not lower_filter(sample_event)


@pytest.mark.gds_cli
def test_time_greater_than_predicate():
    time1 = time_type.TimeType(seconds=1593610856, useconds=223451)
    time2 = time_type.TimeType(seconds=1593610856, useconds=223502)

    time_pred = predicates.greater_than(time1)
    assert not time_pred(time1)
    assert time_pred(time2)


@pytest.mark.gds_cli
def test_comparing_item_to_time_fails(sample_event):
    """
    Test that comparing a time to an event is problematic, since this assertion
    SHOULD always be true but is, in fact, false because we're comparing a data
    object to a time object
    """
    time = time_type.TimeType(seconds=12345)
    sample_event.time = time_type.TimeType(seconds=9999999)

    time_pred = predicates.greater_than(time)
    # This SHOULD return true, but it doesn't
    assert not time_pred(sample_event)


@pytest.mark.gds_cli
def test_comparing_item_to_converted_time_succeeds(sample_event):
    time = time_type.TimeType(seconds=12345)
    sample_event.time = time_type.TimeType(seconds=9999999)

    time_pred = predicates.greater_than(time)
    converted_time_pred = filtering_utils.time_to_data_predicate(time_pred)
    # Now, this returns the correct value
    assert converted_time_pred(sample_event)
