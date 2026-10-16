"""test_cfdp_manager.py:

Reusable integration tests for Svc.Ccsds.Cfdp.CfdpManager. They run against any
deployment that includes the component and whose flight software shares a
filesystem with the GDS host (the tests stage files on the local filesystem).

Covers nasa/fprime#5890: ground-configurable inputs must not reach FW_ASSERT.
"""

import json
import os
import shutil
import tempfile
import time

import pytest

MAX_FILE_PATH_SIZE = 200


@pytest.fixture
def cfdp(fprime_test_api):
    return fprime_test_api.get_mnemonic("Svc.Ccsds.Cfdp.CfdpManager")


@pytest.fixture
def playback_dir():
    """Directory containing one entry whose full path exceeds MaxFilePathSize and one short entry."""
    directory = tempfile.mkdtemp(prefix="cfdp_pb_")
    long_name = "a" * (MAX_FILE_PATH_SIZE - 1)
    assert len(directory) + 1 + len(long_name) > MAX_FILE_PATH_SIZE
    for name in (long_name, "short.bin"):
        with open(os.path.join(directory, name), "wb") as handle:
            handle.write(b"\x00" * 16)
    yield directory, long_name
    shutil.rmtree(directory, ignore_errors=True)


def test_playback_directory_skips_overlong_entry(fprime_test_api, cfdp, playback_dir):
    """PlaybackDirectory reports and skips an entry whose path exceeds MaxFilePathSize.

    Covers: CFDP-5890-1, CFDP-5890-2 (no assertion; SendFileInitiateFail event for the entry).
    """
    directory, long_name = playback_dir
    fprime_test_api.clear_histories()

    fprime_test_api.send_and_assert_command(
        f"{cfdp}.PlaybackDirectory",
        [0, 2, "CLASS_1", "KEEP", 0, directory, "/dest"],
        max_delay=5,
    )
    fprime_test_api.assert_event(f"{cfdp}.PlaybackInitiated", timeout=5)

    # The over-long entry is reported with SendFileInitiateFail and the scan continues
    failure = fprime_test_api.await_event(f"{cfdp}.SendFileInitiateFail", timeout=10)
    assert failure is not None, "Expected SendFileInitiateFail for the over-long entry"
    assert failure.get_args()[0].val == long_name

    # Flight software is still alive: a second command still round-trips
    fprime_test_api.send_and_assert_command(f"{cfdp}.ResetCounters", [0], max_delay=5)


def test_zero_timer_parameters_do_not_assert(fprime_test_api, cfdp):
    """A Class 2 transfer with ack_timer = inactivity_timer = 0 times out instead of asserting.

    Covers: CFDP-5890-3, CFDP-5890-4.
    """
    fprime_test_api.clear_histories()
    channel = {
        "ack_limit": 1,
        "nack_limit": 1,
        "ack_timer": 0,
        "inactivity_timer": 0,
        "dequeue_enabled": "ENABLED",
        "move_dir": "",
        "max_outgoing_pdus_per_cycle": 10,
        "tmp_dir": "/tmp",
        "fail_dir": "/tmp",
    }
    fprime_test_api.send_and_assert_command(
        f"{cfdp}.CHANNELCONFIG_PRM_SET", [json.dumps([channel, channel])], max_delay=5
    )

    source = tempfile.NamedTemporaryFile(prefix="cfdp_zero_", delete=False)
    source.write(b"\x01" * 64)
    source.close()
    try:
        fprime_test_api.send_and_assert_command(
            f"{cfdp}.SendFile",
            [0, 2, "CLASS_2", "KEEP", 0, source.name, "/dest/zero.bin"],
            max_delay=5,
        )
        # With a zero inactivity timer the transaction times out on the first 1 Hz cycle
        fprime_test_api.assert_event(f"{cfdp}.TxInactivityTimeout", timeout=10)
        # Flight software is still alive
        fprime_test_api.send_and_assert_command(
            f"{cfdp}.ResetCounters", [0], max_delay=5
        )
    finally:
        os.unlink(source.name)
