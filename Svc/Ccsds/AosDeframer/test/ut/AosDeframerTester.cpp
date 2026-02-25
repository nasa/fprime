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

// ----------------------------------------------------------------------
// Tests - Basic Validation
// ----------------------------------------------------------------------

void AosDeframerTester::testNominalDeframing() {
    this->configureDefault();

    // Create a simple SPP packet
    U8 payload[100];
    FwSizeType sppSize = this->createSppPacket(payload, 0x001, 50);  // APID 1, 50 bytes data

    // Assemble frame with FHP=0
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0);
    ComCfg::FrameContext context;

    // Invoke the deframer
    this->invoke_to_dataIn(0, buffer, context);

    // Should output one packet
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(1);  // Frame buffer returned

    // Verify packet content
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), sppSize);

    // Verify telemetry
    ASSERT_TLM_SIZE(3);  // LatestVcFrameCount, FramesProcessed, and PacketsExtracted
    ASSERT_TLM_FramesProcessed_SIZE(1);
    ASSERT_TLM_FramesProcessed(0, 1);
    ASSERT_TLM_PacketsExtracted_SIZE(1);
    ASSERT_TLM_PacketsExtracted(0, 1);
}

void AosDeframerTester::testDataReturn() {
    this->configureDefault();

    U8 data[1] = {0};
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext context;

    this->invoke_to_dataReturnIn(0, buffer, context);

    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
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

// testAcceptAllVcid removed: accept-all-VCID mode is not supported.
// Each VC struct maps to exactly one VCID for per-packet spanning state tracking.

// ----------------------------------------------------------------------
// Tests - M_PDU Processing
// ----------------------------------------------------------------------

void AosDeframerTester::testFhpAtZero() {
    this->configureDefault();

    U8 payload[100];
    FwSizeType sppSize = this->createSppPacket(payload, 0x002, 50);

    // FHP = 0 means first packet starts at beginning of data zone
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), sppSize);
}

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
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), sppSize);
}

void AosDeframerTester::testFhpNoPacketStart() {
    this->configureDefault();

    // For TEST_FRAME_SIZE=256 with FECF: data zone = 256 - 6 - 2 - 2 = 246 bytes
    // Create a packet that spans two frames: header (6) + data (250) = 256 bytes
    U8 payload1[300];
    FwSizeType sppSize = this->createSppPacket(payload1, 0x004, 250);  // Packet that will span two frames

    // Calculate data zone size
    const FwSizeType dataZoneSize =
        TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;

    // First frame - send as much of the packet as fits (up to data zone size)
    Fw::Buffer buffer1 = this->assembleFrameBuffer(payload1, dataZoneSize, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer1, context);

    // No complete packet yet (packet spans into next frame)
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    // Now send continuation frame with FHP = 0x7FE (no packet start)
    U8 payload2[256];
    FwSizeType remainingSize = sppSize - dataZoneSize;
    ::memcpy(payload2, payload1 + dataZoneSize, remainingSize);

    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload2, remainingSize, M_PDUSubfields::FHP_NO_PACKET_START);

    this->invoke_to_dataIn(0, buffer2, context);

    // Should now have complete packet
    ASSERT_from_dataOut_SIZE(1);
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
    ASSERT_TLM_PacketsExtracted_SIZE(3);
    ASSERT_TLM_PacketsExtracted(2, 3);  // Final count is 3
}

// ----------------------------------------------------------------------
// Tests - Spanning Packets
// ----------------------------------------------------------------------

void AosDeframerTester::testSpanningPacketTwoFrames() {
    this->configureDefault();

    // Create a packet larger than one frame's data zone
    const FwSizeType dataZoneSize =
        TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;
    const FwSizeType packetDataLen = dataZoneSize + 50;  // Spans into second frame

    U8 fullPacket[512];
    FwSizeType totalPacketSize = this->createSppPacket(fullPacket, 0x020, static_cast<U16>(packetDataLen));

    // First frame - partial packet
    Fw::Buffer buffer1 = this->assembleFrameBuffer(fullPacket, dataZoneSize, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);  // Packet not complete yet
    this->clearHistory();

    // Second frame - rest of packet with FHP pointing to next packet
    FwSizeType remainingBytes = totalPacketSize - dataZoneSize;
    U8 payload2[200];
    ::memcpy(payload2, fullPacket + dataZoneSize, remainingBytes);

    // Add another packet after the spanning one
    FwSizeType nextPacketSize = this->createSppPacket(payload2 + remainingBytes, 0x021, 20);

    Fw::Buffer buffer2 =
        this->assembleFrameBuffer(payload2, remainingBytes + nextPacketSize, static_cast<U16>(remainingBytes));

    this->invoke_to_dataIn(0, buffer2, context);

    // Should have both packets now
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), totalPacketSize);
}

