""" test_cmd_FileManager.py:

Test the command FileManager with basic integration tests.
    fileManager.CreateDirectory
    fileManager.ShellCommand
    fileManager.FileSize
    fileManager.MoveFile
    fileManager.AppendFile
    fileManager.RemoveFile
    fileManager.RemoveDirectory

"""


def test_send_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of FileManager commands.
    """

    deployment = fprime_test_api.get_deployment()

    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.CreateDirectory", ["/tmp/file"], max_delay=1)

    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.CreateDirectory", ["/tmp/file2"], max_delay=1)

    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.ShellCommand", ["ls", "/tmp/shell_ls.log"], max_delay=1)

    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.FileSize", ["/tmp/shell_ls.log"], max_delay=1)

    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.MoveFile", ["/tmp/shell_ls.log","/tmp/file/shell_ls2.log"], max_delay=1)
    

    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.ShellCommand", ["pwd", "/tmp/shell_pwd.log"], max_delay=1)

    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.FileSize", ["/tmp/shell_pwd.log"], max_delay=1)

    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.AppendFile", ["/tmp/shell_pwd.log", "/tmp/file/shell_ls_pwd.log"], max_delay=1)        


    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.RemoveFile", ["/tmp/shell_pwd.log",True], max_delay=1)

    # fail bc directory is not empty  (pytest script will stop if use send_and_assert_cmd.  Use send_command to avoid stop)

    fprime_test_api.send_command(deployment + "." + "fileManager.RemoveDirectory", ["/tmp/file"])

    fprime_test_api.send_and_assert_command(deployment + "." + "fileManager.RemoveDirectory", ["/tmp/file2"], max_delay=1)        
