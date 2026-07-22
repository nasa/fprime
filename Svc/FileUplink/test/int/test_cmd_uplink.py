"""test_cmd_uplink.py:

Test FileUplink component with basic integration tests.
"""

from pathlib import Path


def test_send_uplink_command(fprime_test_api):
    """Test file uplink commands

    Uploads test files from fprime test directory to FSW /tmp/.
    These files are then used by FileManager and FileDownlink tests.
    """
    # Locate test files in fprime repository
    test_dir = Path(__file__).parent.resolve()

    fprime_test_api.uplink_file_and_await_completion(
        str(test_dir / "test_seq.seq"), "/tmp/test_seq.seq", timeout=100
    )
    fprime_test_api.uplink_file_and_await_completion(
        str(test_dir / "test_seq_wait.seq"), "/tmp/test_seq_wait.seq", timeout=100
    )
    fprime_test_api.uplink_file_and_await_completion(
        str(test_dir / "1MiB.txt"), "/tmp/1MiB.txt", timeout=100
    )
