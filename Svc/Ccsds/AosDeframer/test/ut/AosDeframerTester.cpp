// ======================================================================
// \title  AosDeframerTester.cpp
// \author Auto-generated
// \brief  cpp file for AosDeframer component test harness implementation class
// ======================================================================

#include "AosDeframerTester.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/Types/AOSHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/AOSTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Types/M_PDUHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

AosDeframerTester::AosDeframerTester()
    : AosDeframerGTestBase("AosDeframerTester", AosDeframerTester::MAX_HISTORY_SIZE), component("AosDeframer") {
    this->initComponents();
    this->connectPorts();
}

AosDeframerTester::~AosDeframerTester() {}

void AosDeframerTester::assertDataOutVcId(const U8 expectedVcId) const {
    const U32 dataOutSize = static_cast<U32>(this->fromPortHistory_dataOut->size());
    for (U32 i = 0; i < dataOutSize; i++) {
        ASSERT_EQ(this->fromPortHistory_dataOut->at(i).context.get_vcId(), expectedVcId);
    }
}

// ----------------------------------------------------------------------
// Tests - Basic Validation
// ----------------------------------------------------------------------

void AosDeframerTester::testNominalDeframing() {
    const U8 testVcId = 7;
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, testVcId,
                              PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK);

    // Create a simple SPP packet
    U8 payload[100];
    FwSizeType sppSize = this->createSppPacket(payload, 0x001, 50);  // APID 1, 50 bytes data

    // Assemble frame with FHP=0
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0, ComCfg::SpacecraftId, testVcId);
    ComCfg::FrameContext context;

    // Invoke the deframer
    this->invoke_to_dataIn(0, buffer, context);

    // Should output one packet
    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(testVcId);
    ASSERT_from_dataReturnOut_SIZE(1);  // Frame buffer returned

    // Verify packet content and context
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), sppSize);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_pvn(), ComCfg::Pvn::SPACE_PACKET_PROTOCOL);

    // Verify telemetry
    ASSERT_TLM_SIZE(3);  // LatestVcFrameCount, FramesProcessed, and PacketsExtracted
    ASSERT_TLM_FramesProcessed_SIZE(1);
    ASSERT_TLM_FramesProcessed(0, 1);
    ASSERT_TLM_PacketsExtracted_SIZE(1);
    ASSERT_TLM_PacketsExtracted(0, 1);
    ASSERT_TLM_LatestVcFrameCount_SIZE(1);
    ASSERT_TLM_LatestVcFrameCount(0, 0);  // vcCount=0 (assembleFrameBuffer default)
}

void AosDeframerTester::testDataReturn() {
    this->configureDefault();

    U8 data[1] = {0};
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext context;

    this->invoke_to_dataReturnIn(0, buffer, context);

    // dataReturnIn receives back dynamically allocated packet buffers from downstream
    // and deallocates them via the deallocate port.
    ASSERT_from_deallocate_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_deallocate->at(0).fwBuffer.getData(), data);
}

void AosDeframerTester::testInvalidScId() {
    this->configureDefault();

    U8 payload[50];
    FwSizeType sppSize = this->createSppPacket(payload, 0x001, 20);

    // Use wrong spacecraft ID
    U16 wrongScid = static_cast<U16>((ComCfg::SpacecraftId + 1) & 0x3FF);
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0, wrongScid);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // No packets should be output
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // Frame returned
    ASSERT_from_errorNotify_SIZE(1);
    ASSERT_from_errorNotify(0, Ccsds::FrameError::AOS_INVALID_SCID);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidSpacecraftId_SIZE(1);
}

void AosDeframerTester::testInvalidVcId() {
    // Configure to accept only VCID 0 (always filtered - no accept-all-vcid mode)
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0);

    U8 payload[50];
    FwSizeType sppSize = this->createSppPacket(payload, 0x001, 20);

    // Use wrong VCID (1 instead of 0)
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0, ComCfg::SpacecraftId, 1);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify_SIZE(1);
    ASSERT_from_errorNotify(0, Ccsds::FrameError::AOS_INVALID_VCID);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidVcId_SIZE(1);
}

void AosDeframerTester::testInvalidFrameLength() {
    this->configureDefault();

    // Send a buffer smaller than expected frame size
    U8 shortBuffer[50];
    ::memset(shortBuffer, 0, sizeof(shortBuffer));
    Fw::Buffer buffer(shortBuffer, sizeof(shortBuffer));
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify_SIZE(1);
    ASSERT_from_errorNotify(0, Ccsds::FrameError::AOS_INVALID_LENGTH);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidFrameLength_SIZE(1);
}

