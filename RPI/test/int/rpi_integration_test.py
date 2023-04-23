import os
import subprocess
import time
from enum import Enum

from fprime_gds.common.testing_fw import predicates
from fprime_gds.common.utils.event_severity import EventSeverity


"""
This enum is includes the values of EventSeverity that can be filtered by the ActiveLogger Component
"""
FilterSeverity = Enum(
    "FilterSeverity",
    "WARNING_HI WARNING_LO COMMAND ACTIVITY_HI ACTIVITY_LO DIAGNOSTIC",
)


def set_event_filter(fprime_test_api, severity, enabled):
    """
    This helper will send a command that updates the given severity filter on the ActiveLogger
    Component in the Ref App.
    Args:
        severity: A valid FilterSeverity Enum Value (str) or an instance of FilterSeverity
        enabled: a boolean of whether or not to enable the given severity
    Return:
        boolean of whether the report filter was set successfully.
    """
    enabled = "ENABLED" if enabled else "DISABLED"
    if isinstance(severity, FilterSeverity):
        severity = severity.name
    else:
        severity = FilterSeverity[severity].name
    try:
        fprime_test_api.send_command(
            "eventLogger.SET_EVENT_FILTER",
            [severity, enabled],
        )
        return True
    except AssertionError:
        return False


def set_default_filters(fprime_test_api):
    """
    Sets the default send filters on the ref aps ActiveLogger
    """
    set_event_filter(fprime_test_api, "COMMAND", True)
    set_event_filter(fprime_test_api, "ACTIVITY_LO", True)
    set_event_filter(fprime_test_api, "ACTIVITY_HI", True)
    set_event_filter(fprime_test_api, "WARNING_LO", True)
    set_event_filter(fprime_test_api, "WARNING_HI", True)
    set_event_filter(fprime_test_api, "DIAGNOSTIC", False)


def test_is_streaming(fprime_test_api):
    results = fprime_test_api.assert_telemetry_count(5, timeout=10)
    for result in results:
        msg = "received channel {} update: {}".format(result.get_id(), result.get_str())
        print(msg)


def test_send_command(fprime_test_api):
    fprime_test_api.send_and_assert_command("cmdDisp.CMD_NO_OP", max_delay=0.1)
    assert fprime_test_api.get_command_test_history().size() == 1
    fprime_test_api.send_and_assert_command("cmdDisp.CMD_NO_OP", max_delay=0.1)
    assert fprime_test_api.get_command_test_history().size() == 2


def test_send_and_assert_no_op(fprime_test_api):
    length = 100
    failed = 0
    evr_seq = ["OpCodeDispatched", "NoOpReceived", "OpCodeCompleted"]
    any_reordered = False
    dropped = False
    for i in range(0, length):
        results = fprime_test_api.send_and_await_event(
            "cmdDisp.CMD_NO_OP", events=evr_seq, timeout=25
        )
        msg = "Send and assert NO_OP Trial #{}".format(i)
        if not fprime_test_api.test_assert(len(results) == 3, msg, True):
            items = fprime_test_api.get_event_test_history().retrieve()
            last = None
            reordered = False
            for item in items:
                if last is not None:
                    if item.get_time() < last.get_time():
                        fprime_test_api.log(
                            "during iteration #{}, a reordered event was detected: {}".format(
                                i, item
                            )
                        )
                        any_reordered = True
                        reordered = True
                        break
                last = item
            if not reordered:
                fprime_test_api.log(
                    "during iteration #{}, a dropped event was detected".format(i)
                )
                dropped = True
            failed += 1
        fprime_test_api.clear_histories()

    case = True
    case &= fprime_test_api.test_assert(
        not any_reordered, "Expected no events to be reordered.", True
    )
    case &= fprime_test_api.test_assert(
        not dropped, "Expected no events to be dropped.", True
    )
    msg = "{} sequences failed out of {}".format(failed, length)
    case &= fprime_test_api.test_assert(failed == 0, msg, True)

    assert (
        case
    ), "Expected all checks to pass (reordering, dropped events, all passed). See log."


def test_active_logger_filter(fprime_test_api):
    set_default_filters(fprime_test_api)
    try:
        cmd_events = fprime_test_api.get_event_pred(severity=EventSeverity.COMMAND)
        actHI_events = fprime_test_api.get_event_pred(
            severity=EventSeverity.ACTIVITY_HI
        )
        pred = predicates.greater_than(0)
        zero = predicates.equal_to(0)
        # Drain time for dispatch events
        time.sleep(10)

        fprime_test_api.send_and_assert_command("cmdDisp.CMD_NO_OP")
        fprime_test_api.send_and_assert_command("cmdDisp.CMD_NO_OP")

        time.sleep(0.5)

        fprime_test_api.assert_event_count(pred, cmd_events)
        fprime_test_api.assert_event_count(pred, actHI_events)

        set_event_filter(fprime_test_api, FilterSeverity.COMMAND, False)
        # Drain time for dispatch events
        time.sleep(10)
        fprime_test_api.clear_histories()
        fprime_test_api.send_command("cmdDisp.CMD_NO_OP")
        fprime_test_api.send_command("cmdDisp.CMD_NO_OP")

        time.sleep(0.5)

        fprime_test_api.assert_event_count(zero, cmd_events)
        fprime_test_api.assert_event_count(pred, actHI_events)
    finally:
        set_default_filters(fprime_test_api)


def test_seqgen(fprime_test_api):
    """Tests the seqgen code"""
    sequence = os.path.join(os.path.dirname(__file__), "test_seq.seq")
    assert (
        subprocess.run(
            [
                "fprime-seqgen",
                "-d",
                str(fprime_test_api.pipeline.dictionary_path),
                sequence,
                "/tmp/ref_test_int.bin",
            ]
        ).returncode
        == 0
    ), "Failed to run fprime-seqgen"
    fprime_test_api.send_and_assert_command(
        "cmdSeq.CS_RUN", args=["/tmp/ref_test_int.bin", "BLOCK"], max_delay=5
    )
