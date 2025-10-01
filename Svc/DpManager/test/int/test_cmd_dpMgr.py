"""test_cmd_dpMgr.py:

Test the command dpMgr with basic integration tests.
"""


def test_send_dpMgr_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command
    """

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.DpManager") + "." + "CLEAR_EVENT_THROTTLE",
        max_delay=1,
    )