void AosDeframerTester::testInvalidFecf() {
    this->configureDefault();

    U8 payload[50];
    FwSizeType sppSize = this->createSppPacket(payload, 0x001, 20);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0);

    // Corrupt the CRC (last 2 bytes)
    buffer.getData()[TEST_FRAME_SIZE - 1] ^= 0xFF;

    ComCfg::FrameContext context;
    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify_SIZE(1);
    ASSERT_from_errorNotify(0, Ccsds::FrameError::AOS_INVALID_CRC);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidFecf_SIZE(1);
    ASSERT_TLM_CrcErrorCount_SIZE(1);
    ASSERT_TLM_CrcErrorCount(0, 1);

    // Second corrupt frame - verify counter accumulates
    this->clearHistory();
    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload, sppSize, 0);
    buffer2.getData()[TEST_FRAME_SIZE - 1] ^= 0xFF;
    this->invoke_to_dataIn(0, buffer2, context);
    ASSERT_TLM_CrcErrorCount(0, 2);
}

void AosDeframerTester::testInvalidTfvn() {
    this->configureDefault();

    U8 payload[50];
    FwSizeType sppSize = this->createSppPacket(payload, 0x001, 20);

    // Use wrong TFVN (0 instead of 1)
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0, ComCfg::SpacecraftId, 0, 0, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify_SIZE(1);
    ASSERT_from_errorNotify(0, Ccsds::FrameError::AOS_INVALID_VERSION);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidTfvn_SIZE(1);
}

void AosDeframerTester::testVcFrameCountGap() {
    this->configureDefault();

    U8 payload[50];
    FwSizeType sppSize = this->createSppPacket(payload, 0x001, 20);
    ComCfg::FrameContext context;

    // First valid frame initializes the tracked VC frame count and should not report a gap.
    Fw::Buffer buffer1 = this->assembleFrameBuffer(payload, sppSize, 0, ComCfg::SpacecraftId, 0, 0);
    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_errorNotify_SIZE(0);
    ASSERT_EVENTS_VcFrameCountGap_SIZE(0);

    this->clearHistory();

    // Skip VC frame count 1 to force a discontinuity.
    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload, sppSize, 0, ComCfg::SpacecraftId, 0, 2);
    this->invoke_to_dataIn(0, buffer2, context);

    ASSERT_from_dataOut_SIZE(1);  // Frame is still processed
    this->assertDataOutVcId(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // Frame buffer returned
    ASSERT_from_errorNotify_SIZE(1);
    ASSERT_from_errorNotify(0, Ccsds::FrameError::AOS_VC_FRAME_COUNT_GAP);
    ASSERT_EVENTS_VcFrameCountGap_SIZE(1);
    ASSERT_EVENTS_VcFrameCountGap(0, 0, 2, 1);
    ASSERT_TLM_LatestVcFrameCount_SIZE(1);
    ASSERT_TLM_LatestVcFrameCount(0, 2);
}

// ----------------------------------------------------------------------
// Tests - M_PDU Processing
// ----------------------------------------------------------------------

void AosDeframerTester::testFhpAtOffset() {
    this->configureDefault();

    // Create payload with junk data followed by an SPP packet
    U8 payload[150];
    const FwSizeType junkOffset = 30;
    ::memset(payload, 0xAA, junkOffset);  // Junk data (continuation)

    FwSizeType sppSize = this->createSppPacket(payload + junkOffset, 0x003, 50);

    // FHP points to where packet starts
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, junkOffset + sppSize, static_cast<U16>(junkOffset));
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), sppSize);
}

