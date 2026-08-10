"""asm_framer_integration_test.py:

Integration tests for the Svc::Ccsds::AsmFramer component. These tests run against a
deployment with the AsmFramer inserted between the TM framing layer and the com adapter
(e.g. TestDeploymentsProject/Ref), with the GDS using the matching ASM-aware deframing
chain (--framing-selection space-packet-space-data-link-asm).

If telemetry and events are received, the GDS successfully synchronized on the Attached
Sync Marker (CCSDS 131.0-B-5 Section 9) prepended by the AsmFramer; if commands execute,
the uplink path (which carries no ASM) is unaffected.
"""


def test_downlink_synchronization(fprime_test_api):
    """Telemetry is received through the ASM-marked downlink

    Receiving any telemetry proves the ASM deframer located the ASM and recovered
    the TM transfer frames behind it.
    """
    results = fprime_test_api.assert_telemetry_count(5, timeout=10)
    assert len(results) >= 5


def test_events_through_asm_downlink(fprime_test_api):
    """Events are received through the ASM-marked downlink"""
    fprime_test_api.send_and_assert_event(
        "CdhCore.cmdDisp.CMD_NO_OP",
        events=["CdhCore.cmdDisp.NoOpReceived"],
        timeout=5,
    )


def test_uplink_unaffected(fprime_test_api):
    """Commands execute: the TC uplink path carries no ASM and is unaffected"""
    fprime_test_api.send_and_assert_command("CdhCore.cmdDisp.CMD_NO_OP", timeout=5)
