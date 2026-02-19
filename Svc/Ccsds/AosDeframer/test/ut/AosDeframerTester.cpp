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
    ASSERT_TLM_SIZE(2);  // FrameCount and PacketCount
    ASSERT_TLM_FrameCount_SIZE(1);
    ASSERT_TLM_FrameCount(0, 1);
    ASSERT_TLM_PacketCount_SIZE(1);
    ASSERT_TLM_PacketCount(0, 1);
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
    // Configure to accept only VCID 0
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0, false);

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

void AosDeframerTester::testInvalidCrc() {
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
    ASSERT_EVENTS_InvalidCrc_SIZE(1);
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

void AosDeframerTester::testAcceptAllVcid() {
    // Configure to accept all VCIDs
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0, true);

    U8 payload[50];
    FwSizeType sppSize = this->createSppPacket(payload, 0x001, 20);

    // Test with various VCIDs
    for (U8 vcid = 0; vcid < 4; vcid++) {
        this->clearHistory();
        Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0, ComCfg::SpacecraftId, vcid);
        ComCfg::FrameContext context;

        this->invoke_to_dataIn(0, buffer, context);

        ASSERT_from_dataOut_SIZE(1);
        ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_vcId(), vcid);
    }
}

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
    const FwSizeType dataZoneSize = TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE -
                                     M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;

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

    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload2, remainingSize, AOSMPDUSubfields::FHP_NO_PACKET_START);

    this->invoke_to_dataIn(0, buffer2, context);

    // Should now have complete packet
    ASSERT_from_dataOut_SIZE(1);
}

void AosDeframerTester::testFhpIdleDataOnly() {
    this->configureDefault();

    U8 payload[100];
    ::memset(payload, 0x55, sizeof(payload));  // Idle pattern

    // FHP = 0x7FF means idle data only
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sizeof(payload), AOSMPDUSubfields::FHP_IDLE_DATA_ONLY);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    // No packets output, but frame was processed
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_IdleFrame_SIZE(1);
    ASSERT_TLM_FrameCount_SIZE(1);
    ASSERT_TLM_FrameCount(0, 1);
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
    ASSERT_TLM_PacketCount_SIZE(3);
    ASSERT_TLM_PacketCount(2, 3);  // Final count is 3
}

// ----------------------------------------------------------------------
// Tests - Spanning Packets
// ----------------------------------------------------------------------

void AosDeframerTester::testSpanningPacketTwoFrames() {
    this->configureDefault();

    // Create a packet larger than one frame's data zone
    const FwSizeType dataZoneSize = TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE -
                                     M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;
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

    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload2, remainingBytes + nextPacketSize,
                                                    static_cast<U16>(remainingBytes));

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

    const FwSizeType dataZoneSize = TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE -
                                     M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;

    // Create a packet that spans 3 frames but fits in spanning buffer
    // Each frame data zone ~246 bytes, so use packet ~600 bytes
    // Total packet = header (6) + data (600) = 606 bytes < 1536
    const FwSizeType packetDataLen = 600;
    U8 fullPacket[700];
    FwSizeType totalPacketSize = this->createSppPacket(fullPacket, 0x030, static_cast<U16>(packetDataLen));

    ComCfg::FrameContext context;

    // First frame - first ~246 bytes of packet
    Fw::Buffer buffer1 = this->assembleFrameBuffer(fullPacket, dataZoneSize, 0,
                                                    ComCfg::SpacecraftId, 0, 0, 1, true);
    this->invoke_to_dataIn(0, buffer1, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    // Second frame - continuation only
    Fw::Buffer buffer2 = this->assembleFrameBuffer(fullPacket + dataZoneSize, dataZoneSize,
                                                    AOSMPDUSubfields::FHP_NO_PACKET_START,
                                                    ComCfg::SpacecraftId, 0, 1, 1, true);
    this->invoke_to_dataIn(0, buffer2, context);
    ASSERT_from_dataOut_SIZE(0);
    this->clearHistory();

    // Third frame - rest of packet
    FwSizeType remainingBytes = totalPacketSize - (2 * dataZoneSize);
    Fw::Buffer buffer3 = this->assembleFrameBuffer(fullPacket + (2 * dataZoneSize), remainingBytes,
                                                    static_cast<U16>(remainingBytes),
                                                    ComCfg::SpacecraftId, 0, 2, 1, true);
    this->invoke_to_dataIn(0, buffer3, context);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), totalPacketSize);
}

void AosDeframerTester::testSpanningPacketContinuation() {
    this->configureDefault();

    // For spanning, packet must be larger than data zone size (246 bytes)
    // Create packet with header (6) + data (280) = 286 bytes
    const FwSizeType dataZoneSize = TEST_FRAME_SIZE - AOSHeader::SERIALIZED_SIZE -
                                     M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;
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

    Fw::Buffer buffer2 = this->assembleFrameBuffer(payload2, continuation + nextSize,
                                                    static_cast<U16>(continuation));

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
    payload[1] = 0x00;  // Length high byte
    payload[2] = 0x10;  // Length low byte (16)
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
    ASSERT_TLM_FrameCount_SIZE(1);
}

