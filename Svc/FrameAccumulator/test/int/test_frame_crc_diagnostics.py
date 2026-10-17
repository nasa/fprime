"""Exercise F Prime CRC diagnostics through a running deployment and GDS."""

import os
import struct
import time
import zlib

import pytest
from fprime_gds.common.utils.event_severity import EventSeverity


def corrupt_frame():
    """Encode the FprimeProtocol U32 header and invert one CRC bit."""
    body = bytes([1, 2, 3, 4])
    framed = struct.pack(">II", 0xDEADBEEF, len(body)) + body
    return framed + struct.pack(">I", zlib.crc32(framed) ^ 1)


@pytest.mark.skipif(
    not os.environ.get("FPRIME_CRC_INJECTOR_SOCKET"),
    reason="Provide a raw-uplink injection socket; see test/int/README.md",
)
def test_default_crc_warning_throttle_and_recovery(fprime_test_api, frame_crc_injector):
    """Covers SVC-FRAME-ACCUMULATOR-005/006/007/008/009 via the live stack.

    Requires FprimeFrameDetector, warnings enabled, and the default quota of
    one warning per 60 seconds. The raw-uplink hook must target this instance.
    """
    api = fprime_test_api
    accumulator = api.get_mnemonic("Svc.FrameAccumulator")
    dispatcher = api.get_mnemonic("Svc.CommandDispatcher")
    event = f"{accumulator}.FrameCrcFailure"
    command = f"{dispatcher}.CMD_NO_OP"
    warnings = api.get_event_subhistory(api.get_event_pred(event), fsw_order=False)
    try:
        api.send_and_assert_command(command, max_delay=5)
        bad = corrupt_frame()
        frame_crc_injector(bad[:-4])
        api.assert_telemetry_count(2, start="NOW", timeout=5)
        time.sleep(0.5)
        assert warnings.size() == 0, "An incomplete candidate is not a CRC failure"

        frame_crc_injector(bad[-4:])
        first = api.assert_event(event, history=warnings, start=0, timeout=5)
        first_received = time.monotonic()
        assert first.get_severity() == EventSeverity.WARNING_LO
        assert [arg.val for arg in first.get_args()] == [1, 1]
        print("FIRST_CRC_WARNING", [arg.val for arg in first.get_args()], flush=True)
        api.send_and_assert_command(command, max_delay=5)

        frame_crc_injector(bad + bad)
        api.send_and_assert_command(command, max_delay=5)
        time.sleep(0.5)
        assert warnings.size() == 1, "Repeated CRC failures must be throttled"

        # Wait beyond the full default period before checking the next snapshot.
        time.sleep(max(0, 61 - (time.monotonic() - first_received)))
        api.send_and_assert_command(command, max_delay=5)
        frame_crc_injector(bad)
        received = api.assert_event_count(
            2, event, history=warnings, start=0, timeout=5
        )
        second = received[-1]
        assert [arg.val for arg in second.get_args()] == [4, 3 * len(bad) + 1]
        print("SECOND_CRC_WARNING", [arg.val for arg in second.get_args()], flush=True)
        assert second.get_time() - first.get_time() >= 60
        api.send_and_assert_command(command, max_delay=5)
        assert warnings.size() == 2
    finally:
        api.remove_event_subhistory(warnings)
