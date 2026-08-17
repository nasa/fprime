"""test_cmd_PrmDb.py:

Test the parameter database with basic integration tests.
"""


def test_send_PrmDb(fprime_test_api):
    """Test PrmDb commands

    Tests PrmDb's PRM_SAVE_FILE command and a FileManager move using send_and_assert command.

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