void AosDeframerTester::testSpanningPacketMultipleFrames() {
    // Use default frame size for this test
    // Spanning packet buffer is ComCfg::AosMaxFrameFixedSize = 1536 bytes
    // Use a packet that fits in that but spans 3 smaller frames
    this->configureDefault();

    const FwSizeType dataZoneSize =
        TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;

    // Create a packet that spans 3 frames but fits in spanning buffer
    // Each frame data zone ~246 bytes, so use packet ~600 bytes
    // Total packet = header (6) + data (600) = 606 bytes < 1536
    const FwSizeType packetDataLen = 600;
    U8 fullPacket[700];
    FwSizeType totalPacketSize = this->createSppPacket(fullPacket, 0x030, static_cast<U16>(packetDataLen));

    ComCfg::FrameContext context;

    // First frame - first ~246 bytes of packet
    Fw::Buffer buffer1 = this->assembleFrameBuffer(fullPacket, dataZoneSize, 0, ComCfg::SpacecraftId, 0, 0, 1, true);
    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    // Second frame - continuation only
    Fw::Buffer buffer2 =
        this->assembleFrameBuffer(fullPacket + dataZoneSize, dataZoneSize, M_PDUSubfields::FHP_NO_PACKET_START,
                                  ComCfg::SpacecraftId, 0, 1, 1, true);
    this->invoke_to_dataIn(0, buffer2, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    // Third frame - rest of packet
    FwSizeType remainingBytes = totalPacketSize - (2 * dataZoneSize);
    Fw::Buffer buffer3 =
        this->assembleFrameBuffer(fullPacket + (2 * dataZoneSize), remainingBytes, static_cast<U16>(remainingBytes),
                                  ComCfg::SpacecraftId, 0, 2, 1, true);
    this->invoke_to_dataIn(0, buffer3, context);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), totalPacketSize);
}

void AosDeframerTester::testSpanningPacketFourFrames() {
    this->configureDefault();

    const FwSizeType dataZoneSize =
        TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;

    // Header (6) + data (900) = 906 bytes, which spans four ~246-byte data zones
    const FwSizeType packetDataLen = 900;
    U8 fullPacket[1024];
    FwSizeType totalPacketSize = this->createSppPacket(fullPacket, 0x031, static_cast<U16>(packetDataLen));

    ASSERT_TRUE(totalPacketSize > (3 * dataZoneSize));
    ASSERT_TRUE(totalPacketSize <= (4 * dataZoneSize));

    ComCfg::FrameContext context;

    Fw::Buffer buffer1 = this->assembleFrameBuffer(fullPacket, dataZoneSize, 0, ComCfg::SpacecraftId, 0, 0, 1, true);
    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    Fw::Buffer buffer2 =
        this->assembleFrameBuffer(fullPacket + dataZoneSize, dataZoneSize, M_PDUSubfields::FHP_NO_PACKET_START,
                                  ComCfg::SpacecraftId, 0, 1, 1, true);
    this->invoke_to_dataIn(0, buffer2, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    Fw::Buffer buffer3 =
        this->assembleFrameBuffer(fullPacket + (2 * dataZoneSize), dataZoneSize, M_PDUSubfields::FHP_NO_PACKET_START,
                                  ComCfg::SpacecraftId, 0, 2, 1, true);
    this->invoke_to_dataIn(0, buffer3, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    FwSizeType remainingBytes = totalPacketSize - (3 * dataZoneSize);
    Fw::Buffer buffer4 =
        this->assembleFrameBuffer(fullPacket + (3 * dataZoneSize), remainingBytes, static_cast<U16>(remainingBytes),
                                  ComCfg::SpacecraftId, 0, 3, 1, true);
    this->invoke_to_dataIn(0, buffer4, context);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), totalPacketSize);
}

