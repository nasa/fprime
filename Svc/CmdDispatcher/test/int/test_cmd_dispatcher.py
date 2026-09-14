"""test_cmd_dispatcher.py:

Test the command dispatcher with basic integration tests.
"""


def test_send_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of CmdDispatcher commands.
    """

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher") + "." + "CMD_NO_OP",
        max_delay=1,
        timeout=15,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
        + "."
        + "CMD_CLEAR_TRACKING",
        max_delay=1,
        timeout=15,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
        + "."
        + "CMD_NO_OP_STRING",
        ["test_string_2"],
        max_delay=1,
        timeout=15,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher") + "." + "CMD_TEST_CMD_1",
        [2, 3, 4],
        max_delay=1,
        timeout=15,
    )


def test_dispatch_counters_nominal(fprime_test_api):
    """Test that dispatch counters are consistent under nominal commanding

    Related to CD-006 (the concurrent-drop requirement itself is verified by unit test). Sends commands
    and verifies CommandsDispatched advances on the dispatcher's run cycle while CommandsDropped
    (update-on-change) reports no new drops.
    """
    mnemonic = fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
    for _ in range(3):
        fprime_test_api.send_and_assert_command(
            mnemonic + "." + "CMD_NO_OP", max_delay=1, timeout=15
        )

    dispatched = fprime_test_api.await_telemetry(
        mnemonic + "." + "CommandsDispatched", start=None, timeout=15
    )
    assert dispatched is not None, "CommandsDispatched telemetry was not emitted"
    assert dispatched.get_val() >= 3
    fprime_test_api.assert_telemetry_count(
        0, mnemonic + "." + "CommandsDropped", start=None, timeout=2
    )
