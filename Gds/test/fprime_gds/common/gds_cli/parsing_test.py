"""
A set of PyTest-style unit tests for parsing commands
"""

import fprime_gds.executables.fprime_cli as fprime_cli
import pytest


@pytest.fixture
def standard_parser():
    return fprime_cli.create_parser()


# A set of valid CLI inputs and the output we expect from them
@pytest.mark.parametrize(
    "input_cli_arguments, expected_args_dict",
    [
        ("", {"func": None}),
        ([], {"func": None}),
        # TODO: Unsure how to test help/version args, since they exit the program?
        # (["-h"], {"func": None}),
        (
            ["channels"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": None,
                "id": None,
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["commands"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": None,
                "id": None,
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["command-send", "some.command.name"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "command_name": "some.command.name",
                "list": False,
                "key": None,
                "arguments": None,
                "url": "localhost:5000",
            },
        ),
        (
            ["events"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": None,
                "id": None,
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["channels", "-l"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": True,
                "session": None,
                "follow": None,
                "id": None,
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["channels", "--list"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": True,
                "session": None,
                "follow": None,
                "id": None,
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["channels", "-s", "348"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": 348,
                "follow": None,
                "id": None,
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["channels", "-f", "0.1"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": 0.1,
                "id": None,
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["commands", "-i", "10"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": None,
                "id": [10],
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["commands", "-i", "3", "4", "8"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": None,
                "id": [3, 4, 8],
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
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
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": None,
                "id": None,
                "component": [
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
                "search": None,
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["events", "-S", "per.aspera.ad.astra"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": None,
                "id": None,
                "component": None,
                "search": "per.aspera.ad.astra",
                "url": "localhost:5000",
                "json": False,
            },
        ),
        (
            ["events", "-u", "jpl.nasa.gov"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": None,
                "id": None,
                "component": None,
                "search": None,
                "url": "jpl.nasa.gov",
                "json": False,
            },
        ),
        (
            ["events", "-j"],
            {
                "func": fprime_cli.PLACEHOLDER_FUNC,
                "list": False,
                "session": None,
                "follow": None,
                "id": None,
                "component": None,
                "search": None,
                "url": "localhost:5000",
                "json": True,
            },
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
    # Not giving any Session ID after giving the option should raise an error
    with pytest.raises(SystemExit):
        fprime_cli.parse_args(standard_parser, ["commands", "-s"])


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
