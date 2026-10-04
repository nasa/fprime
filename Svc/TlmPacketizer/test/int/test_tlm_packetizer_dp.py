"""Integration tests for data product recording in Svc.TlmPacketizer

These tests exercise the START_DP_RECORDING / STOP_DP_RECORDING commands of a running deployment through the GDS.
They expect the deployment to connect the packetizer's product ports to a data product manager and writer, to send
at least one packet of TLM_GROUP every second, and to map "Svc.TlmPacketizer" in its int_config.json.
"""

import pytest
from fprime_gds.common.testing_fw import predicates

TLM_GROUP = 1  # Group recorded by these tests. Ref sends its group 1 packets every second.
PACKETS_PER_CONTAINER = 3
LARGE_PACKETS_PER_CONTAINER = 20  # Never reached within a test, yet small enough for the data product buffers of Ref
PRIORITY = 7
INVALID_GROUP = 0xFFFF  # Larger than any MAX_CONFIGURABLE_TLMPACKETIZER_GROUP a project would configure


@pytest.fixture
def packetizer(fprime_test_api):
    """Mnemonic of the Svc.TlmPacketizer instance of the deployment under test"""
    return fprime_test_api.get_mnemonic("Svc.TlmPacketizer")


@pytest.fixture(autouse=True)
def stop_recording(fprime_test_api, packetizer):
    """Leave the packetizer with recording disabled, even when a test fails midway"""
    yield
    fprime_test_api.send_and_assert_command(f"{packetizer}.STOP_DP_RECORDING", [TLM_GROUP], max_delay=5)


class NoGroupRecording(predicates.predicate):
    """Predicate on the DpGroupsRecording channel value: no group is being recorded"""

    def __call__(self, value):
        return not any(bool(flag) for flag in value)

    def __str__(self):
        return "all DpGroupsRecording flags are False"


class GroupRecording(predicates.predicate):
    """Predicate on the DpGroupsRecording channel value: exactly the given group is being recorded"""

    def __init__(self, group):
        self.group = group

    def __call__(self, value):
        return all(bool(flag) == (index == self.group) for index, flag in enumerate(value))

    def __str__(self):
        return f"only DpGroupsRecording[{self.group}] is True"


def test_dp_recording_disabled_by_default(fprime_test_api, packetizer):
    """TPK-012: no data product is recorded unless commanded

    Stopping a group that is not recording responds OK, reports NOT_RECORDING, and the recording telemetry still shows no
    recorded group.
    """
    fprime_test_api.send_and_assert_command(
        f"{packetizer}.STOP_DP_RECORDING",
        [TLM_GROUP],
        max_delay=5,
        events=[fprime_test_api.get_event_pred(f"{packetizer}.DpRecordingStopped", [TLM_GROUP, "NOT_RECORDING"])],
    )
    fprime_test_api.assert_telemetry(f"{packetizer}.DpGroupsRecording", NoGroupRecording(), timeout=5)