void AosDeframerTester::testFhpNoPacketStart() {
    this->configureDefault();

    // Scenario 1: FHP_NO_PACKET_START with NO active spanning packet (orphan continuation)
    // This covers the "continuation data cannot be used" path in extractPackets.
    {
        U8 orphanData[TEST_DATA_ZONE_SIZE];
        ::memset(orphanData, 0xAA, sizeof(orphanData));
        Fw::Buffer orphanFrame =
            this->assembleFrameBuffer(orphanData, TEST_DATA_ZONE_SIZE, M_PDUSubfields::FHP_NO_PACKET_START);
        ComCfg::FrameContext context;
        this->invoke_to_dataIn(0, orphanFrame, context);
        // Data silently dropped, no error events, no output
        ASSERT_from_dataOut_SIZE(0);
        ASSERT_from_errorNotify_SIZE(0);
        this->clearHistory();
    }

    // Scenario 2: FHP_NO_PACKET_START with an active spanning packet (normal continuation)
    // For TEST_FRAME_SIZE=256 with FECF: data zone = 256 - 6 - 2 - 2 = 246 bytes
    // Create a packet that spans two frames: header (6) + data (250) = 256 bytes
    U8 payload1[300];
    FwSizeType sppSize = this->createSppPacket(payload1, 0x004, 250);  // Packet that will span two frames

    // First frame - send as much of the packet as fits (up to data zone size)
    Fw::Buffer buffer1 = this->assembleFrameBuffer(payload1, TEST_DATA_ZONE_SIZE, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer1, context);

    // No complete packet yet (packet spans into next frame)
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    // Now send continuation frame with FHP = 0x7FE (no packet start)
    U8 payload2[256];
    FwSizeType remainingSize = sppSize - TEST_DATA_ZONE_SIZE;
    ::memcpy(payload2, payload1 + TEST_DATA_ZONE_SIZE, remainingSize);

    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload2, remainingSize, M_PDUSubfields::FHP_NO_PACKET_START,
                                                   ComCfg::SpacecraftId, 0, 1);

    this->invoke_to_dataIn(0, buffer2, context);

    // Should now have complete packet
    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
}

void AosDeframerTester::testFhpIdleDataOnly() {
    this->configureDefault();

    U8 payload[100];
    ::memset(payload, 0x55, sizeof(payload));  // Idle pattern

    // FHP = 0x7FF means idle data only
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sizeof(payload), M_PDUSubfields::FHP_IDLE_DATA_ONLY);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // No packets output, but frame was processed
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_IdleFrame_SIZE(1);
    ASSERT_EVENTS_IdleFrame(0, 0);  // vcId=0 (the configured VC)
    ASSERT_TLM_FramesProcessed_SIZE(1);
    ASSERT_TLM_FramesProcessed(0, 1);
}

void AosDeframerTester::testMultiplePacketsInFrame() {
    this->configureDefault();

    U8 payload[200];
    FwSizeType offset = 0;

    // Create 3 small packets
    offset += this->createSppPacket(payload + offset, 0x010, 20);
    offset += this->createSppPacket(payload + offset, 0x011, 25);
    offset += this->createSppPacket(payload + offset, 0x012, 30);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, offset, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(3);
    this->assertDataOutVcId(0);
    ASSERT_TLM_PacketsExtracted_SIZE(3);
    ASSERT_TLM_PacketsExtracted(2, 3);  // Final count is 3
}

// ----------------------------------------------------------------------
// Tests - Spanning Packets
// ----------------------------------------------------------------------

void AosDeframerTester::testSpanningPacketTwoFrames() {
    this->configureDefault();

    // Create a packet larger than one frame's data zone
    const FwSizeType packetDataLen = TEST_DATA_ZONE_SIZE + 50;  // Spans into second frame

    U8 fullPacket[512];
    FwSizeType totalPacketSize = this->createSppPacket(fullPacket, 0x020, static_cast<U16>(packetDataLen));

    // First frame - partial packet
    Fw::Buffer buffer1 = this->assembleFrameBuffer(fullPacket, TEST_DATA_ZONE_SIZE, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);  // Packet not complete yet
    this->clearHistory();

    // Second frame - rest of packet with FHP pointing to next packet
    FwSizeType remainingBytes = totalPacketSize - TEST_DATA_ZONE_SIZE;
    U8 payload2[200];
    ::memcpy(payload2, fullPacket + TEST_DATA_ZONE_SIZE, remainingBytes);

    // Add another packet after the spanning one
    FwSizeType nextPacketSize = this->createSppPacket(payload2 + remainingBytes, 0x021, 20);

    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload2, remainingBytes + nextPacketSize,
                                                   static_cast<U16>(remainingBytes), ComCfg::SpacecraftId, 0, 1);

    this->invoke_to_dataIn(0, buffer2, context);

    // Should have both packets now
    ASSERT_from_dataOut_SIZE(2);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), totalPacketSize);
}

