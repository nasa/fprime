"""test_cmd_FileManager.py:

Test the command FileManager with basic integration tests.
    fileManager.CreateDirectory
    fileManager.ShellCommand  # Don't use ShellCommand
    fileManager.FileSize
    fileManager.MoveFile
    fileManager.AppendFile
    fileManager.RemoveFile
    fileManager.RemoveDirectory

"""

import time


def test_send_fileManager_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of fileManager commands.
    """
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "CreateDirectory",
        ["/tmp/file"],
        max_delay=10,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "CreateDirectory",
        ["/tmp/file2"],
        max_delay=10,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "FileSize",
        ["/tmp/1MiB.txt"],
        max_delay=50,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "MoveFile",
        ["/tmp/1MiB.txt", "/tmp/file/1MiB.txt"],
        max_delay=50,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "FileSize",
        ["/tmp/file/1MiB.txt"],
        max_delay=50,
    )
    # Use small file to AppendFile to work with RaspberryPi
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "AppendFile",
        ["/tmp/test_seq.seq", "/tmp/file/test_seq.seq"],
    )
    time.sleep(10)
    # put back
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "MoveFile",
        ["/tmp/file/1MiB.txt", "/tmp/1MiB.txt"],
        max_delay=50,
    )

    # fail bc directory is not empty  (pytest script will stop if use send_and_assert_cmd.  Use send_command to avoid stop) No max_delay (expected warning_hi)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "RemoveDirectory",
        ["/tmp/file"],
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "RemoveDirectory",
        ["/tmp/file2"],
        max_delay=10,
    )
    # Cleanup directory
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "RemoveFile",
        ["/tmp/file/test_seq.seq", True],
        max_delay=5,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "RemoveDirectory",
        ["/tmp/file"],
        max_delay=5,
    )
