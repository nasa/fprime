""" ref_integration_test.py:

A set of integration tests to apply to the Ref app. This is intended to be a reference of integration testing.
"""

import subprocess
import time
import os
#from enum import Enum
from pathlib import Path

#from fprime_gds.common.testing_fw import predicates
#from fprime_gds.common.utils.event_severity import EventSeverity

## regenerate .seq file

def regen_file(seqfile,deployment):
    # ex: test_seq.seq
    oldfile = open (seqfile +".seq", "r")
    newfile = open (seqfile +"_new.seq", "w")

    eachline = oldfile.readlines()
    line = 1
    for writing in eachline:
        
        index = writing.find("cmdDisp.")
        if index != -1:
            writing = writing[:index] + deployment + "." + writing[index:]
        
        newfile.write(writing)        
        line = line + 1

    oldfile.close()
    newfile.close()
    return seqfile +"_new.seq"

def test_seqgen(fprime_test_api):
    
    """Tests the seqgen can be dispatched (requires localhost testing)"""

    deployment =  fprime_test_api.get_deployment()
    
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_AUTO', max_delay=1)
    
#    sequence = Path(__file__).parent / "test_seq_new.seq"
#    sequence2 = Path(__file__).parent / "test_seq_wait_new.seq"

    sequence = Path(__file__).parent / regen_file("test_seq",deployment)
    sequence2 = Path(__file__).parent / regen_file("test_seq_wait",deployment)

    
    assert (
        subprocess.run(
            [
                "fprime-seqgen",
                "-d",
                str(fprime_test_api.pipeline.dictionary_path),
                str(sequence),
                "/tmp/ref_test_seq.bin",
            ]
        ).returncode
        == 0
    ), "Failed to run fprime-seqgen"

    assert (
        subprocess.run(
            [
                "fprime-seqgen",
                "-d",
                str(fprime_test_api.pipeline.dictionary_path),
                str(sequence2),
                "/tmp/ref_test_seq_wait.bin",
            ]
        ).returncode
        == 0
    ), "Failed to run fprime-seqgen"
    
    fprime_test_api.send_and_assert_command(
        fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_RUN', args=["/tmp/ref_test_seq.bin", "BLOCK"], max_delay=5
    )

    ######    ###### remove newfile   ######
    print(f" file:",sequence)
    os.remove(sequence)
    os.remove(sequence2)
    
def test_send_seq(fprime_test_api):
    """
     CS_RUN
     CS_VALIDATE
     CS_MANUAL
     CS_AUTO
     CS_START
     CS_CANCEL
     CS_STEP
     CS_JOINT_WAIT  (Need to have more 1 run sequence.  Ref don't have more one sequence)
    """

    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_VALIDATE',["/tmp/ref_test_seq.bin"], max_delay=1)    
    # sequence execute_2 auto 
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_RUN',["/tmp/ref_test_seq.bin", "BLOCK"], max_delay=5)

    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CommandDispatcher') + '.' + 'CMD_NO_OP_STRING',["test_string_execute_2 auto completed"], max_delay=1)    
    
    ######    ######    ######
    ## Toggle Manual/AUTO
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_MANUAL', max_delay=1)
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_AUTO', max_delay=1)
    
    ######    ######    ######    
    ## Manual Mode testing Start/Cancel
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_MANUAL', max_delay=1)
    
    # Load Sequence but not execute is current SEQ manual (will load sequence only)
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_RUN', ["/tmp/ref_test_seq.bin", "NO_BLOCK"], max_delay=5)
    
    # sequence execute_5 manually Command 0 in sequence
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_START', max_delay=1)    

    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_CANCEL', max_delay=1)        

    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CommandDispatcher') + '.' + 'CMD_NO_OP_STRING',["manually START/CANCEL after cmd 0"], max_delay=1)

    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_VALIDATE',["/tmp/ref_test_seq.bin"], max_delay=1)

    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_AUTO', max_delay=1)
    
    ######    ######    ######    
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_MANUAL', max_delay=1)
    ## Manual Mode testing Start  WARNING_LO = No sequence active
    # WARNING_LO => No sequence active and EXCUTION_ERROR.  No completion (will cause pytest to assert when no completion) use send_command will ingore completion?

    #fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_START', max_delay=1)
    fprime_test_api.send_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_START')

    # Load Sequence but not execute is current SEQ manual (will load sequence only)    
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_RUN', ["/tmp/ref_test_seq_wait.bin", "NO_BLOCK"], max_delay=5)
    
    # sequence manually mode start  Command 0 in sequence    
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_START', max_delay=1)
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CommandDispatcher') + '.' + 'CMD_NO_OP_STRING',["manually START command 0"], max_delay=1)


    # sequence manually mode step  Next Command 1  in sequence  (Only manual mode)          
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_STEP', max_delay=3)        
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CommandDispatcher') + '.' + 'CMD_NO_OP_STRING',["manually START/STEP cmd 1 "], max_delay=3)
    
    time.sleep(10)    
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_STEP', max_delay=1)
    time.sleep(130)    
    fprime_test_api.send_and_assert_command(fprime_test_api.getCmdDispName('Svc.CmdSequencer') + '.' + 'CS_STEP', max_delay=3)    
    
   ######    ######    ######              