void AosDeframerTester::testSpanningPacketFourFrames() {
    this->configureDefault();

    // Header (6) + data (900) = 906 bytes, which spans four ~246-byte data zones
    const FwSizeType packetDataLen = 900;
    U8 fullPacket[1024];
    FwSizeType totalPacketSize = this->createSppPacket(fullPacket, 0x031, static_cast<U16>(packetDataLen));

    ASSERT_TRUE(totalPacketSize > (3 * TEST_DATA_ZONE_SIZE));
    ASSERT_TRUE(totalPacketSize <= (4 * TEST_DATA_ZONE_SIZE));

    ComCfg::FrameContext context;

    Fw::Buffer buffer1 =
        this->assembleFrameBuffer(fullPacket, TEST_DATA_ZONE_SIZE, 0, ComCfg::SpacecraftId, 0, 0, 1, true);
    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    Fw::Buffer buffer2 =
        this->assembleFrameBuffer(fullPacket + TEST_DATA_ZONE_SIZE, TEST_DATA_ZONE_SIZE,
                                  M_PDUSubfields::FHP_NO_PACKET_START, ComCfg::SpacecraftId, 0, 1, 1, true);
    this->invoke_to_dataIn(0, buffer2, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    Fw::Buffer buffer3 =
        this->assembleFrameBuffer(fullPacket + (2 * TEST_DATA_ZONE_SIZE), TEST_DATA_ZONE_SIZE,
                                  M_PDUSubfields::FHP_NO_PACKET_START, ComCfg::SpacecraftId, 0, 2, 1, true);
    this->invoke_to_dataIn(0, buffer3, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    FwSizeType remainingBytes = totalPacketSize - (3 * TEST_DATA_ZONE_SIZE);
    Fw::Buffer buffer4 =
        this->assembleFrameBuffer(fullPacket + (3 * TEST_DATA_ZONE_SIZE), remainingBytes,
                                  static_cast<U16>(remainingBytes), ComCfg::SpacecraftId, 0, 3, 1, true);
    this->invoke_to_dataIn(0, buffer4, context);

    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), totalPacketSize);
}

void AosDeframerTester::testSpanningPacketContinuation() {
    this->configureDefault();

    // For spanning, packet must be larger than data zone size (246 bytes)
    // Create packet with header (6) + data (280) = 286 bytes
    const U16 packetDataLen = 280;

    U8 payload1[300];
    FwSizeType sppSize = this->createSppPacket(payload1, 0x040, packetDataLen);

    // First frame - send full data zone (partial packet)
    Fw::Buffer buffer1 = this->assembleFrameBuffer(payload1, TEST_DATA_ZONE_SIZE, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);  // Packet not complete yet
    this->clearHistory();

    // Second frame has continuation + FHP at correct offset
    U8 payload2[150];
    FwSizeType continuation = sppSize - TEST_DATA_ZONE_SIZE;
    ::memcpy(payload2, payload1 + TEST_DATA_ZONE_SIZE, continuation);

    // Add new packet after continuation
    FwSizeType nextSize = this->createSppPacket(payload2 + continuation, 0x041, 30);

    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload2, continuation + nextSize, static_cast<U16>(continuation),
                                                   ComCfg::SpacecraftId, 0, 1);

    this->invoke_to_dataIn(0, buffer2, context);

    ASSERT_from_dataOut_SIZE(2);  // Both packets
    this->assertDataOutVcId(0);
}

void AosDeframerTester::testSpanningPacketAllocFailureEvent() {
    this->configureDefault();

    U8 payload[64] = {};
    // Start an EPP packet that declares a payload large enough to exceed the test allocator buffer.
    payload[0] = (ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL << EPPSubfields::packetVersionOffset);
    payload[0] |= EppProtocolId::MissionSpecific << EPPSubfields::protocolIdOffset;
    payload[0] |= 0x02 & EPPSubfields::lengthOfLengthMask;

    payload[1] = 0x00;  // Ext Field

    payload[2] = 0xFF;
    payload[3] = 0xFF;  // dataLength = 65535 -> total packet size = 65539 (> ALLOC_BUF_SIZE=65536)

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sizeof(payload), 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAllocFailed_SIZE(1);
}

void AosDeframerTester::testSpanningPacketAbandonedOnVcGap() {
    this->configureDefault();

    // Create a packet that spans two frames (6 + 280 = 286 bytes > 246-byte data zone)
    U8 fullPacket[300];
    this->createSppPacket(fullPacket, 0x050, 280);

    ComCfg::FrameContext context;

    // Frame 0 (vcCount=0): send only the first data zone's worth — spanning in progress
    Fw::Buffer buffer1 = this->assembleFrameBuffer(fullPacket, TEST_DATA_ZONE_SIZE, 0, ComCfg::SpacecraftId, 0, 0);
    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    // Frame 2 (vcCount=2, gap): spanning packet abandoned; fresh complete packet still extracted
    U8 freshPayload[50];
    FwSizeType freshSize = this->createSppPacket(freshPayload, 0x051, 20);
    Fw::Buffer buffer2 = this->assembleFrameBuffer(freshPayload, freshSize, 0, ComCfg::SpacecraftId, 0, 2);
    this->invoke_to_dataIn(0, buffer2, context);

    ASSERT_from_dataOut_SIZE(1);  // Only the fresh packet — partial spanning packet was dropped
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), freshSize);
    ASSERT_EVENTS_VcFrameCountGap_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAbandoned_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAbandoned(0, 0, ComCfg::Pvn::SPACE_PACKET_PROTOCOL, TEST_DATA_ZONE_SIZE, 286);
}

