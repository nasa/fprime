"""test_cmd_dpWriter.py:

Test the command dpWriter with basic integration tests.
"""


def test_send_dpWriter_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command
    """

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.DpWriter") + "." + "CLEAR_EVENT_THROTTLE",
        max_delay=10,
    )
