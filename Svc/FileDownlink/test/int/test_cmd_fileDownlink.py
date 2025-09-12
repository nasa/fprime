"""test_cmd_fileDownlink.py:

Test the command FileDownlink with basic integration tests.
    fileDownlink.SendFile
    fileDownlink.SendPartial
    fileDownlink.Cancel


"""


def test_send_fileDownlink_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of fileDownlink commands.
    """

    ## source = /tmp/test_seq_wait.seq   ,   Dest = /tmp/<user>/fprime-downlink/DL.log
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileDownlink") + "." + "SendFile",
        ["/tmp/test_seq_wait.seq", "DL.log"],
        max_delay=30,
    )

    ## source = /tmp/1MiB.txt   ,   Dest = /tmp/<user>/fprime-downlink/DL3.log, start offset 0 , for 100 Bytes
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileDownlink") + "." + "SendPartial",
        ["/tmp/1MiB.txt", "DL3.log", 0, 100],
        max_delay=5,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileDownlink") + "." + "Cancel", max_delay=5
    )