void AosDeframerTester::testSpanningPacketMaintainedOnOIDFrame() {
    this->configureDefault();

    // Create a packet that spans two frames (6 + 280 = 286 bytes > 246-byte data zone)
    U8 fullPacket[300];
    this->createSppPacket(fullPacket, 0x060, 280);

    ComCfg::FrameContext context;

    // Frame 0 (vcCount=0): spanning in progress
    Fw::Buffer buffer1 = this->assembleFrameBuffer(fullPacket, TEST_DATA_ZONE_SIZE, 0, ComCfg::SpacecraftId, 0, 0);
    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    // Frame 1 (vcCount=1): only idle data frame — spanning packet kept
    U8 idlePayload[1] = {0};
    Fw::Buffer buffer2 = this->assembleFrameBuffer(idlePayload, sizeof(idlePayload), M_PDUSubfields::FHP_IDLE_DATA_ONLY,
                                                   ComCfg::SpacecraftId, 0, 1);
    this->invoke_to_dataIn(0, buffer2, context);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_EVENTS_IdleFrame_SIZE(1);
    ASSERT_EVENTS_IdleFrame(0, 0);
    ASSERT_EVENTS_SpanningPacketAbandoned_SIZE(0);
}

void AosDeframerTester::testSpanningPacketAbandonedOnPrematureFhp() {
    this->configureDefault();

    // Packet A: 400 bytes total (6-byte SPP header + 394-byte data), exceeds data zone by 154 bytes
    U8 packetA[400];
    this->createSppPacket(packetA, 0x070, 394);

    ComCfg::FrameContext context;

    // Frame 0 (vcCount=0): FHP=0, first 246 bytes of packet A accumulated in spanning packet
    Fw::Buffer buffer1 = this->assembleFrameBuffer(packetA, TEST_DATA_ZONE_SIZE, 0, ComCfg::SpacecraftId, 0, 0);
    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);  // Packet A incomplete
    this->clearHistory();

    // Frame 1 (vcCount=1): FHP=50, only 50 continuation bytes precede packet B
    // Packet A needs 154 more bytes but only gets 50 — spanning packet is abandoned
    U8 payload2[TEST_DATA_ZONE_SIZE];
    const FwSizeType fhp = 50;
    ::memcpy(payload2, packetA + TEST_DATA_ZONE_SIZE, fhp);  // 50 bytes of packet A's tail
    FwSizeType sizeB = this->createSppPacket(payload2 + fhp, 0x071, 20);
    Fw::Buffer buffer2 =
        this->assembleFrameBuffer(payload2, fhp + sizeB, static_cast<U16>(fhp), ComCfg::SpacecraftId, 0, 1);
    this->invoke_to_dataIn(0, buffer2, context);

    // Packet A abandoned at the FHP boundary; only packet B extracted
    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), sizeB);
    ASSERT_EVENTS_SpanningPacketAbandoned_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAbandoned(0, 0, ComCfg::Pvn::SPACE_PACKET_PROTOCOL, TEST_DATA_ZONE_SIZE + fhp, 400);
}

void AosDeframerTester::testSppHeaderSpansFrame() {
    this->configureDefault();

    // Fill most of the data zone with a complete packet, leaving only 3 bytes for the
    // next packet's header (SPP header is 6 bytes, so 3 bytes are insufficient to size it).
    U8 payload1[TEST_DATA_ZONE_SIZE];
    FwSizeType firstSize = this->createSppPacket(payload1, 0x072, 237);  // 6 + 237 = 243 bytes

    // Second packet: first 3 bytes go in frame 0, last 3 header bytes + data go in frame 1
    U8 secondPacket[50];
    FwSizeType secondSize = this->createSppPacket(secondPacket, 0x073, 20);  // 26 bytes
    const FwSizeType splitAt = TEST_DATA_ZONE_SIZE - firstSize;              // = 3
    ::memcpy(payload1 + firstSize, secondPacket, splitAt);

    ComCfg::FrameContext context;

    // Frame 0: first packet complete + 3-byte partial SPP header
    Fw::Buffer buffer1 = this->assembleFrameBuffer(payload1, TEST_DATA_ZONE_SIZE, 0, ComCfg::SpacecraftId, 0, 0);
    this->invoke_to_dataIn(0, buffer1, context);

    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), firstSize);
    this->clearHistory();

    // Frame 1: remaining 23 bytes of second packet (3 header + 20 data)
    Fw::Buffer buffer2 = this->assembleFrameBuffer(secondPacket + splitAt, secondSize - splitAt,
                                                   M_PDUSubfields::FHP_NO_PACKET_START, ComCfg::SpacecraftId, 0, 1);
    this->invoke_to_dataIn(0, buffer2, context);

    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), secondSize);
}

