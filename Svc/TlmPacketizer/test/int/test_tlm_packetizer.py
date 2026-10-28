"""test_tlm_packetizer.py:

Reusable integration tests for Svc.TlmPacketizer per-packet configuration.

The deployment configuration (``--deployment-config``) must map the instance and
name one packet whose channels update at least once per rate-group tick:

    "Svc.TlmPacketizer": "CdhCore.tlmSend",
    "Svc.TlmPacketizer.section": "REALTIME",
    "Svc.TlmPacketizer.packetId": 6,
    "Svc.TlmPacketizer.packetGroup": 2,
    "Svc.TlmPacketizer.packetChannel": "Ref.systemResources.CPU",
    "Svc.TlmPacketizer.otherPacketChannel": "CdhCore.cmdDisp.CommandsDispatched",
    "Svc.TlmPacketizer.maxLevel": 3,
    "Svc.TlmPacketizer.quietSeconds": 4,
    "Svc.TlmPacketizer.tickSeconds": 1

``quietSeconds`` is how long a packet must stay silent to count as "not sent"; it
must exceed the packetizer's run period. ``tickSeconds`` is the period of the
packetizer's ``Run`` port, used to size the rate-override observation window.
"""

import time

import pytest

from fprime_gds.common.testing_fw import predicates

UNKNOWN_PACKET_ID = 0xFFFFFFF0


class Packetizer:
    """Deployment-specific knobs for the component under test."""

    def __init__(self, api):
        self.api = api
        config = api.load_config_file() or {}
        self.instance = api.get_mnemonic("Svc.TlmPacketizer")
        self.section = config.get("Svc.TlmPacketizer.section", "REALTIME")
        self.packet_id = int(config.get("Svc.TlmPacketizer.packetId", 1))
        self.group = int(config.get("Svc.TlmPacketizer.packetGroup", 1))
        self.channel = config["Svc.TlmPacketizer.packetChannel"]
        self.other_channel = config.get("Svc.TlmPacketizer.otherPacketChannel")
        self.max_level = int(config.get("Svc.TlmPacketizer.maxLevel", 3))
        self.quiet = float(config.get("Svc.TlmPacketizer.quietSeconds", 4))
        self.tick = float(config.get("Svc.TlmPacketizer.tickSeconds", 1))

    def command(self, name, args, events=None):
        return self.api.send_and_assert_command(
            f"{self.instance}.{name}", args, max_delay=2, events=events
        )

    def event(self, name, args=None, timeout=5):
        result = self.api.assert_event(f"{self.instance}.{name}", args, timeout=timeout)
        print(f"EVR {result.get_str(verbose=True)}")
        return result

    def failed_command(self, name, args):
        """Send a command that must fail; assert the dispatcher reports the error."""
        self.api.clear_histories()
        self.api.send_command(f"{self.instance}.{name}", args)
        dispatcher = self.api.get_mnemonic("Svc.CommandDispatcher")
        result = self.api.assert_event(f"{dispatcher}.OpCodeError", timeout=5)
        print(f"EVR {result.get_str(verbose=True)}")

    def packet_flows(self):
        update = self.api.await_telemetry(self.channel, timeout=self.quiet + 1)
        assert update is not None, f"{self.channel} not received"
        print(f"TLM {self.channel} received")

    def packet_silent(self):
        self.api.clear_histories()
        update = self.api.await_telemetry(self.channel, timeout=self.quiet)
        assert update is None, f"{self.channel} received while packet was inhibited"
        print(f"TLM {self.channel} silent for {self.quiet}s")

    def other_packet_flows(self):
        if self.other_channel is None:
            return
        update = self.api.await_telemetry(self.other_channel, timeout=self.quiet + 1)
        assert update is not None, f"{self.other_channel} not received"
        print(f"TLM {self.other_channel} received")

    def restore(self):
        self.command("ENABLE_SECTION", [self.section, "ENABLED"])
        self.command("SET_LEVEL", [self.max_level])
        self.command(
            "CONFIGURE_GROUP_SEND", [self.section, self.group, "INHERIT_FROM_PARENT"]
        )
        self.command("CLEAR_PACKET_OVERRIDE", [self.section, self.packet_id])


@pytest.fixture
def packetizer(fprime_test_api):
    pkt = Packetizer(fprime_test_api)
    fprime_test_api.clear_histories()
    yield pkt
    pkt.restore()