// ----------------------------------------------------------------------
// Tests - Configuration
// ----------------------------------------------------------------------

void AosDeframerTester::testFecfDisabled() {
    // When FECF is disabled, frame size is reduced by trailer size
    const U32 frameSizeNoFecf = TEST_FRAME_SIZE - AOSTrailer::SERIALIZED_SIZE;

    // Configure without FECF
    this->component.configure(frameSizeNoFecf, false, ComCfg::SpacecraftId, 0, true);

    U8 payload[100];
    FwSizeType sppSize = this->createSppPacket(payload, 0x200, 50);

    // Assemble frame without CRC
    Fw::Buffer buffer = this->assembleFrameBuffer(payload, sppSize, 0, ComCfg::SpacecraftId, 0, 0, 1, false);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    // No CRC error events
    ASSERT_EVENTS_InvalidCrc_SIZE(0);
}

void AosDeframerTester::testPvnMaskSppOnly() {
    // Configure for SPP only
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0, true, PvnBitfield::SPP_MASK);

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
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0, true, PvnBitfield::EPP_MASK);

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
        ASSERT_TLM_FrameCount(0, i + 1);
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

    ASSERT_TLM_PacketCount_SIZE(3);
    ASSERT_TLM_PacketCount(2, 3);  // Final value
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

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void AosDeframerTester::configureDefault() {
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0, true,
                              PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK);
}

Fw::Buffer AosDeframerTester::assembleFrameBuffer(U8* payload,
                                                   FwSizeType payloadLength,
                                                   U16 fhp,
                                                   U16 scid,
                                                   U8 vcid,
                                                   U32 vcCount,
                                                   U8 tfvn,
                                                   bool includeFecf) {
    const U32 frameSize = includeFecf ? TEST_FRAME_SIZE : (TEST_FRAME_SIZE - AOSTrailer::SERIALIZED_SIZE);
    ::memset(this->m_frameData, 0, sizeof(this->m_frameData));

    // Build AOS Primary Header (6 bytes)
    // Byte 0-1: globalVcId (2b TFVN | 8b SCID LSB | 6b VCID)
    U16 globalVcId = static_cast<U16>((tfvn & 0x3) << AOSHeaderSubfields::frameVersionOffset);
    globalVcId |= static_cast<U16>((scid & 0xFF) << AOSHeaderSubfields::spacecraftIdLsbOffset);
    globalVcId |= static_cast<U16>(vcid & 0x3F);
    this->m_frameData[0] = static_cast<U8>(globalVcId >> 8);
    this->m_frameData[1] = static_cast<U8>(globalVcId & 0xFF);

    // Byte 2-4: VC Frame Count (24 bits)
    this->m_frameData[2] = static_cast<U8>((vcCount >> 16) & 0xFF);
    this->m_frameData[3] = static_cast<U8>((vcCount >> 8) & 0xFF);
    this->m_frameData[4] = static_cast<U8>(vcCount & 0xFF);

    // Byte 5: Signaling field (replay | cycle use | SCID MSB | VC cycle)
    U8 signaling = 0;
    signaling |= static_cast<U8>(1 << AOSHeaderSubfields::cycleCountFlagOffset);  // Cycle count in use
    signaling |= static_cast<U8>(((scid >> 8) & 0x3) << AOSHeaderSubfields::spacecraftIdMsbOffset);
    signaling |= static_cast<U8>((vcCount >> 24) & 0x0F);  // Cycle count
    this->m_frameData[5] = signaling;

    // Byte 6-7: M_PDU Header (First Header Pointer)
    this->m_frameData[6] = static_cast<U8>(fhp >> 8);
    this->m_frameData[7] = static_cast<U8>(fhp & 0xFF);

    // Copy payload to data zone
    const FwSizeType dataZoneStart = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE;
    const FwSizeType dataZoneEnd = frameSize - (includeFecf ? AOSTrailer::SERIALIZED_SIZE : 0);
    FwSizeType maxPayload = dataZoneEnd - dataZoneStart;
    FwSizeType copyLen = FW_MIN(payloadLength, maxPayload);
    ::memcpy(this->m_frameData + dataZoneStart, payload, copyLen);

    // Fill remaining data zone with EPP fill packet (type=1, lengthOfLength=0)
    // This prevents interpretation of zeros as valid SPP packets
    FwSizeType fillStart = dataZoneStart + copyLen;
    if (fillStart < dataZoneEnd) {
        // EPP fill packet header: PVN=7, type=1, lengthOfLength=0
        // This is 0xF0 = (7<<5) | (1<<4) | 0
        this->m_frameData[fillStart] = 0xF0;
        // Fill rest with idle pattern
        for (FwSizeType i = fillStart + 1; i < dataZoneEnd; i++) {
            this->m_frameData[i] = 0x55;  // Idle fill pattern
        }
    }

    // Add FECF if enabled
    if (includeFecf) {
        U16 crc = Ccsds::Utils::CRC16::compute(this->m_frameData, frameSize - AOSTrailer::SERIALIZED_SIZE);
        this->m_frameData[frameSize - 2] = static_cast<U8>(crc >> 8);
        this->m_frameData[frameSize - 1] = static_cast<U8>(crc & 0xFF);
    }

    return Fw::Buffer(this->m_frameData, frameSize);
}

