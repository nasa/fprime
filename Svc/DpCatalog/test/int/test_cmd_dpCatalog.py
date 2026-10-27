"""test_cmd_dpWriter.py:

Test the command dpWriter with basic integration tests.
"""


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


# Highest FwDpPriorityType (U32) value; with a start of 0 this requests the whole catalog
PRIORITY_MAX = 4294967295


def test_start_xmit_catalog_inverted_range_rejected(fprime_test_api):
    """START_XMIT_CATALOG with start > end is rejected with VALIDATION_ERROR and an event

    No transmission starts, so the catalog is left as-is for a later command.
    """
    dp_catalog = fprime_test_api.get_mnemonic("Svc.DpCatalog")
    fprime_test_api.send_and_assert_command(dp_catalog + ".BUILD_CATALOG", max_delay=10)

    fprime_test_api.clear_histories()
    fprime_test_api.send_command(
        dp_catalog + ".START_XMIT_CATALOG", ["NO_WAIT", "false", 10, 5]
    )
    assert fprime_test_api.await_event(
        dp_catalog + ".XmitPriorityRangeInvalid", args=[10, 5], start=0, timeout=10
    ), "Inverted range must emit XmitPriorityRangeInvalid"
    op_code_error = fprime_test_api.await_event(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher") + ".OpCodeError",
        start=0,
        timeout=10,
    )
    assert op_code_error, "Inverted range must fail the command"
    assert (
        op_code_error.get_args()[1].val == "VALIDATION_ERROR"
    ), "Inverted range must fail with VALIDATION_ERROR"
    started = fprime_test_api.get_event_pred(dp_catalog + ".CatalogXmitRangeStarted")
    assert not [
        event for event in fprime_test_api.get_event_test_history().retrieve() if started(event)
    ], "Inverted range must not start transmission"


def test_start_xmit_catalog_full_range(fprime_test_api):
    """START_XMIT_CATALOG over [0, PRIORITY_MAX] transmits the whole catalog and completes"""
    dp_catalog = fprime_test_api.get_mnemonic("Svc.DpCatalog")
    fprime_test_api.send_and_assert_command(dp_catalog + ".BUILD_CATALOG", max_delay=10)

    fprime_test_api.clear_histories()
    fprime_test_api.send_and_assert_command(
        dp_catalog + ".START_XMIT_CATALOG",
        ["WAIT", "false", 0, PRIORITY_MAX],
        max_delay=120,
    )
    assert fprime_test_api.await_event(
        dp_catalog + ".CatalogXmitRangeStarted",
        args=[0, PRIORITY_MAX],
        start=0,
        timeout=10,
    )
    assert fprime_test_api.await_event(
        dp_catalog + ".CatalogXmitCompleted", start=0, timeout=10
    )
