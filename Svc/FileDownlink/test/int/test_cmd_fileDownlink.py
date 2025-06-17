""" test_cmd_fileDownlink.py:

Test the command FileDownlink with basic integration tests.
    fileDownlink.SendFile
    fileDownlink.SendPartial
    fileDownlink.Cancel


"""

def test_send_command(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of fileDownlink commands.
    """

    ## source = /tmp/file/shell_ls2.log   ,   Dest = /tmp/chuynh/fprime-downlink/DL.log
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileDownlink') + '.' + 'SendFile', ["/tmp/file/shell_ls2.log", "DL.log"], max_delay=5)


    ## source = /tmp/file/shell_ls2.log   ,   Dest = /tmp/chuynh/fprime-downlink/DL3.log, start offset 0 , for 30 Bytes
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileDownlink') + '.' + 'SendPartial', ["/tmp/file/shell_ls2.log", "DL3.log",0,30,], max_delay=5)    
    
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileDownlink') + '.' + 'Cancel', max_delay=1)
    
