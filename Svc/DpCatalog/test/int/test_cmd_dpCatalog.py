"""test_cmd_dpWriter.py:

Test the command dpWriter with basic integration tests.
"""

import re

import pytest


def test_send_dpWriter_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command
    """

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.DpCatalog") + "." + "CLEAR_CATALOG",
        max_delay=10,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.DpCatalog") + "." + "BUILD_CATALOG",
        max_delay=10,
    )
    # wait/no_wait option command fatal
    # F    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.DpCatalog') + '.' + 'START_XMIT_CATALOG', ["NO_WAIT"], max_delay=10)

    # warning_lo bc DpCatalog transmit not active
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.DpCatalog") + "." + "STOP_XMIT_CATALOG",
        max_delay=10,
    )


# ----------------------------------------------------------------------
# SET_DP_PRIORITY
# ----------------------------------------------------------------------

# Data product file names are Dp_<id>_<seconds>_<microseconds>.fdp, decimal fields zero-padded to at
# least 8 digits (DP_FILENAME_FORMAT)
DP_FILE_NAME = re.compile(r"Dp_([0-9]{8,})_([0-9]{8,})_([0-9]{8,})\.fdp$")

# Command of the Ref DpDemo component that generates a data product, if the deployment has one
DP_DEMO_COMMAND = "Ref.dpDemo.Dp"


def _dp_identity(file_name):
    """Return (id, tSec, tSub) of a data product from its file name"""
    match = DP_FILE_NAME.search(file_name)
    assert match is not None, f"{file_name} is not a data product file name"
    return int(match.group(1)), int(match.group(2)), int(match.group(3))


def _build_catalog(fprime_test_api):
    """Stop any transmit, then rebuild the catalog from the data product directories"""
    dp_cat = fprime_test_api.get_mnemonic("Svc.DpCatalog")
    fprime_test_api.send_and_assert_command(dp_cat + ".STOP_XMIT_CATALOG", max_delay=10)
    fprime_test_api.send_and_assert_command(dp_cat + ".CLEAR_CATALOG", max_delay=10)
    fprime_test_api.send_and_assert_command(dp_cat + ".BUILD_CATALOG", max_delay=10)
    return dp_cat


def _generate_dp(fprime_test_api, priority):
    """Generate a data product with DpDemo and return its (id, tSec, tSub, file name) from DpFileAdded

    DpDemo fills the product on its next rate group tick and DpWriter notifies DpCatalog, which adds it
    to the built catalog and reports the file name.
    """
    if DP_DEMO_COMMAND not in fprime_test_api.pipeline.dictionaries.command_name:
        pytest.skip(
            f"deployment has no {DP_DEMO_COMMAND} command to generate a data product"
        )
    dp_cat = fprime_test_api.get_mnemonic("Svc.DpCatalog")
    added = fprime_test_api.send_and_await_event(
        DP_DEMO_COMMAND,
        ["IMMEDIATE", priority, "PROC_TYPE_NONE"],
        dp_cat + ".DpFileAdded",
        timeout=30,
    )
    assert (
        added is not None
    ), "DpFileAdded was not received for the generated data product"
    file_name = added.get_args()[0].val
    return _dp_identity(file_name) + (file_name,)


def test_set_dp_priority_not_found(fprime_test_api):
    """SET_DP_PRIORITY for a product that is not in the catalog emits DpNotFound and fails

    The catalog is left usable: a following STOP_XMIT_CATALOG completes normally.
    """
    dp_cat = _build_catalog(fprime_test_api)
    cmd_disp = fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
    set_cmd = dp_cat + ".SET_DP_PRIORITY"
    set_opcode = fprime_test_api.translate_command_name(set_cmd)

    fprime_test_api.send_and_assert_event(
        set_cmd,
        [0xDEAD, 1, 2, 1],
        [
            fprime_test_api.get_event_pred(dp_cat + ".DpNotFound", [0xDEAD, 1, 2]),
            fprime_test_api.get_event_pred(
                cmd_disp + ".OpCodeError", [set_opcode, "EXECUTION_ERROR"]
            ),
        ],
        timeout=10,
    )
    fprime_test_api.send_and_assert_command(dp_cat + ".STOP_XMIT_CATALOG", max_delay=10)


def test_set_dp_priority_reorders_transmit(fprime_test_api):
    """SET_DP_PRIORITY moves a product in the transmit order; the same priority is a no-op

    Two products are generated with priorities 20 and 30. Raising the second to 5 makes it the first
    product sent when the catalog is transmitted, ahead of the first one, which is reported sent with
    its unchanged priority 20.
    """
    dp_cat = _build_catalog(fprime_test_api)
    first = _generate_dp(fprime_test_api, 20)
    second = _generate_dp(fprime_test_api, 30)
    assert (
        first[:3] != second[:3]
    ), "generated data products must have distinct identities"

    # Same priority: reported with equal old and new values, command succeeds
    fprime_test_api.send_and_assert_command(
        dp_cat + ".SET_DP_PRIORITY",
        [first[0], first[1], first[2], 20],
        max_delay=10,
        events=[
            fprime_test_api.get_event_pred(
                dp_cat + ".DpPrioritySet", [first[0], first[1], first[2], 20, 20]
            )
        ],
    )

    # Raise the second product above the first
    fprime_test_api.send_and_assert_command(
        dp_cat + ".SET_DP_PRIORITY",
        [second[0], second[1], second[2], 5],
        max_delay=10,
        events=[
            fprime_test_api.get_event_pred(
                dp_cat + ".DpPrioritySet", [second[0], second[1], second[2], 30, 5]
            )
        ],
    )

    # The second product is sent before the first, each with its transmit priority
    sending_second = fprime_test_api.get_event_pred(
        dp_cat + ".SendingProduct", [second[3], None, 5]
    )
    sending_first = fprime_test_api.get_event_pred(
        dp_cat + ".SendingProduct", [first[3], None, 20]
    )
    results = fprime_test_api.send_and_await_event(
        dp_cat + ".START_XMIT_CATALOG",
        ["NO_WAIT", False],
        [sending_second, sending_first],
        timeout=120,
    )
    assert (
        len(results) == 2
    ), "the reprioritized product was not sent before the other one"
    fprime_test_api.assert_event(dp_cat + ".CatalogXmitCompleted", timeout=120)