void AosDeframerTester::testEppHeaderSpansFrame() {
    this->configureDefault();

    // Fill most of the data zone with a complete packet, leaving only 2 bytes for the
    // next packet's header (EPP lol=2 header is 4 bytes, so 2 bytes are insufficient to size it).
    U8 payload1[TEST_DATA_ZONE_SIZE];
    FwSizeType firstSize = this->createSppPacket(payload1, 0x074, 238);  // 6 + 238 = 244 bytes

    // EPP packet lol=2: 1-byte first field + 1-byte extension + 2-byte length + 20 data = 24 bytes
    U8 eppPacket[50];
    FwSizeType eppSize = this->createEppPacket(eppPacket, EppProtocolId::MissionSpecific, EppLengthOfLength::Two, 20);
    const FwSizeType splitAt = TEST_DATA_ZONE_SIZE - firstSize;  // = 2
    ::memcpy(payload1 + firstSize, eppPacket, splitAt);

    ComCfg::FrameContext context;

    // Frame 0: SPP complete + 2-byte partial EPP header
    Fw::Buffer buffer1 = this->assembleFrameBuffer(payload1, TEST_DATA_ZONE_SIZE, 0, ComCfg::SpacecraftId, 0, 0);
    this->invoke_to_dataIn(0, buffer1, context);

    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), firstSize);
    this->clearHistory();

    // Frame 1: remaining 22 bytes of EPP packet (2 length bytes + 20 data bytes)
    Fw::Buffer buffer2 = this->assembleFrameBuffer(eppPacket + splitAt, eppSize - splitAt,
                                                   M_PDUSubfields::FHP_NO_PACKET_START, ComCfg::SpacecraftId, 0, 1);
    this->invoke_to_dataIn(0, buffer2, context);

    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), eppSize);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_pvn(), ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
}

void AosDeframerTester::testAllocFailureNextPacketExtracted() {
    this->configureDefault();

    U8 payload[100];
    FwSizeType offset = 0;

    // Packet A: alloc will be forced to fail
    FwSizeType sizeA = this->createSppPacket(payload + offset, 0x080, 20);
    offset += sizeA;

    // Packet B: should be extracted cleanly after A's failure
    FwSizeType sizeB = this->createSppPacket(payload + offset, 0x081, 20);
    offset += sizeB;

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, offset, 0);
    ComCfg::FrameContext context;

    m_failNextAlloc = true;
    this->invoke_to_dataIn(0, buffer, context);

    // Packet A was dropped; packet B extracted cleanly
    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), sizeB);
    ASSERT_EVENTS_SpanningPacketAllocFailed_SIZE(1);
}

// ----------------------------------------------------------------------
// Tests - SPP Extraction
// ----------------------------------------------------------------------

void AosDeframerTester::testSppIdlePacketFiltering() {
    this->configureDefault();

    U8 payload[200];
    FwSizeType offset = 0;

    // Real packet
    FwSizeType sppSize0 = this->createSppPacket(payload + offset, 0x101, 20);
    offset += sppSize0;

    // Another real packet
    FwSizeType sppSize1 = this->createSppPacket(payload + offset, 0x102, 25);
    offset += sppSize1;

    // Idle packet (APID 0x7FF)
    offset += this->createSppPacket(payload + offset, ComCfg::Apid::SPP_IDLE_PACKET, 30);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, offset, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // Only 2 packets output (idle filtered)
    ASSERT_from_dataOut_SIZE(2);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), sppSize0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_pvn(), ComCfg::Pvn::SPACE_PACKET_PROTOCOL);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(1).data.getSize(), sppSize1);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(1).context.get_pvn(), ComCfg::Pvn::SPACE_PACKET_PROTOCOL);
}

// ----------------------------------------------------------------------
// Tests - EPP Extraction
// ----------------------------------------------------------------------

void AosDeframerTester::testEppExtraction() {
    const U8 testVcId = 11;
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, testVcId,
                              PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK);

    U8 payload[100];
    FwSizeType eppSize =
        this->createEppPacket(payload, EppProtocolId::MissionSpecific, EppLengthOfLength::One, 50);  // Protocol ID 2

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, eppSize, 0, ComCfg::SpacecraftId, testVcId);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(testVcId);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), eppSize);
    ComCfg::FrameContext outContext = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(outContext.get_pvn(), ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
}

