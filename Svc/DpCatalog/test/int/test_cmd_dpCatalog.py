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
