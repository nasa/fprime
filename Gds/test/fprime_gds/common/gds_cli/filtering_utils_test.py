"""
A suite of unit tests for testing the predicates and filtering utilities the
GDS CLI uses
"""

import pytest

from fprime.common.models.serialize import time_type
from fprime_gds.common.data_types.event_data import EventData
import fprime_gds.common.gds_cli.filtering_utils as filtering_utils
import fprime_gds.common.gds_cli.test_api_utils as test_api_utils
from fprime_gds.common.templates.event_template import EventTemplate
from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity


@pytest.fixture
def sample_template():
    # TODO: Possibly add an argument for testing's sake?
    return EventTemplate(
        event_id=12345,
        name="Luggage_Combination",
        component="helmet",
        args=[],
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


# TODO: Write tests for passing in templates as well?
# TODO: Also write tests for generic SysData, and for non-event objects?


@pytest.mark.gds_cli
def test_string_filter_valid_function_passing(sample_event):
    def get_event_component(eventData) -> str:
        return eventData.get_template().get_comp_name()

    default_filter = filtering_utils.contains_search_string("helmet")
    function_filter = filtering_utils.contains_search_string(
        "helmet", get_event_component
    )
    assert not default_filter(sample_event)
    assert function_filter(sample_event)


@pytest.mark.gds_cli
def test_string_filter_case_sensitive(sample_event):
    upper_filter = filtering_utils.contains_search_string("Luggage_Combination")
    lower_filter = filtering_utils.contains_search_string("luggage_combination")
    assert upper_filter(sample_event)
    assert not lower_filter(sample_event)
