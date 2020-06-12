"""
A set of PyTest-style unit tests for parsing commands
"""

import pytest

import fprime_gds.executables.fprime_cli as fprime_cli
import fprime_gds.common.gds_cli.events as events


@pytest.fixture
def standard_parser():
    return fprime_cli.create_parser()

def default_valid_args_dict(updated_values_dict={}):
    """
    The output arguments dictionary for a valid channels/commands/events
    command without any arguments provided to it; can pass in a dictionary of
    key/value pairs to update this dictionary as necessary for a given test
    """
    dictionary = {
        "func": fprime_cli.PLACEHOLDER_FUNC,
        "dictionary_path": None,
        "ip_address": "127.0.0.1",
        "port": 50050,
        "list": False,
        "follow": False,
        "ids": None,
        "components": None,
        "search": None,
        "json": False,
    }
    dictionary.update(updated_values_dict)
    return dictionary


# A set of valid CLI inputs and the output we expect from them
@pytest.mark.parametrize(
    "input_cli_arguments, expected_args_dict",
    [
        ("", {"func": None}),
        ([], {"func": None}),
        # TODO: Unsure how to test help/version args, since they exit the program?
        # (["-h"], {"func": None}),
        (["channels"], default_valid_args_dict(),),
        (["commands"], default_valid_args_dict(),),
        (
            ["command-send", "some.command.name"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "command_name": "some.command.name",
                "list": False,
                "key": None,
                "arguments": None,
                "dictionary_path": None,
                "ip_address": "127.0.0.1",
                "port": 50050,
            },
        ),
        (["events"], default_valid_args_dict({"func": events.get_events_output}),),
        (["channels", "-l"], default_valid_args_dict({"list": True}),),
        (["channels", "--list"], default_valid_args_dict({"list": True}),),
        (["channels", "-d", "follow/the/yellow/brick/road"], default_valid_args_dict({"dictionary_path": "follow/the/yellow/brick/road"}),),
        (["channels", "-f"], default_valid_args_dict({"follow": True}),),
        (["commands", "-i", "10"], default_valid_args_dict({"ids": [10]}),),
        (
            ["commands", "-i", "3", "4", "8"],
            default_valid_args_dict({"ids": [3, 4, 8]}),
        ),
        (
            [
                "events",
                "-c",
                "Grover's Corners",
                "Sutton County",
                "New Hampshire",
                "United States of America",
                "North America",
                "Western Hemisphere",
                "Earth",
                "The Solar System",
                "The Universe",
            ],
            default_valid_args_dict(
                {
                    "func": events.get_events_output,
                    "components": [
                        "Grover's Corners",
                        "Sutton County",
                        "New Hampshire",
                        "United States of America",
                        "North America",
                        "Western Hemisphere",
                        "Earth",
                        "The Solar System",
                        "The Universe",
                    ],
                }
            ),
        ),
        (
            ["events", "-S", "per.aspera.ad.astra"],
            default_valid_args_dict(
                {"func": events.get_events_output, "search": "per.aspera.ad.astra"}
            ),
        ),
        (
            ["events", "-ip", "jpl.nasa.gov"],
            default_valid_args_dict(
                {"func": events.get_events_output, "ip_address": "jpl.nasa.gov"}
            ),
        ),
        (
            ["events", "-j"],
            default_valid_args_dict({"func": events.get_events_output, "json": True}),
        ),
    ],
)
@pytest.mark.gds_cli
def test_output_after_parsing_valid_input(
    standard_parser, input_cli_arguments, expected_args_dict
):
    args = fprime_cli.parse_args(standard_parser, input_cli_arguments)

    args_dict = vars(args)
    assert args_dict == expected_args_dict


@pytest.mark.gds_cli
def test_command_send_no_command_given_error(standard_parser):
    # Not giving a command option here should raise a "SystemExit" exception
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["command-send"])


@pytest.mark.gds_cli
def test_components_no_search_term_given_error(standard_parser):
    # Not giving any string after giving the option should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["commands", "-S"])


@pytest.mark.gds_cli
def test_id_flag_no_ids_given_error(standard_parser):
    # Not giving any IDs after giving the option should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["channels", "-i"])


@pytest.mark.gds_cli
def test_id_flag_float_id_given_error(standard_parser):
    # Giving a non-integer ID here should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["events", "-i", "34.8"])


@pytest.mark.gds_cli
def test_id_flag_string_id_given_error(standard_parser):
    # Giving a non-integer ID here should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["channels", "-i", "crashtime"])


@pytest.mark.gds_cli
def test_components_no_components_given_error(standard_parser):
    # Not giving any components after giving the option should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["events", "-c"])
