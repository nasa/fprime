"""
A set of PyTest-style unit tests for parsing commands
"""

import pytest


import fprime_gds.common.gds_cli.channels as channels
import fprime_gds.common.gds_cli.commands as commands
import fprime_gds.common.gds_cli.command_send as command_send
import fprime_gds.common.gds_cli.events as events
import fprime_gds.executables.fprime_cli as fprime_cli


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
        "func": None,
        "dictionary_path": "PATH",
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


def default_valid_channels_dict(updated_values_dict={}):
    dictionary = default_valid_args_dict(
        {"func": channels.ChannelsCommand.handle_arguments}
    )
    dictionary.update(updated_values_dict)
    return dictionary


def default_valid_commands_dict(updated_values_dict={}):
    dictionary = default_valid_args_dict(
        {"func": commands.CommandsCommand.handle_arguments}
    )
    dictionary.update(updated_values_dict)
    return dictionary


def default_valid_events_dict(updated_values_dict={}):
    dictionary = default_valid_args_dict(
        {"func": events.EventsCommand.handle_arguments}
    )
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
        (["channels", "PATH"], default_valid_channels_dict(),),
        (["commands", "PATH"], default_valid_commands_dict(),),
        (
            ["command-send", "PATH", "some.command.name"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "command_name": "some.command.name",
                "arguments": None,
                "dictionary_path": "PATH",
                "ip_address": "127.0.0.1",
                "port": 50050,
            },
        ),
        (["events", "PATH"], default_valid_events_dict(),),
        (["channels", "PATH", "-l"], default_valid_channels_dict({"list": True}),),
        (["channels", "PATH", "--list"], default_valid_channels_dict({"list": True}),),
        (
            ["channels", "follow/the/yellow/brick/road"],
            default_valid_channels_dict(
                {"dictionary_path": "follow/the/yellow/brick/road"}
            ),
        ),
        (["channels", "PATH", "-f"], default_valid_channels_dict({"follow": True}),),
        (["commands", "PATH", "-i", "10"], default_valid_commands_dict({"ids": [10]}),),
        (
            ["commands", "PATH", "-i", "3", "4", "8"],
            default_valid_commands_dict({"ids": [3, 4, 8]}),
        ),
        (
            [
                "events",
                "PATH",
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
            ["events", "-S", "per.aspera.ad.astra", "PATH"],
            default_valid_events_dict({"search": "per.aspera.ad.astra"}),
        ),
        (
            ["events", "-ip", "jpl.nasa.gov", "PATH"],
            default_valid_events_dict({"ip_address": "jpl.nasa.gov"}),
        ),
        (["events", "-j", "PATH"], default_valid_events_dict({"json": True}),),
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
def test_no_path_given_error(standard_parser):
    # Not giving any path to a dictionary should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["channels"])


@pytest.mark.gds_cli
def test_command_send_no_command_given_error(standard_parser):
    # Not giving a command option here should raise a "SystemExit" exception
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["command-send", "PATH"])


@pytest.mark.gds_cli
def test_components_no_search_term_given_error(standard_parser):
    # Not giving any string after giving the option should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["commands", "PATH", "-S"])


@pytest.mark.gds_cli
def test_id_flag_no_ids_given_error(standard_parser):
    # Not giving any IDs after giving the option should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["channels", "PATH", "-i"])


@pytest.mark.gds_cli
def test_id_flag_float_id_given_error(standard_parser):
    # Giving a non-integer ID here should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["events", "PATH", "-i", "34.8"])


@pytest.mark.gds_cli
def test_id_flag_string_id_given_error(standard_parser):
    # Giving a non-integer ID here should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["channels", "PATH", "-i", "crashtime"])


@pytest.mark.gds_cli
def test_components_no_components_given_error(standard_parser):
    # Not giving any components after giving the option should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["events", "PATH", "-c"])
