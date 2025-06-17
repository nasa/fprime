""" test_cmd_PrmDb.py:

Test the command dispatcher with basic integration tests.
"""
import time

def test_send_PrmDb(fprime_test_api):
    """Test that commands may be sent

     Tests command send, dispatch, and receipt using send_and_assert command with a pair of CmdDispatcher commands.

     # remove file PrmDB.dat

     recvBuffComp.PARAMETER1_PRM_SET/SAV  
     recvBuffComp.PARAMETER2_PRM_SET/SAV

     sendBuffComp.PARAMETER3_PRM_SET/SAV  
     sendBuffComp.PARAMETER4_PRM_SET/SAV

    """
    
    
    # 1st remove file PrmDB.dat (manually)
    # setup directory to remove later
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'CreateDirectory', ["/tmp/PrmDb"], max_delay=5)    
    
    # send PRM_SAVE_FILE (Wrote 0 records)
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.PrmDb') + '.' + 'PRM_SAVE_FILE', max_delay=1)
    # Verify PrmDB.dat is empty    open /tmp/save_file1 (PrmDb.dat file = 0bytes )
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'ShellCommand', ["ls -lagt", "/tmp/PrmDb/save_file1"], max_delay=5)


    ## setup default-value
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.RecvBuff') + '.' + 'PARAMETER1_PRM_SET', [5], max_delay=5)
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.RecvBuff') + '.' + 'PARAMETER2_PRM_SET', [6], max_delay=5)
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.SendBuff') + '.' + 'PARAMETER3_PRM_SET', [7], max_delay=5)
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.SendBuff') + '.' + 'PARAMETER4_PRM_SET', [8], max_delay=5)
    
    # send command PARAMETER1_PRM_SET / SAVE
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.RecvBuff') + '.' + 'PARAMETER1_PRM_SET', [11], max_delay=5)
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.RecvBuff') + '.' + 'PARAMETER1_PRM_SAVE', max_delay=1)    

    # Wrote 1 records
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.PrmDb') + '.' + 'PRM_SAVE_FILE', max_delay=5)
    # make sure file save
    time.sleep(2)
    # Verify PrmDb.dat is increase = 13 bytes     open /tmp/save_file2 (PrmDb.dat file = 13bytes )    
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'ShellCommand', ["ls -lagt", "/tmp/PrmDb/save_file2"], max_delay=5)        
    # Send PARAMETER2_PRM_SET / SAVE       signed integer -32867 and 32767
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.RecvBuff') + '.' + 'PARAMETER2_PRM_SET', [22], max_delay=5)
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.RecvBuff') + '.' + 'PARAMETER2_PRM_SAVE', max_delay=5)    

    # Wrote 2 records
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.PrmDb') + '.' + 'PRM_SAVE_FILE', max_delay=1)
    # make sure file save
    time.sleep(2)    
    # Verify PrmDb.dat is increate 24 bytes     open /tmp/save_file3 (PrmDb.dat file = 24bytes )    
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'ShellCommand', ["ls -lagt", "/tmp/PrmDb/save_file3"], max_delay=5)
    
    # Send PARAMETER3_PRM_SET / SAVE       unsigned integer 0..255
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.SendBuff') + '.' + 'PARAMETER3_PRM_SET', [33], max_delay=5)
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.SendBuff') + '.' + 'PARAMETER3_PRM_SAVE', max_delay=5)    

    # Wrote 3 records
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.PrmDb') + '.' + 'PRM_SAVE_FILE', max_delay=1)
    # make sure file save
    time.sleep(2)
    # Verify PrmDb.dat is increase = 34 bytes     open /tmp/save_file4 (PrmDb.dat file = 34bytes )    
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'ShellCommand', ["ls -lagt", "/tmp/PrmDb/save_file4"], max_delay=5)
    
    # Send PARAMETER4_PRM_SET / SAVE       float
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.SendBuff') + '.' + 'PARAMETER4_PRM_SET', [44], max_delay=5)
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Ref.SendBuff') + '.' + 'PARAMETER4_PRM_SAVE', max_delay=5)    

    # Wrote 4 records
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.PrmDb') + '.' + 'PRM_SAVE_FILE', max_delay=1)
    # make sure file save
    time.sleep(2)        
    # Verify PrmDb.dat is increase = 47 bytes     open /tmp/save_file5 (PrmDb.dat file = 47bytes )    
    fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.FileManager') + '.' + 'ShellCommand', ["ls -lagt", "/tmp/PrmDb/save_file5"], max_delay=5)

    # Manually verify PrmDb.dat file is increase each set/save parameter updated  in /tmp/PrmDb directory
    # remove directory after confirm it
    # remove PrmDb.dat from Ref directory (next time run see PrmDb.dat size increase otherwise will be same size as last run)
    