void AosDeframerTester::testSpanningPacketContinuation() {
    this->configureDefault();

    // For spanning, packet must be larger than data zone size (246 bytes)
    // Create packet with header (6) + data (280) = 286 bytes
    const FwSizeType dataZoneSize =
        TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;
    const U16 packetDataLen = 280;

    U8 payload1[300];
    FwSizeType sppSize = this->createSppPacket(payload1, 0x040, packetDataLen);

    // First frame - send full data zone (partial packet)
    Fw::Buffer buffer1 = this->assembleFrameBuffer(payload1, dataZoneSize, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);  // Packet not complete yet
    this->clearHistory();

    // Second frame has continuation + FHP at correct offset
    U8 payload2[150];
    FwSizeType continuation = sppSize - dataZoneSize;
    ::memcpy(payload2, payload1 + dataZoneSize, continuation);

    // Add new packet after continuation
    FwSizeType nextSize = this->createSppPacket(payload2 + continuation, 0x041, 30);

    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload2, continuation + nextSize, static_cast<U16>(continuation));

    this->invoke_to_dataIn(0, buffer2, context);

    ASSERT_from_dataOut_SIZE(2);  // Both packets
}

// ----------------------------------------------------------------------
// Tests - SPP Extraction
// ----------------------------------------------------------------------

void AosDeframerTester::testSppExtraction() {
    this->configureDefault();

    U8 payload[100];
    FwSizeType sppSize = this->createSppPacket(payload, 0x100, 50);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    ComCfg::FrameContext outContext = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(outContext.get_pvn(), ComCfg::Pvn::SPACE_PACKET_PROTOCOL);
    ASSERT_EQ(outContext.get_apid(), static_cast<ComCfg::Apid::T>(0x100));
}

void AosDeframerTester::testSppIdlePacketFiltering() {
    this->configureDefault();

    U8 payload[200];
    FwSizeType offset = 0;

    // Real packet
    offset += this->createSppPacket(payload + offset, 0x101, 20);

    // Idle packet (APID 0x7FF)
    offset += this->createSppPacket(payload + offset, 0x7FF, 30);

    // Another real packet
    offset += this->createSppPacket(payload + offset, 0x102, 25);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, offset, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // Only 2 packets output (idle filtered)
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_apid(), static_cast<ComCfg::Apid::T>(0x101));
    ASSERT_EQ(this->fromPortHistory_dataOut->at(1).context.get_apid(), static_cast<ComCfg::Apid::T>(0x102));
}

void AosDeframerTester::testSppSequenceCount() {
    this->configureDefault();

    U8 payload[100];
    const U16 seqCount = 0x1234;
    FwSizeType sppSize = this->createSppPacket(payload, 0x103, 20, seqCount);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_sequenceCount(), seqCount);
}

// ----------------------------------------------------------------------
// Tests - EPP Extraction
// ----------------------------------------------------------------------

