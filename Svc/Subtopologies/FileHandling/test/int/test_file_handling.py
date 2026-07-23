"""test_file_handling.py:

Test the file handling functionality with:
1. Uplink a file
2. Downlink a file

Both these tests assume that FSW and GDS are running on the same system, for simplicity
Should projects require to run on different systems, they are encouraged to adapt these tests as needed
"""

import tempfile
from pathlib import Path
import random


def test_uplink_file(fprime_test_api):
    """IMPORTANT: These tests assume that FSW and GDS are running on the same system

    Create a file locally, "uplink" it (FSW and GDS are expected to be the same system)
    and verify the contents are the same after the uplink process."""

    TEST_DATA = f"test uplink data {random.random()}\n"

    tmp_file_in = tempfile.NamedTemporaryFile(mode="w+")
    tmp_file_out = tempfile.NamedTemporaryFile(mode="r+")

    tmp_file_in.write(TEST_DATA)
    tmp_file_in.flush()

    # Begin file uplink and wait for completion event
    fprime_test_api.uplink_file_and_await_completion(
        tmp_file_in.name, destination=tmp_file_out.name, timeout=15
    )

    tmp_file_out.seek(0)
    assert tmp_file_out.readlines() == [TEST_DATA]


def test_downlink_file(fprime_test_api):
    """IMPORTANT: These tests assume that FSW and GDS are running on the same system

    Create a file locally, "downlink" it (FSW and GDS are expected to be the same system)
    and verify the contents are the same after the downlink process.
    """
    TEST_DATA = f"test downlink data {random.random()}\n"

    # Retrieve GDS sandbox area for file downlink
    down_store = Path(fprime_test_api.pipeline.down_store)
    if not down_store.exists():
        down_store.mkdir(parents=True, exist_ok=True)

    # Add some randomness to avoid collisions in the downlink folder
    output_filename = f"downlink_test_{random.randint(0, 10000)}.txt"
    output_file = down_store / output_filename

    # Using dir="/tmp/" to force short filenames which can be a limitation on downlink
    tmp_file_in = tempfile.NamedTemporaryFile(mode="w+", dir="/tmp/")
    tmp_file_in.write(TEST_DATA)
    tmp_file_in.flush()

    # Request file downlink via FSW command
    fprime_test_api.send_and_assert_command(
        f"{fprime_test_api.get_mnemonic('Svc.FileDownlink')}.SendFile",
        [str(tmp_file_in.name), str(output_filename)],
    )

    assert output_file.read_text() == TEST_DATA
