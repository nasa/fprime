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


def test_delete_dp_rejections(fprime_test_api):
    """DELETE_DP is rejected with a DpDeleteError event before the catalog is built and for an
    unknown product. Both cases leave the deployment untouched, so this runs against any
    deployment that includes DpCatalog.
    """
    dp_catalog = fprime_test_api.get_mnemonic("Svc.DpCatalog")
    unknown_dp = [0xFFFFFFFE, 1, 2]

    # CLEAR_CATALOG drops the built catalog: deletion is refused until the next BUILD_CATALOG
    fprime_test_api.send_and_assert_command(dp_catalog + ".CLEAR_CATALOG", max_delay=10)
    result = fprime_test_api.send_and_await_event(
        dp_catalog + ".DELETE_DP", unknown_dp, dp_catalog + ".DpDeleteError", timeout=10
    )
    assert result, "DELETE_DP before BUILD_CATALOG was not rejected"
    assert "NOT_BUILT" in result.get_display_text()

    fprime_test_api.send_and_assert_command(dp_catalog + ".BUILD_CATALOG", max_delay=10)
    result = fprime_test_api.send_and_await_event(
        dp_catalog + ".DELETE_DP", unknown_dp, dp_catalog + ".DpDeleteError", timeout=10
    )
    assert result, "DELETE_DP of an unknown product was not rejected"
    assert "NOT_FOUND" in result.get_display_text()