FwSizeType AosDeframerTester::createSppPacket(U8* buffer, U16 apid, U16 dataLength, U16 seqCount) {
    // SPP Header (6 bytes)
    // Byte 0-1: Packet Identification (3b PVN=0 | 1b type | 1b sec hdr | 11b APID)
    U16 pktId = apid & 0x07FF;  // APID in lower 11 bits, PVN=0 in upper bits
    buffer[0] = static_cast<U8>(pktId >> 8);
    buffer[1] = static_cast<U8>(pktId & 0xFF);

    // Byte 2-3: Packet Sequence Control (2b flags | 14b seq count)
    U16 seqCtrl = static_cast<U16>((0x3 << 14) | (seqCount & 0x3FFF));  // Unsegmented
    buffer[2] = static_cast<U8>(seqCtrl >> 8);
    buffer[3] = static_cast<U8>(seqCtrl & 0xFF);

    // Byte 4-5: Packet Data Length (actual length - 1)
    U16 lengthField = static_cast<U16>(dataLength > 0 ? dataLength - 1 : 0);
    buffer[4] = static_cast<U8>(lengthField >> 8);
    buffer[5] = static_cast<U8>(lengthField & 0xFF);

    // Fill data with pattern
    for (U16 i = 0; i < dataLength; i++) {
        buffer[6 + i] = static_cast<U8>(i & 0xFF);
    }

    return SpacePacketHeader::SERIALIZED_SIZE + dataLength;
}

FwSizeType AosDeframerTester::createEppPacket(U8* buffer, U8 protocolId, U16 dataLength) {
    // EPP Header
    // Byte 0: 3b PVN=7 | 1b type=0 | 4b protocol ID
    buffer[0] = static_cast<U8>((7 << 5) | (protocolId & 0x0F));

    // Byte 1-2: Length field (for standard protocol IDs)
    buffer[1] = static_cast<U8>(dataLength >> 8);
    buffer[2] = static_cast<U8>(dataLength & 0xFF);

    // Fill data with pattern
    for (U16 i = 0; i < dataLength; i++) {
        buffer[3 + i] = static_cast<U8>((i + 0x80) & 0xFF);
    }

    return 3 + dataLength;
}

FwSizeType AosDeframerTester::createEppIdlePacket(U8* buffer, U8 lengthOfLength, FwSizeType packetLength) {
    // EPP Idle Header
    // Byte 0: 3b PVN=7 | 1b type=1 | 4b length of length
    buffer[0] = static_cast<U8>((7 << 5) | (1 << 4) | (lengthOfLength & 0x0F));

    if (lengthOfLength == 0) {
        // Fill packet - just header byte
        return 1;
    }

    // Write length field
    FwSizeType offset = 1;
    for (U8 i = 0; i < lengthOfLength; i++) {
        U8 shift = static_cast<U8>((lengthOfLength - 1 - i) * 8);
        buffer[offset++] = static_cast<U8>((packetLength >> shift) & 0xFF);
    }

    // Fill with idle pattern
    for (FwSizeType i = 0; i < packetLength; i++) {
        buffer[offset++] = 0x55;
    }

    return 1 + lengthOfLength + packetLength;
}

U8 AosDeframerTester::getFrameTfvn(U8* frameData) {
    return static_cast<U8>((frameData[0] >> 6) & 0x03);
}

U16 AosDeframerTester::getFrameScId(U8* frameData) {
    U16 scidLsb = static_cast<U16>(((frameData[0] & 0x3F) << 2) | (frameData[1] >> 6));
    U16 scidMsb = static_cast<U16>((frameData[5] >> 4) & 0x03);
    return static_cast<U16>(scidLsb | (scidMsb << 8));
}

U8 AosDeframerTester::getFrameVcId(U8* frameData) {
    return static_cast<U8>(frameData[1] & 0x3F);
}

U32 AosDeframerTester::getFrameVcCount(U8* frameData) {
    U32 count = 0;
    count |= static_cast<U32>(frameData[2]) << 16;
    count |= static_cast<U32>(frameData[3]) << 8;
    count |= static_cast<U32>(frameData[4]);

    if (frameData[5] & 0x40) {  // Cycle count in use
        count |= static_cast<U32>(frameData[5] & 0x0F) << 24;
    }

    return count;
}

U16 AosDeframerTester::getFrameFhp(U8* frameData) {
    return static_cast<U16>((frameData[6] << 8) | frameData[7]);
}

}  // namespace Ccsds

}  // namespace Svc