void AosDeframerTester::testEppLengthOfLength() {
    const U8 testVcId = 11;
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, testVcId,
                              PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK);

    // lol=1: 1 first byte + 1 length byte + 11 data = 13 bytes
    U8 packet1[13];
    FwSizeType size1 = this->createEppPacket(packet1, EppProtocolId::MissionSpecific, EppLengthOfLength::One, 11);

    // lol=2: 1 first byte + 1 extension byte + 2 length bytes + 277 data = 281 bytes
    U8 packet2[281];
    FwSizeType size2 = this->createEppPacket(packet2, EppProtocolId::MissionSpecific, EppLengthOfLength::Two, 277);

    // lol=4: 1 first byte + 1 extension byte + 2 reserved bytes + 4 length bytes + 1367 data = 1375 bytes
    U8 packet4[1375];
    FwSizeType size4 = this->createEppPacket(packet4, EppProtocolId::MissionSpecific, EppLengthOfLength::Four, 1367);

    ComCfg::FrameContext context;
    U32 vcCount = 0;

    // --- lol=1: fits in a single frame ---
    Fw::Buffer frame0 = this->assembleFrameBuffer(packet1, size1, 0, ComCfg::SpacecraftId, testVcId, vcCount++);
    this->invoke_to_dataIn(0, frame0, context);
    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(testVcId);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), size1);
    this->clearHistory();

    // --- lol=2: spans two frames (246 + 35 bytes) ---
    Fw::Buffer frame1 =
        this->assembleFrameBuffer(packet2, TEST_DATA_ZONE_SIZE, 0, ComCfg::SpacecraftId, testVcId, vcCount++);
    this->invoke_to_dataIn(0, frame1, context);
    ASSERT_from_dataOut_SIZE(0);

    FwSizeType lol2Remaining = size2 - TEST_DATA_ZONE_SIZE;
    Fw::Buffer frame2 =
        this->assembleFrameBuffer(packet2 + TEST_DATA_ZONE_SIZE, lol2Remaining, M_PDUSubfields::FHP_NO_PACKET_START,
                                  ComCfg::SpacecraftId, testVcId, vcCount++);
    this->invoke_to_dataIn(0, frame2, context);
    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(testVcId);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), size2);
    this->clearHistory();

    // --- lol=4: spans six frames (246 bytes x5 + 145 bytes) ---
    Fw::Buffer frame3 =
        this->assembleFrameBuffer(packet4, TEST_DATA_ZONE_SIZE, 0, ComCfg::SpacecraftId, testVcId, vcCount++);
    this->invoke_to_dataIn(0, frame3, context);
    ASSERT_from_dataOut_SIZE(0);

    for (FwSizeType i = 1; i <= 4; i++) {
        Fw::Buffer frameN =
            this->assembleFrameBuffer(packet4 + i * TEST_DATA_ZONE_SIZE, TEST_DATA_ZONE_SIZE,
                                      M_PDUSubfields::FHP_NO_PACKET_START, ComCfg::SpacecraftId, testVcId, vcCount++);
        this->invoke_to_dataIn(0, frameN, context);
        ASSERT_from_dataOut_SIZE(0);
    }

    FwSizeType lol4Remaining = size4 - 5 * TEST_DATA_ZONE_SIZE;
    Fw::Buffer frame8 =
        this->assembleFrameBuffer(packet4 + 5 * TEST_DATA_ZONE_SIZE, lol4Remaining, M_PDUSubfields::FHP_NO_PACKET_START,
                                  ComCfg::SpacecraftId, testVcId, vcCount++);
    this->invoke_to_dataIn(0, frame8, context);
    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(testVcId);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), size4);
}

void AosDeframerTester::testEppIdlePacket() {
    this->configureDefault();

    U8 payload[100];
    FwSizeType offset = 0;

    // Real SPP packet
    offset += this->createSppPacket(payload + offset, 0x104, 20);

    // Another real packet
    offset += this->createSppPacket(payload + offset, 0x105, 15);

    // EPP idle packet with length
    offset += this->createEppPacket(payload + offset, EppProtocolId::Idle, EppLengthOfLength::Two, 10);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, offset, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // Only 2 packets (EPP idle filtered)
    ASSERT_from_dataOut_SIZE(2);
    this->assertDataOutVcId(0);
}

