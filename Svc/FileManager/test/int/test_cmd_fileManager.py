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

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of fileManager commands.
    """
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'CreateDirectory', ["/tmp/file"], max_delay=1)

    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'CreateDirectory', ["/tmp/file2"], max_delay=1)

    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'ShellCommand', ["ls", "/tmp/shell_ls.log"], max_delay=1)

    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'FileSize', ["/tmp/shell_ls.log"], max_delay=1)

    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'MoveFile', ["/tmp/shell_ls.log","/tmp/file/shell_ls2.log"], max_delay=1)
    

    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'ShellCommand', ["pwd", "/tmp/shell_pwd.log"], max_delay=1)

    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'FileSize', ["/tmp/shell_pwd.log"], max_delay=1)

    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'AppendFile', ["/tmp/shell_pwd.log", "/tmp/file/shell_ls_pwd.log"], max_delay=1)        


    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'RemoveFile', ["/tmp/shell_pwd.log", True] , max_delay=1)

    # fail bc directory is not empty  (pytest script will stop if use send_and_assert_cmd.  Use send_command to avoid stop) No max_delay

    fprime_test_api.send_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'RemoveDirectory', ["/tmp/file"])

    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'RemoveDirectory', ["/tmp/file2"], max_delay=1)        
