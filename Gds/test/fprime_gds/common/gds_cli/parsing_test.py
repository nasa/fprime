"""
A set of PyTest-style unit tests for parsing commands
"""

import os

import pytest

import fprime_gds.executables.fprime_cli as fprime_cli


@pytest.fixture
def standard_parser():
    return fprime_cli.create_parser()


def get_path_relative_to_file(path: str) -> str:
    """
    Converts the given path to one relative to this file.
    """
    dirname = os.path.dirname(__file__)
    return os.path.realpath(os.path.join(dirname, path))


def default_project_dictionary():
    """
    The project dictionary to use for normal tests
    """
    return get_path_relative_to_file("../testing_fw/UnitTestDictionary.xml")


def default_valid_args_dict(updated_values_dict={}):
    """
    The output arguments dictionary for a valid channels/commands/events
    command without any arguments provided to it; can pass in a dictionary of
    key/value pairs to update this dictionary as necessary for a given test
    """
    dictionary = {
        "func": None,
        "dictionary": default_project_dictionary(),
        "ip_address": "127.0.0.1",
        "port": 50050,
        "list": False,
        "timeout": 0.0,
        "ids": None,
        "components": None,
        "search": None,
        "json": False,
    }
    dictionary.update(updated_values_dict)
    return dictionary


def default_valid_channels_dict(updated_values_dict={}):
    dictionary = default_valid_args_dict(
        {"func": fprime_cli.ChannelsParser.command_func}
    )
    dictionary.update(updated_values_dict)
    return dictionary


def default_valid_command_send_dict(updated_values_dict={}):
    dictionary = default_valid_args_dict(
        {
            "func": fprime_cli.CommandSendParser.command_func,
            "arguments": [],
            "command_name": None,
        }
    )
    dictionary.update(updated_values_dict)
    del dictionary["timeout"]
    return dictionary


def default_valid_events_dict(updated_values_dict={}):
    dictionary = default_valid_args_dict({"func": fprime_cli.EventsParser.command_func})
    dictionary.update(updated_values_dict)
    return dictionary


# A set of valid CLI inputs and the output we expect from them
@pytest.mark.parametrize(
    "input_cli_arguments, expected_args_dict",
    [
        (["channels"], default_valid_channels_dict(),),
        (
            ["command-send", "some.command.name"],
            default_valid_command_send_dict({"command_name": "some.command.name"}),
        ),
        (
            # TODO: This test is currently failing because an invalid dictionary throws an exception; figure out way to test -d flag?
            ["events", "-d", "someDictionary.xml"],
            default_valid_events_dict(
                {"dictionary": get_path_relative_to_file("someDictionary.xml")}
            ),
        ),
        (["channels", "-l"], default_valid_channels_dict({"list": True}),),
        (["command-send", "--list"], default_valid_command_send_dict({"list": True}),),
        (["channels", "-t", "4"], default_valid_channels_dict({"timeout": 4.0}),),
        (["events", "-i", "10"], default_valid_events_dict({"ids": [10]}),),
        (
            ["events", "-i", "3", "4", "8"],
            default_valid_events_dict({"ids": [3, 4, 8]}),
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
            default_valid_events_dict(
                {
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
            ["events", "-s", "per.aspera.ad.astra"],
            default_valid_events_dict({"search": "per.aspera.ad.astra"}),
        ),
        (
            ["events", "-ip", "jpl.nasa.gov"],
            default_valid_events_dict({"ip_address": "jpl.nasa.gov"}),
        ),
        (["events", "-j"], default_valid_events_dict({"json": True}),),
    ],
)
@pytest.mark.gds_cli
def test_output_after_parsing_valid_input(
    standard_parser, input_cli_arguments, expected_args_dict
):
    args = fprime_cli.parse_args(standard_parser, input_cli_arguments)

    args_dict = vars(args)
    assert args_dict == expected_args_dict


# TODO: Unsure how to test help/version args, since they exit the program?
@pytest.mark.gds_cli
def test_empty_input_exits_programs(standard_parser):
    # Not giving a command option here should raise a "SystemExit" exception
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, [""])


@pytest.mark.gds_cli
def test_no_input_exits_programs(standard_parser):
    # Not giving a command option here should raise a "SystemExit" exception
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, [])


@pytest.mark.gds_cli
def test_command_send_no_command_or_list_given_error(standard_parser):
    # Not giving a command option here should raise a "SystemExit" exception
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["command-send"])


@pytest.mark.gds_cli
def test_components_no_search_term_given_error(standard_parser):
    # Not giving any string after giving the option should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["channels", "-s"])


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