void AosDeframerTester::testEppExtraction() {
    this->configureDefault();

    U8 payload[100];
    FwSizeType eppSize = this->createEppPacket(payload, 0x02, 50);  // Protocol ID 2

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, eppSize, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    ComCfg::FrameContext outContext = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(outContext.get_pvn(), ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
}

void AosDeframerTester::testEppIdlePacket() {
    this->configureDefault();

    U8 payload[100];
    FwSizeType offset = 0;

    // Real SPP packet
    offset += this->createSppPacket(payload + offset, 0x104, 20);

    // EPP idle packet with length
    offset += this->createEppIdlePacket(payload + offset, 2, 10);

    // Another real packet
    offset += this->createSppPacket(payload + offset, 0x105, 15);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, offset, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // Only 2 packets (EPP idle filtered)
    ASSERT_from_dataOut_SIZE(2);
}

void AosDeframerTester::testEppFillPacket() {
    this->configureDefault();

    U8 payload[150];
    FwSizeType offset = 0;

    // Real packet
    offset += this->createSppPacket(payload + offset, 0x106, 30);

    // EPP fill packet (length of length = 0) - consumes rest
    offset += this->createEppIdlePacket(payload + offset, 0, 0);

    // Fill rest with pattern
    ::memset(payload + offset, 0x55, sizeof(payload) - offset);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sizeof(payload) - 50, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // Only 1 packet (fill consumed rest)
    ASSERT_from_dataOut_SIZE(1);
}

void AosDeframerTester::testInvalidEppVersion() {
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

    // Unrecognized PVN should result in no packets output
    // (extraction stops when encountering unknown packet type)
    ASSERT_from_dataOut_SIZE(0);
    // Frame should still be returned
    ASSERT_from_dataReturnOut_SIZE(1);
    // Telemetry should still be updated for frame count
    ASSERT_TLM_FramesProcessed_SIZE(1);
}

void AosDeframerTester::testHeaderDeserializeFailureHelperPath() {
    this->configureDefault();
    this->clearHistory();

    // Direct helper call to cover the deserialize failure branch, which is preempted by
    // dataIn_handler's outer frame-length guard in normal port-driven execution.
    U8 shortFrame[AOSHeader::SERIALIZED_SIZE - 1] = {};
    Fw::Buffer buffer(shortFrame, sizeof(shortFrame));
    ComCfg::FrameContext context;

    const bool ok = this->component.parseAndValidateHeader(buffer, context);

    ASSERT_FALSE(ok);
    ASSERT_from_errorNotify_SIZE(1);
    ASSERT_from_errorNotify(0, Ccsds::FrameError::AOS_INVALID_LENGTH);
    ASSERT_EVENTS_InvalidFrameLength_SIZE(1);
}

void AosDeframerTester::testExtractorGuardPaths() {
    this->configureDefault();
    this->clearHistory();

    AosDeframer::AosDeframerVc& vc = this->component.m_vcs[0];
    ComCfg::FrameContext context;

    U8 sppShortHeader[SpacePacketHeader::SERIALIZED_SIZE - 1] = {};
    ASSERT_EQ(this->component.extractSppPacket(vc, sppShortHeader, sizeof(sppShortHeader), context), 0);

    U8 dummy = 0;
    ASSERT_EQ(this->component.extractEppPacket(vc, &dummy, 0, context), 0);

    // Idle EPP with length-of-length=2 but only one byte available
    U8 eppIdleShortLengthField[1] = {static_cast<U8>((7 << 5) | (1 << 4) | 0x02)};
    ASSERT_EQ(this->component.extractEppPacket(vc, eppIdleShortLengthField, sizeof(eppIdleShortLengthField), context),
              0);

    // Idle EPP with complete length field but truncated idle payload
    U8 eppIdleTruncatedPayload[3] = {static_cast<U8>((7 << 5) | (1 << 4) | 0x02), 0x00, 0x04};
    ASSERT_EQ(this->component.extractEppPacket(vc, eppIdleTruncatedPayload, sizeof(eppIdleTruncatedPayload), context),
              0);

    // Standard EPP (protocol ID <= 0x07) with truncated header
    U8 eppStandardShortHeader[2] = {static_cast<U8>((7 << 5) | 0x02), 0x00};
    ASSERT_EQ(this->component.extractEppPacket(vc, eppStandardShortHeader, sizeof(eppStandardShortHeader), context), 0);

    // Extended EPP (protocol ID >= 0x08) with truncated header
    U8 eppExtendedShortHeader[2] = {static_cast<U8>((7 << 5) | 0x08), 0x00};
    ASSERT_EQ(this->component.extractEppPacket(vc, eppExtendedShortHeader, sizeof(eppExtendedShortHeader), context), 0);

    // Standard EPP with declared payload larger than bytes available
    U8 eppStandardIncompletePacket[3] = {static_cast<U8>((7 << 5) | 0x02), 0x00, 0x02};
    ASSERT_EQ(
        this->component.extractEppPacket(vc, eppStandardIncompletePacket, sizeof(eppStandardIncompletePacket), context),
        0);

    ASSERT_from_dataOut_SIZE(0);
}

void AosDeframerTester::testExtendedEppProtocolBranch() {
    this->configureDefault();
    this->clearHistory();

    AosDeframer::AosDeframerVc& vc = this->component.m_vcs[0];
    ComCfg::FrameContext context;

    // Extended protocol ID (0x8-0xF) uses the alternate branch in extractEppPacket.
    U8 eppExtendedPacket[3] = {static_cast<U8>((7 << 5) | 0x08), 0x00, 0x00};
    FwSizeType consumed = this->component.extractEppPacket(vc, eppExtendedPacket, sizeof(eppExtendedPacket), context);

    ASSERT_EQ(consumed, static_cast<FwSizeType>(sizeof(eppExtendedPacket)));
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_pvn(), ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
    ASSERT_TLM_PacketsExtracted_SIZE(1);
}

void AosDeframerTester::testAppendToSpanningPacketEppCompletion() {
    this->configureDefault();
    this->clearHistory();

    AosDeframer::AosDeframerVc& vc = this->component.m_vcs[0];
    vc.spanningPacket.active = true;
    vc.spanningPacket.pvn = static_cast<U8>(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
    vc.spanningPacket.bytesReceived = 2;
    vc.spanningPacket.expectedSize = 4;
    vc.spanningPacket.buffer[0] = static_cast<U8>((7 << 5) | 0x08);
    vc.spanningPacket.buffer[1] = 0x00;

    U8 tail[2] = {0x00, 0xAA};
    ComCfg::FrameContext context;
    const bool complete = this->component.appendToSpanningPacket(vc, tail, sizeof(tail), context);

    ASSERT_TRUE(complete);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), static_cast<FwSizeType>(4));
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_pvn(), ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
    ASSERT_FALSE(vc.spanningPacket.active);
    ASSERT_EQ(vc.spanningPacket.bytesReceived, static_cast<FwSizeType>(0));
    ASSERT_EQ(vc.spanningPacket.expectedSize, static_cast<FwSizeType>(0));
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
    offset += this->createEppPacket(payload + offset, 0x02, 20);

    // Another SPP
    offset += this->createSppPacket(payload + offset, 0x202, 15);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, offset, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // Only SPP packets extracted (EPP skipped but may stop extraction)
    ASSERT_from_dataOut_SIZE(1);
}

