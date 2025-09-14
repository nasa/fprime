"""test_cmd_uplink.py:

Test command dispatcher with basic integration tests.
"""


def test_send_uplink_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of CmdDispatcher commands.
    """
    # file_path = test_seq.seq  and destination = /tmp/test_seq.seq (for fileManager)
    fprime_test_api.uplink_file_and_await_completion(
        "test_seq.seq", "/tmp/test_seq.seq", timeout=100
    )
    # for fileDownlink
    fprime_test_api.uplink_file_and_await_completion(
        "test_seq_wait.seq", "/tmp/test_seq_wait.seq", timeout=100
    )
    # for health, fileDownlink, fileManager
    fprime_test_api.uplink_file_and_await_completion(
        "1MiB.txt", "/tmp/1MiB.txt", timeout=100
    )
