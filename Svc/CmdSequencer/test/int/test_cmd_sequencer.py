"""test_cmd_sequencer.py:

A set of integration tests to apply to the CmdSequencer app. This is intended to be a reference of integration testing.
"""

import subprocess
import time
import os
from pathlib import Path


def test_generate_file(fprime_test_api):
    """generate seq files before run seqgen"""

    # Create test_seq_new.seq file with Deployment name
    with open("test_seq_new.seq", "w") as f:
        f.write("; A test sequence " + "\n")
        f.write(";\n")
        f.write(
            "R00:00:00"
            + " "
            + (
                fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
                + "."
                + "CMD_NO_OP"
            )
            + "\n\n"
        )

        f.write("; Let's try out some commands with arguments " + "\n")
        f.write(
            "R00:00:01.050"
            + " "
            + (
                fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
                + "."
                + "CMD_NO_OP_STRING"
            )
            + " "
            + '"'
            + "Awesome String!"
            + '"'
            + ";"
            + "\n"
        )

    # raise error
    try:
        with open("test_seq_new.seq", "x") as f:
            f.write("Created using exclusive mode.")
    except FileExistsError:
        print("Already exists.")

    # Create test_seq_wait_new.seq file with Deployment name
    with open("test_seq_wait_new.seq", "w") as f2:
        f2.write("; A test sequence " + "\n")
        f2.write(";\n")
        f2.write(
            "R00:00:00"
            + " "
            + (
                fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
                + "."
                + "CMD_NO_OP"
            )
            + "\n\n"
        )

        f2.write("; Let's try out some commands with arguments " + "\n")
        f2.write(
            "R00:00:01.050"
            + " "
            + (
                fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
                + "."
                + "CMD_NO_OP_STRING"
            )
            + " "
            + '"'
            + "SEQ WAIT 2 MINS!"
            + '"'
            + "\n"
        )
        f2.write(";\n")
        f2.write(
            "R00:02:00.050"
            + " "
            + (
                fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
                + "."
                + "CMD_NO_OP_STRING"
            )
            + " "
            + '"'
            + "SEQ after 2mins!"
            + '"'
            + "\n"
        )
        f2.write(";\n")
        f2.write(
            "R00:00:01.050"
            + " "
            + (
                fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
                + "."
                + "CMD_NO_OP_STRING"
            )
            + " "
            + '"'
            + "SEQ DONE !"
            + '"'
            + "\n"
        )
        f2.write(";\n")

    # raise error
    try:
        with open("test_seq_wait_new.seq", "x") as f2:
            f2.write("Created using exclusive mode.")
    except FileExistsError:
        print("Already exists.")


def test_seqgen(fprime_test_api):
    """Tests the seqgen can be dispatched (requires localhost testing)"""

    deployment = fprime_test_api.get_deployment()

    print("DEPLOYMENT: %s", deployment)

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_AUTO", max_delay=10
    )

    sequence = Path(__file__).parent / "test_seq_new.seq"
    sequence2 = Path(__file__).parent / "test_seq_wait_new.seq"

    assert (
        subprocess.run(
            [
                "fprime-seqgen",
                "-d",
                str(fprime_test_api.pipeline.dictionary_path),
                str(sequence),
                "ref_test_seq.bin",
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
                "ref_test_seq_wait.bin",
            ]
        ).returncode
        == 0
    ), "Failed to run fprime-seqgen"

    # uplink ref_test_seq.bin and ref_test_seq_wait.bin
    fprime_test_api.uplink_file_and_await_completion(
        "ref_test_seq.bin", "/tmp/ref_test_seq.bin", timeout=100
    )
    fprime_test_api.uplink_file_and_await_completion(
        "ref_test_seq_wait.bin", "/tmp/ref_test_seq_wait.bin", timeout=100
    )

    # execute sequence
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_RUN",
        args=["/tmp/ref_test_seq.bin", "BLOCK"],
        max_delay=5,
    )

    ######    ###### remove new file 1   ######
    #    print(f" file1:",sequence)
    #    print(f" file2:",sequence2)
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

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_VALIDATE",
        ["/tmp/ref_test_seq.bin"],
        max_delay=10,
    )
    # sequence execute_2 auto
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_RUN",
        ["/tmp/ref_test_seq.bin", "BLOCK"],
        max_delay=5,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
        + "."
        + "CMD_NO_OP_STRING",
        ["test_string_execute_2 auto completed"],
        max_delay=10,
    )

    ######    ######    ######
    ## Toggle Manual/AUTO
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_MANUAL",
        max_delay=10,
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_AUTO", max_delay=10
    )

    ######    ######    ######
    ## Manual Mode testing Start/Cancel
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_MANUAL",
        max_delay=10,
    )

    # Load Sequence but not execute is current SEQ manual (will load sequence only)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_RUN",
        ["/tmp/ref_test_seq.bin", "NO_BLOCK"],
        max_delay=5,
    )

    # sequence execute_5 manually Command 0 in sequence
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_START",
        max_delay=10,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_CANCEL",
        max_delay=10,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
        + "."
        + "CMD_NO_OP_STRING",
        ["manually START/CANCEL after cmd 0"],
        max_delay=10,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_VALIDATE",
        ["/tmp/ref_test_seq.bin"],
        max_delay=10,
    )

    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_AUTO", max_delay=10
    )

    ######    ######    ######
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_MANUAL",
        max_delay=10,
    )
    ## Manual Mode testing Start  WARNING_LO = No sequence active
    # WARNING_LO => No sequence active and EXECUTION_ERROR.  No completion (will cause pytest to assert when no completion) use send_command will ignore completion?

    # fprime_test_api.send_and_assert_command(fprime_test_api.get_mnemonic('Svc.CmdSequencer') + '.' + 'CS_START', max_delay=10)
    fprime_test_api.send_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_START"
    )

    # Load Sequence but not execute is current SEQ manual (will load sequence only)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_RUN",
        ["/tmp/ref_test_seq_wait.bin", "NO_BLOCK"],
        max_delay=5,
    )

    # sequence manually mode start  Command 0 in sequence
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_START",
        max_delay=10,
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
        + "."
        + "CMD_NO_OP_STRING",
        ["manually START command 0"],
        max_delay=10,
    )

    # sequence manually mode step  Next Command 1  in sequence  (Only manual mode)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_STEP", max_delay=3
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
        + "."
        + "CMD_NO_OP_STRING",
        ["manually START/STEP cmd 1 "],
        max_delay=3,
    )

    time.sleep(10)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_STEP", max_delay=10
    )
    time.sleep(130)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.CmdSequencer") + "." + "CS_STEP", max_delay=3
    )

    # cleanup sequence files (sequence create local and uplink files)
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "RemoveFile",
        ["/tmp/ref_test_seq.bin", True],
        max_delay=15,
    )
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "RemoveFile",
        ["/tmp/ref_test_seq_wait.bin", True],
        max_delay=15,
    )
    ######    ######    ######