void AosDeframerTester::testPvnMaskEppOnly() {
    // Configure for EPP only
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0, PvnBitfield::EPP_MASK);

    U8 payload[100];
    FwSizeType eppSize = this->createEppPacket(payload, 0x02, 30);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, eppSize, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_pvn(), ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
}

// ----------------------------------------------------------------------
// Tests - Telemetry
// ----------------------------------------------------------------------

void AosDeframerTester::testFrameCountTelemetry() {
    this->configureDefault();

    U8 payload[50];
    FwSizeType sppSize = this->createSppPacket(payload, 0x300, 20);

    ComCfg::FrameContext context;

    // Send 3 frames
    for (U32 i = 0; i < 3; i++) {
        this->clearHistory();
        Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0);
        this->invoke_to_dataIn(0, buffer, context);
        ASSERT_TLM_FramesProcessed(0, i + 1);
    }
}

void AosDeframerTester::testPacketCountTelemetry() {
    this->configureDefault();

    U8 payload[200];
    FwSizeType offset = 0;
    offset += this->createSppPacket(payload + offset, 0x301, 20);
    offset += this->createSppPacket(payload + offset, 0x302, 25);
    offset += this->createSppPacket(payload + offset, 0x303, 30);

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, offset, 0);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_TLM_PacketsExtracted_SIZE(3);
    ASSERT_TLM_PacketsExtracted(2, 3);  // Final value
}

void AosDeframerTester::testCrcErrorCountTelemetry() {
    this->configureDefault();

    U8 payload[50];
    FwSizeType sppSize = this->createSppPacket(payload, 0x304, 20);

    ComCfg::FrameContext context;

    // Send 2 frames with bad CRC
    for (U32 i = 0; i < 2; i++) {
        this->clearHistory();
        Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0);
        buffer.getData()[TEST_FRAME_SIZE - 1] ^= 0xFF;  // Corrupt CRC
        this->invoke_to_dataIn(0, buffer, context);
        ASSERT_TLM_CrcErrorCount(0, i + 1);
    }
}

}  // namespace Ccsds

}  // namespace Svc
