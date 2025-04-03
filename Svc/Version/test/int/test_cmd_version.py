""" test_cmd_version.py:

Test the command version with basic integration tests.
"""


def test_send_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of version commands.
    """
    fprime_test_api.send_and_assert_command("version.VERSION",["PROJECT"], max_delay=0.1)
    
    fprime_test_api.send_and_assert_command("version.VERSION",["FRAMEWORK"], max_delay=0.1)

    fprime_test_api.send_and_assert_command("version.VERSION",["LIBRARY"], max_delay=0.1)

    fprime_test_api.send_and_assert_command("version.VERSION",["CUSTOM"], max_delay=0.1)

    fprime_test_api.send_and_assert_command("version.VERSION",["ALL"], max_delay=0.1)

    ##
    fprime_test_api.send_and_assert_command("version.ENABLE",["ENABLED"], max_delay=0.1)

    fprime_test_api.send_and_assert_command("version.ENABLE",["DISABLED"], max_delay=0.1)                
    