def test_dp_recording_start_and_stop(fprime_test_api, packetizer):
    """TPK-008, TPK-011, TPK-012: a started group fills and sends containers, and stopping sends the partial one

    While recording, packets of the group keep flowing to the ground as regular packetized telemetry, the data product
    writer reports written files, and the recording counters advance. Stopping reports the partial container.
    """
    dp_writer = fprime_test_api.get_mnemonic("Svc.DpWriter")
    start = fprime_test_api.telemetry_history.size()  # telemetry index before the command, to avoid racing its effects
    event_start = fprime_test_api.event_history.size()
    fprime_test_api.send_and_assert_command(
        f"{packetizer}.START_DP_RECORDING",
        [TLM_GROUP, PACKETS_PER_CONTAINER, PRIORITY],
        max_delay=5,
        events=[
            fprime_test_api.get_event_pred(
                f"{packetizer}.DpRecordingStarted", [TLM_GROUP, PACKETS_PER_CONTAINER, PRIORITY]
            )
        ],
    )
    fprime_test_api.assert_telemetry(f"{packetizer}.DpGroupsRecording", GroupRecording(TLM_GROUP), start=start, timeout=10)

    # A container is sent once PACKETS_PER_CONTAINER packets of the group were recorded, and written by the writer
    fprime_test_api.assert_telemetry(
        f"{packetizer}.DpContainersSent", predicates.greater_than(0), start=start, timeout=30
    )
    fprime_test_api.assert_telemetry(
        f"{packetizer}.DpPacketsRecorded", predicates.greater_than_or_equal_to(PACKETS_PER_CONTAINER), timeout=10
    )
    fprime_test_api.assert_event(f"{dp_writer}.FileWritten", timeout=30)

    # The counters above arrive as regular packetized telemetry, so downlink keeps flowing while recording (TPK-012),
    # and every container of this small size was obtained
    fprime_test_api.assert_event_count(0, [f"{packetizer}.DpBufferError"], start=event_start)

    start = fprime_test_api.telemetry_history.size()
    fprime_test_api.send_and_assert_command(
        f"{packetizer}.STOP_DP_RECORDING",
        [TLM_GROUP],
        max_delay=5,
        events=[
            fprime_test_api.get_event_pred(
                f"{packetizer}.DpRecordingStopped",
                [TLM_GROUP, predicates.is_a_member_of(["PARTIAL_SENT", "PARTIAL_NOT_SENT"])],
            )
        ],
    )
    fprime_test_api.assert_telemetry(f"{packetizer}.DpGroupsRecording", NoGroupRecording(), start=start, timeout=10)


def test_dp_recording_restart_applies_new_settings(fprime_test_api, packetizer):
    """TPK-011: starting a group that is already recording sends its partial container and restarts

    The second START_DP_RECORDING is accepted (OK) and reported with the new settings.
    """
    fprime_test_api.send_and_assert_command(
        f"{packetizer}.START_DP_RECORDING", [TLM_GROUP, LARGE_PACKETS_PER_CONTAINER, PRIORITY], max_delay=5
    )
    fprime_test_api.assert_telemetry(f"{packetizer}.DpPacketsRecorded", start="NOW", timeout=10)
    start = fprime_test_api.telemetry_history.size()
    fprime_test_api.send_and_assert_command(
        f"{packetizer}.START_DP_RECORDING",
        [TLM_GROUP, PACKETS_PER_CONTAINER, PRIORITY + 1],
        max_delay=5,
        events=[
            fprime_test_api.get_event_pred(
                f"{packetizer}.DpRecordingStarted", [TLM_GROUP, PACKETS_PER_CONTAINER, PRIORITY + 1]
            )
        ],
    )
    # The partial container of the first run was sent on restart
    fprime_test_api.assert_telemetry(
        f"{packetizer}.DpContainersSent", predicates.greater_than(0), start=start, timeout=10
    )


def test_dp_recording_rejects_invalid_arguments(fprime_test_api, packetizer):
    """TPK-013: invalid group and packet count are rejected with VALIDATION_ERROR and a DpRecordingRejected event"""
    for command, args, reason in [
        ("START_DP_RECORDING", [INVALID_GROUP, PACKETS_PER_CONTAINER, PRIORITY], "START_INVALID_GROUP"),
        ("START_DP_RECORDING", [TLM_GROUP, 0, PRIORITY], "START_INVALID_PACKET_COUNT"),
        ("STOP_DP_RECORDING", [INVALID_GROUP], "STOP_INVALID_GROUP"),
    ]:
        mnemonic = f"{packetizer}.{command}"
        events = [
            fprime_test_api.get_event_pred(f"{packetizer}.DpRecordingRejected", [args[0], reason]),
            fprime_test_api.get_event_pred(
                "cmdDisp.OpCodeError", [fprime_test_api.translate_command_name(mnemonic), "VALIDATION_ERROR"]
            ),
        ]
        fprime_test_api.send_and_assert_event(mnemonic, args, events, timeout=10)
    # None of the rejected commands started recording
    fprime_test_api.send_and_assert_command(
        f"{packetizer}.STOP_DP_RECORDING",
        [TLM_GROUP],
        max_delay=5,
        events=[fprime_test_api.get_event_pred(f"{packetizer}.DpRecordingStopped", [TLM_GROUP, "NOT_RECORDING"])],
    )