void AosDeframerTester::testEppFillPacket() {
    this->configureDefault();

    U8 payload[150];
    FwSizeType offset = 0;

    // Real packet
    offset += this->createSppPacket(payload + offset, 0x106, 30);

    // EPP fill packet (length of length = 0) - consumes rest
    offset += this->createEppPacket(payload + offset, EppProtocolId::Idle, EppLengthOfLength::Zero, 0);

    // Fill rest with pattern
    ::memset(payload + offset, 0x55, sizeof(payload) - offset);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sizeof(payload) - 50, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // Only 1 packet (fill consumed rest)
    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
}

void AosDeframerTester::testInvalidPvnVersion() {
    this->configureDefault();

    U8 payload[100];

    // Create a packet with unrecognized PVN (not 0/SPP and not 7/EPP)
    // PVN=3 (0b011) means first byte upper 3 bits = 011
    // 0x60 = 0b01100000 -> PVN = 3
    payload[0] = 0x60 | 0x02;  // Version 3, Protocol ID 2
    payload[1] = 0x00;         // Length high byte
    payload[2] = 0x10;         // Length low byte (16)
    ::memset(payload + 3, 0xAA, 16);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, 19, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // Unrecognized PVN should result in no packets output and an explicit event.
    ASSERT_from_dataOut_SIZE(0);
    // Frame should still be returned
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EVENTS_DisabledPvn_SIZE(1);
    // Telemetry should still be updated for frame count
    ASSERT_TLM_FramesProcessed_SIZE(1);
}

// ----------------------------------------------------------------------
// Tests - Configuration
// ----------------------------------------------------------------------

void AosDeframerTester::testFecfDisabled() {
    // When FECF is disabled, frame size is reduced by trailer size
    const U32 frameSizeNoFecf = TEST_FRAME_SIZE - AOSTrailer::SERIALIZED_SIZE;

    // Configure without FECF
    this->component.configure(frameSizeNoFecf, false, ComCfg::SpacecraftId, 0);

    U8 payload[100];
    FwSizeType sppSize = this->createSppPacket(payload, 0x200, 50);

    // Assemble frame without CRC
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0, ComCfg::SpacecraftId, 0, 0, 1, false);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    // No FECF error events
    ASSERT_EVENTS_InvalidFecf_SIZE(0);
}

void AosDeframerTester::testPvnMaskSppOnly() {
    // Configure for SPP only
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0, PvnBitfield::SPP_MASK);

    U8 payload[150];
    FwSizeType offset = 0;

    // SPP packet - should be extracted
    offset += this->createSppPacket(payload + offset, 0x201, 20);

    // EPP packet - should be ignored
    offset += this->createEppPacket(payload + offset, 0x02, EppLengthOfLength::One, 20);

    // Another SPP
    offset += this->createSppPacket(payload + offset, 0x202, 15);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, offset, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // Only the first SPP extracted; EPP triggers DisabledPvn and stops extraction
    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EVENTS_DisabledPvn_SIZE(1);
    ASSERT_EVENTS_DisabledPvn(0, 0, ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
}

void AosDeframerTester::testPvnMaskEppOnly() {
    // Configure for EPP only
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0, PvnBitfield::EPP_MASK);

    U8 payload[100];
    FwSizeType eppSize = this->createEppPacket(payload, EppProtocolId::MissionSpecific, EppLengthOfLength::One, 30);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, eppSize, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    this->assertDataOutVcId(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_pvn(), ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
}

// ----------------------------------------------------------------------
// Tests - Telemetry
// ----------------------------------------------------------------------

void AosDeframerTester::testFrameCountTelemetry() {
    const U8 testVcId = 5;
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, testVcId,
                              PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK);

    U8 payload[50];
    FwSizeType sppSize = this->createSppPacket(payload, 0x300, 20);

    ComCfg::FrameContext context;

    // Send 3 frames with incrementing vcCount to avoid gap detection
    for (U32 i = 0; i < 3; i++) {
        this->clearHistory();
        Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0, ComCfg::SpacecraftId, testVcId, i);
        this->invoke_to_dataIn(0, buffer, context);
        this->assertDataOutVcId(testVcId);
        ASSERT_TLM_FramesProcessed(0, i + 1);
        ASSERT_TLM_PacketsExtracted(0, i + 1);
        ASSERT_TLM_LatestVcFrameCount(0, i);
    }
}

// ----------------------------------------------------------------------
// Tests - Robustness against untrusted inputs
// ----------------------------------------------------------------------

void AosDeframerTester::testUntrustedFhp() {
    this->configureDefault();

    U8 payload[50];  // don't care about payload content for this test
    // Create a frame with FHP pointing further than the frame's data zone
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sizeof(payload), TEST_DATA_ZONE_SIZE);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EVENTS_InvalidFhp_SIZE(1);
}

}  // namespace Ccsds

}  // namespace Svc
