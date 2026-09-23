"""test_tlm_packetizer.py:

Integration tests for Svc.TlmPacketizer. Run against a deployment that instantiates
Svc.TlmPacketizer (the stock Ref deployment uses Svc.TlmChan) with a --deployment-config
mapping "Svc.TlmPacketizer" to that instance, and whose packet set defines a packet with no
channels (see the "Empty" packet in TestDeploymentsProject/Ref/Top/RefPackets.fppi).
"""

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