def test_packet_config_report_default(packetizer):
    """GET_PACKET_CONFIG reports GROUP_DEFAULT for an un-overridden packet.

    Covers: SVC-TLMPACKETIZER-GET-PACKET-CONFIG
    """
    packetizer.packet_flows()
    packetizer.command("GET_PACKET_CONFIG", [packetizer.section, packetizer.packet_id])
    report = packetizer.event("PacketConfigReport")
    assert "GROUP_DEFAULT" in report.get_display_text()


def test_packet_send_never_override(packetizer):
    """OVERRIDE_PACKET_SEND SEND_NEVER stops one packet; CLEAR restores it.

    Covers: SVC-TLMPACKETIZER-PACKET-SEND-OVERRIDE, SVC-TLMPACKETIZER-CLEAR-OVERRIDE
    """
    packetizer.command(
        "OVERRIDE_PACKET_SEND",
        [packetizer.section, packetizer.packet_id, "SEND_NEVER"],
    )
    packetizer.event("PacketOverrideSet")
    packetizer.packet_silent()
    packetizer.other_packet_flows()
    packetizer.command("GET_PACKET_CONFIG", [packetizer.section, packetizer.packet_id])
    report = packetizer.event("PacketConfigReport")
    assert "OVERRIDE_SEND" in report.get_display_text()
    assert "SEND_NEVER" in report.get_display_text()
    packetizer.command(
        "CLEAR_PACKET_OVERRIDE", [packetizer.section, packetizer.packet_id]
    )
    packetizer.event("PacketOverrideCleared")
    packetizer.packet_flows()


def test_packet_send_always_beats_group(packetizer):
    """A SEND_ALWAYS packet override sends although its group is SEND_NEVER.

    Covers: SVC-TLMPACKETIZER-SEND-RESOLUTION
    """
    packetizer.command(
        "CONFIGURE_GROUP_SEND", [packetizer.section, packetizer.group, "SEND_NEVER"]
    )
    packetizer.packet_silent()
    packetizer.command(
        "OVERRIDE_PACKET_SEND",
        [packetizer.section, packetizer.packet_id, "SEND_ALWAYS"],
    )
    packetizer.event("PacketOverrideSet")
    packetizer.packet_flows()
    packetizer.command("GET_PACKET_CONFIG", [packetizer.section, packetizer.packet_id])
    report = packetizer.event("PacketConfigReport")
    assert "SEND_ALWAYS" in report.get_display_text()


def test_packet_send_always_beats_section_disable(packetizer):
    """A SEND_ALWAYS packet override sends although its section is DISABLED.

    Covers: SVC-TLMPACKETIZER-SEND-RESOLUTION, SVC-TLMPACKETIZER-SECTION-ENABLE
    """
    packetizer.command("ENABLE_SECTION", [packetizer.section, "DISABLED"])
    packetizer.packet_silent()
    packetizer.command(
        "OVERRIDE_PACKET_SEND",
        [packetizer.section, packetizer.packet_id, "SEND_ALWAYS"],
    )
    packetizer.event("PacketOverrideSet")
    packetizer.packet_flows()
    packetizer.command("ENABLE_SECTION", [packetizer.section, "ENABLED"])
    packetizer.command(
        "CLEAR_PACKET_OVERRIDE", [packetizer.section, packetizer.packet_id]
    )
    packetizer.event("PacketOverrideCleared")


def test_packet_inherit_follows_group(packetizer):
    """An INHERIT_FROM_PARENT packet override follows the group's SEND_NEVER.

    Covers: SVC-TLMPACKETIZER-SEND-RESOLUTION
    """
    packetizer.command(
        "OVERRIDE_PACKET_SEND",
        [packetizer.section, packetizer.packet_id, "INHERIT_FROM_PARENT"],
    )
    packetizer.event("PacketOverrideSet")
    packetizer.packet_flows()
    packetizer.command(
        "CONFIGURE_GROUP_SEND", [packetizer.section, packetizer.group, "SEND_NEVER"]
    )
    packetizer.packet_silent()


