""" test_cmd_dispatcher.py:

Test the command dispatcher with basic integration tests.
"""


def test_send_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of CmdDispatcher commands.
    """
    deployment = fprime_test_api.get_deployment()
    
    fprime_test_api.send_and_assert_command(deployment + "." + "cmdDisp.CMD_NO_OP", max_delay=1)

    fprime_test_api.send_and_assert_command(deployment + "." + "cmdDisp.CMD_CLEAR_TRACKING", max_delay=1)

    fprime_test_api.send_and_assert_command(deployment + "." + "cmdDisp.CMD_NO_OP_STRING",["test_string_2"], max_delay=1)
    
    fprime_test_api.send_and_assert_command(deployment + "." + "cmdDisp.CMD_TEST_CMD_1",[2,3,4], max_delay=1)



