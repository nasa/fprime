""" test_cmd_dispatcher.py:

Test the command dispatcher with basic integration tests.
"""


def test_send_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of NO-OP commands.
    """
    fprime_test_api.send_and_assert_command("cmdDisp.CMD_NO_OP", max_delay=0.1)