def test_packet_rates_override(packetizer):
    """OVERRIDE_PACKET_RATES throttles one packet without touching its send state.

    Covers: SVC-TLMPACKETIZER-PACKET-RATES-OVERRIDE
    """
    min_ticks = 5
    packetizer.command(
        "OVERRIDE_PACKET_RATES",
        [packetizer.section, packetizer.packet_id, "ON_CHANGE_MIN", min_ticks, 0],
    )
    packetizer.event("PacketOverrideSet")
    packetizer.command("GET_PACKET_CONFIG", [packetizer.section, packetizer.packet_id])
    report = packetizer.event("PacketConfigReport")
    assert "OVERRIDE_RATES" in report.get_display_text()
    window_ticks = 2 * min_ticks + 1
    packetizer.api.clear_histories()
    time.sleep(window_ticks * packetizer.tick)
    updates = packetizer.api.assert_telemetry_count(
        predicates.within_range(1, window_ticks // min_ticks + 1),
        channels=packetizer.channel,
    )
    print(
        f"TLM {packetizer.channel}: {len(updates)} updates in {window_ticks} ticks with min={min_ticks}"
    )
    packetizer.command(
        "CLEAR_PACKET_OVERRIDE", [packetizer.section, packetizer.packet_id]
    )
    packetizer.event("PacketOverrideCleared")


def test_set_level_gates_group(packetizer):
    """SET_LEVEL below the packet's group stops an un-overridden packet; raising the level restores it.

    Covers: SVC-TLMPACKETIZER-SET-LEVEL
    """
    packetizer.command("SET_LEVEL", [packetizer.group - 1])
    packetizer.event("LevelSet")
    packetizer.packet_silent()
    packetizer.command("SET_LEVEL", [packetizer.max_level])
    packetizer.event("LevelSet")
    packetizer.packet_flows()


def test_unknown_packet_rejected(packetizer):
    """Configuration commands for an unknown packet id fail with PacketNotFound.

    Covers: SVC-TLMPACKETIZER-PACKET-NOT-FOUND
    """
    packetizer.failed_command(
        "GET_PACKET_CONFIG", [packetizer.section, UNKNOWN_PACKET_ID]
    )
    packetizer.event("PacketNotFound", [UNKNOWN_PACKET_ID])
    packetizer.failed_command(
        "OVERRIDE_PACKET_SEND", [packetizer.section, UNKNOWN_PACKET_ID, "SEND_NEVER"]
    )
    packetizer.event("PacketNotFound", [UNKNOWN_PACKET_ID])
    packetizer.failed_command(
        "CLEAR_PACKET_OVERRIDE", [packetizer.section, UNKNOWN_PACKET_ID]
    )
    packetizer.event("PacketNotFound", [UNKNOWN_PACKET_ID])


def test_invalid_section_rejected(packetizer):
    """NUM_SECTIONS is not a configurable section: the command fails with InvalidPacketConfig.

    Covers: SVC-TLMPACKETIZER-INVALID-SECTION
    """
    packetizer.failed_command(
        "OVERRIDE_PACKET_SEND", ["NUM_SECTIONS", packetizer.packet_id, "SEND_NEVER"]
    )
    packetizer.event("InvalidPacketConfig", [packetizer.packet_id])
    packetizer.failed_command(
        "GET_PACKET_CONFIG", ["NUM_SECTIONS", packetizer.packet_id]
    )
    packetizer.event("InvalidPacketConfig", [packetizer.packet_id])


# Packet ID of the channel-less "Empty" packet in the deployment's packet specification
EMPTY_PACKET_ID = 39


def test_send_empty_packet(fprime_test_api):
    """Test that a packet specification with no channels is accepted and can be sent

    The deployment starting at all demonstrates that setPacketList accepted the empty packet.
    SEND_PKT on it must complete with OK and emit PacketSent rather than PacketNotFound.
    """
    packetizer = fprime_test_api.get_mnemonic("Svc.TlmPacketizer")
    events = [
        fprime_test_api.get_event_pred(f"{packetizer}.PacketSent", [EMPTY_PACKET_ID])
    ]
    fprime_test_api.send_and_assert_command(
        f"{packetizer}.SEND_PKT",
        [EMPTY_PACKET_ID, "REALTIME"],
        max_delay=1,
        timeout=15,
        events=events,
    )
    fprime_test_api.assert_event_count(
        0, fprime_test_api.get_event_pred(f"{packetizer}.PacketNotFound")
    )
