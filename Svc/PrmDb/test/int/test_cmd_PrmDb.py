"""test_cmd_PrmDb.py:

Test the command dispatcher with basic integration tests.
"""


def test_send_PrmDb(fprime_test_api):
    """Test that commands may be sent

    Tests command send, dispatch, and receipt using send_and_assert command with a pair of CmdDispatcher commands.

    """

    # send PRM_SAVE_FILE (Wrote 0 records)

    # dir2 = fprime_test_api.get_prm_db_path()
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.PrmDb") + "." + "PRM_SAVE_FILE", max_delay=1
    )
    # Verify PrmDB.dat is empty open where fprime-gds start (ref) <fprime-gds start>/PrmDb.dat (PrmDb.dat file = 0bytes )
    # print ( "DIR2: %s ", dir2)

    # Current PRM_SAVE_FILE saved at where fprime-gds start <deployment>.  Move file to expected location
    fprime_test_api.send_and_assert_command(
        fprime_test_api.get_mnemonic("Svc.FileManager") + "." + "MoveFile",
        ["PrmDb.dat", fprime_test_api.get_mnemonic("Svc.PrmDb.filename")],
        max_delay=1,
    )


def test_PrmDb_load_file_empty_filename(fprime_test_api):
    """PRM_LOAD_FILE with an empty file name is rejected with VALIDATION_ERROR

    The component must emit PrmDbFileLoadFailed, report VALIDATION_ERROR, and remain
    usable: a subsequent PRM_SAVE_FILE completes normally.
    """
    prm_db = fprime_test_api.get_mnemonic("Svc.PrmDb")
    cmd_disp = fprime_test_api.get_mnemonic("Svc.CommandDispatcher")
    load_cmd = prm_db + ".PRM_LOAD_FILE"
    load_opcode = fprime_test_api.translate_command_name(load_cmd)

    fprime_test_api.send_and_assert_event(
        load_cmd,
        ["", "RESET"],
        [
            fprime_test_api.get_event_pred(prm_db + ".PrmDbFileLoadFailed"),
            fprime_test_api.get_event_pred(
                cmd_disp + ".OpCodeError", [load_opcode, "VALIDATION_ERROR"]
            ),
        ],
    )

    # Component is still idle and accepts further commands
    fprime_test_api.send_and_assert_command(prm_db + ".PRM_SAVE_FILE", max_delay=1)
