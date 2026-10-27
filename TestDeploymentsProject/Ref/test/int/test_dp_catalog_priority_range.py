"""test_dp_catalog_priority_range.py:

Exercise the START_XMIT_CATALOG priority range against the Ref deployment, using
Ref.dpDemo to produce data products of known priorities.
"""

# Highest FwDpPriorityType (U32) value; with a start of 0 this requests the whole catalog
PRIORITY_MAX = 4294967295


def _sent_priorities(fprime_test_api, dp_catalog):
    """Priorities of the SendingProduct events received since the histories were cleared"""
    sending = fprime_test_api.get_event_pred(dp_catalog + ".SendingProduct")
    return [
        event.get_args()[2].val
        for event in fprime_test_api.get_event_test_history().retrieve()
        if sending(event)
    ]


def _xmit_range(fprime_test_api, dp_catalog, start, end):
    """Run a waited START_XMIT_CATALOG over [start, end] and return the priorities sent"""
    fprime_test_api.clear_histories()
    fprime_test_api.send_and_assert_command(
        dp_catalog + ".START_XMIT_CATALOG",
        ["WAIT", "false", start, end],
        max_delay=120,
    )
    assert fprime_test_api.await_event(
        dp_catalog + ".CatalogXmitRangeStarted", args=[start, end], start=0, timeout=10
    )
    assert fprime_test_api.await_event(
        dp_catalog + ".CatalogXmitCompleted", start=0, timeout=10
    )
    return _sent_priorities(fprime_test_api, dp_catalog)


def test_xmit_catalog_priority_range(fprime_test_api):
    """Only products with priority in [start, end] are sent; the rest stay cataloged"""
    dp_catalog = fprime_test_api.get_mnemonic("Svc.DpCatalog")

    # Build the catalog and drain anything left over from earlier tests
    fprime_test_api.send_and_assert_command(dp_catalog + ".CLEAR_CATALOG", max_delay=10)
    fprime_test_api.send_and_assert_command(dp_catalog + ".BUILD_CATALOG", max_delay=10)
    _xmit_range(fprime_test_api, dp_catalog, 0, PRIORITY_MAX)

    # Produce one data product per priority; each is added to the built catalog at runtime
    for priority in (10, 20, 30):
        fprime_test_api.clear_histories()
        fprime_test_api.send_and_assert_command(
            "Ref.dpDemo.Dp", ["IMMEDIATE", priority, "PROC_TYPE_NONE"]
        )
        assert fprime_test_api.await_event(
            dp_catalog + ".DpFileAdded", start=0, timeout=10
        ), f"priority {priority} product was not cataloged"

    # A sub-range sends only the product inside it
    assert _xmit_range(fprime_test_api, dp_catalog, 15, 25) == [20]

    # Bounds are inclusive and the remaining products are still available
    assert _xmit_range(fprime_test_api, dp_catalog, 30, 30) == [30]
    assert _xmit_range(fprime_test_api, dp_catalog, 0, PRIORITY_MAX) == [10]


def test_xmit_catalog_inverted_range_rejected(fprime_test_api):
    """start > end is rejected with VALIDATION_ERROR and XmitPriorityRangeInvalid"""
    dp_catalog = fprime_test_api.get_mnemonic("Svc.DpCatalog")
    fprime_test_api.send_and_assert_command(dp_catalog + ".BUILD_CATALOG", max_delay=10)

    fprime_test_api.clear_histories()
    fprime_test_api.send_command(
        dp_catalog + ".START_XMIT_CATALOG", ["NO_WAIT", "false", 25, 15]
    )
    assert fprime_test_api.await_event(
        dp_catalog + ".XmitPriorityRangeInvalid", args=[25, 15], start=0, timeout=10
    )
    op_code_error = fprime_test_api.await_event(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher") + ".OpCodeError",
        start=0,
        timeout=10,
    )
    assert op_code_error, "Inverted range must fail the command"
    assert op_code_error.get_args()[1].val == "VALIDATION_ERROR"
    started = fprime_test_api.get_event_pred(dp_catalog + ".CatalogXmitRangeStarted")
    assert not [
        event for event in fprime_test_api.get_event_test_history().retrieve() if started(event)
    ]
