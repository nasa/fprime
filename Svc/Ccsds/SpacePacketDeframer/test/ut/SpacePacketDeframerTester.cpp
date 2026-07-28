// ======================================================================
// \title  SpacePacketDeframerTester.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketDeframer component test harness implementation class
// ======================================================================

#include "SpacePacketDeframerTester.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SpacePacketDeframerTester ::SpacePacketDeframerTester()
    : SpacePacketDeframerGTestBase("SpacePacketDeframerTester", SpacePacketDeframerTester::MAX_HISTORY_SIZE),
      component("SpacePacketDeframer") {
    this->initComponents();
    this->connectPorts();
}

SpacePacketDeframerTester ::~SpacePacketDeframerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SpacePacketDeframerTester ::testDataReturnPassthrough() {
    U8 data[1];
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext nullContext;
    this->invoke_to_dataReturnIn(0, buffer, nullContext);
    ASSERT_from_dataReturnOut_SIZE(1);  // incoming buffer should be deallocated
    ASSERT_FROM_PORT_HISTORY_SIZE(1);   // only port call
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(data));
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, nullContext);
}

void SpacePacketDeframerTester ::testNominalDeframing() {
    ComCfg::Apid::T apid = static_cast<ComCfg::Apid::T>(STest::Random::lowerUpper(0, 0x7FF));  // random 11 bit APID
    U16 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0x3FFF));  // random 14 bit sequence count
    U16 dataLength =
        static_cast<U8>(STest::Random::lowerUpper(1, MAX_TEST_PACKET_DATA_SIZE));  // bytes of data, random length
    U8 data[dataLength];
    U16 lengthToken = static_cast<U16>(dataLength - 1);  // Length token is length - 1
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(dataLength); ++i) {
        data[i] = static_cast<U8>(i);
    }

    bool hasSecHdr = static_cast<bool>(STest::Random::lowerUpper(0, 1));  // random secondary header flag
    Fw::Buffer buffer = this->assemblePacket(apid, seqCount, lengthToken, data, dataLength, hasSecHdr);
    ComCfg::FrameContext nullContext;

    this->invoke_to_dataIn(0, buffer, nullContext);

    // Check output packet payload
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_validateApidSeqCount_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);  // only two port calls in nominal case
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), static_cast<Fw::Buffer::SizeType>(dataLength));
    for (U32 i = 0; i < dataLength; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], data[i]);
    }
    // Check output context (header info)
    ComCfg::FrameContext context = this->fromPortHistory_dataOut->at(0).context;
    if (ComCfg::Apid::isValid(apid)) {
        ASSERT_EQ(context.get_apid(), apid);
    } else {
        ASSERT_EQ(context.get_apid(), ComCfg::Apid::INVALID_UNINITIALIZED);
    }
    ASSERT_EQ(context.get_sequenceCount(), seqCount);
    ASSERT_EQ(context.get_hasSecHdr(), hasSecHdr);

    ASSERT_EVENTS_SIZE(0);  // No events should be generated in the nominal case
}

void SpacePacketDeframerTester ::testDeframingIncorrectLength() {
    ComCfg::Apid::T apid = static_cast<ComCfg::Apid::T>(STest::Random::lowerUpper(0, 0x7FF));  // random 11 bit APID
    U16 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0x3FFF));  // random 14 bit sequence count
    U16 realDataLength =
        static_cast<U8>(STest::Random::lowerUpper(1, MAX_TEST_PACKET_DATA_SIZE));  // bytes of data, random length
    U16 invalidLengthToken =
        static_cast<U16>(realDataLength + 1);  // Length token is greater than actual data available
    U8 data[realDataLength];

    Fw::Buffer buffer = this->assemblePacket(apid, seqCount, invalidLengthToken, data, realDataLength);
    ComCfg::FrameContext nullContext;

    this->invoke_to_dataIn(0, buffer, nullContext);

    // No data emitted
    ASSERT_from_dataOut_SIZE(0);
    // Data returned (frame dropped)
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);  // two port calls, one for dataReturn and one for errorNotify
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::SP_INVALID_LENGTH);
    Fw::Buffer returnedBuffer = this->fromPortHistory_dataReturnOut->at(0).data;
    ASSERT_EQ(returnedBuffer.getSize(), buffer.getSize());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, nullContext);  // Data should be the same as input

    // Event logging failure
    ASSERT_EVENTS_SIZE(1);                // No events should be generated in the nominal case
    ASSERT_EVENTS_InvalidLength_SIZE(1);  // No events should be generated in the nominal case
    ASSERT_EVENTS_InvalidLength(0, static_cast<U16>(invalidLengthToken + 1),
                                realDataLength);  // Event logs the size in bytes, so add 1 to length token
}

void SpacePacketDeframerTester ::testPacketDataLengthMaxU16Overflow() {
    // This test asserts the correct overflow behavior when all bits of packet
    // length are used.

    ComCfg::Apid::T apid = static_cast<ComCfg::Apid::T>(1);
    U16 seqCount = 0;
    const U16 overflowLengthToken = 0xFFFFU;  // triggers U16 wrap-to-zero without fix
    U8 payload[1] = {0xAB};                   // 1-byte payload — buffer size will be SERIALIZED_SIZE+1

    Fw::Buffer buffer = this->assemblePacket(apid, seqCount, overflowLengthToken, payload, sizeof(payload));
    ComCfg::FrameContext nullContext;

    this->invoke_to_dataIn(0, buffer, nullContext);

    // Packet must NOT be forwarded downstream
    ASSERT_from_dataOut_SIZE(0);
    // Buffer must be returned (frame dropped)
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::SP_INVALID_LENGTH);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);  // dataReturnOut + errorNotify
    // Returned buffer must be the original input
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, nullContext);
    // InvalidLength event must fire.
    // The EVR argument for transmitted length is static_cast<U16>(pkt_length_wide)
    // where pkt_length_wide=65536, so static_cast<U16>(65536)=0 — same result as
    // static_cast<U16>(overflowLengthToken + 1).
    // maxDataAvailable = bufSize - SERIALIZED_SIZE = 1.
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidLength_SIZE(1);
    ASSERT_EVENTS_InvalidLength(0, overflowLengthToken + 1, sizeof(payload));
}

void SpacePacketDeframerTester ::testBufferExactlyHeaderSize() {
    U8 rawData[SpacePacketHeader::SERIALIZED_SIZE] = {};
    Fw::Buffer buffer(rawData, SpacePacketHeader::SERIALIZED_SIZE);
    ComCfg::FrameContext nullContext;

    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::SP_INVALID_PACKET);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);  // dataReturnOut + errorNotify
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidPacket_SIZE(1);
}

void SpacePacketDeframerTester ::testBufferSmallerThanHeaderSize() {
    // A buffer smaller than SERIALIZED_SIZE — cannot hold even the header.
    // Must be dropped gracefully without asserting.
    U8 rawData[SpacePacketHeader::SERIALIZED_SIZE - 1] = {};
    Fw::Buffer buffer(rawData, SpacePacketHeader::SERIALIZED_SIZE - 1);
    ComCfg::FrameContext nullContext;

    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::SP_INVALID_PACKET);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidPacket_SIZE(1);
}

void SpacePacketDeframerTester ::testBufferSingleByte() {
    // Single-byte buffer — most extreme undersize input.
    // Verifies the size guard fires and no crash occurs.
    U8 rawData[1] = {0xFF};
    Fw::Buffer buffer(rawData, sizeof(rawData));
    ComCfg::FrameContext nullContext;

    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::SP_INVALID_PACKET);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidPacket_SIZE(1);
}

void SpacePacketDeframerTester ::testInvalidPacketIdentificationControlFields() {
    U8 payload[2] = {0xAA, 0xBB};
    ComCfg::FrameContext nullContext;

    Fw::Buffer buffer =
        this->assemblePacketWithControlFields(0x1,  // invalid PVN for Space Packet Protocol in this component
                                              0x0, 0x0, static_cast<U16>(ComCfg::Apid::FW_PACKET_TELEM), 0x3, 0x0012,
                                              static_cast<U16>(sizeof(payload) - 1), payload, sizeof(payload));

    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_validateApidSeqCount_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_errorNotify(0, Svc::Ccsds::FrameError::SP_INVALID_PACKET);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_InvalidPacket_SIZE(1);
}

void SpacePacketDeframerTester ::testCommandPacketTypeAccepted() {
    this->testControlFieldAccepted(0x0, 0x1, 0x0, ComCfg::Apid::FW_PACKET_COMMAND, 0x3);
}

void SpacePacketDeframerTester ::testSecondaryHeaderFlagAccepted() {
    this->testControlFieldAccepted(0x0, 0x0, 0x1, ComCfg::Apid::FW_PACKET_COMMAND, 0x3);
}

void SpacePacketDeframerTester ::testSequenceFlagsAccepted() {
    this->testControlFieldAccepted(0x0, 0x0, 0x0, ComCfg::Apid::FW_PACKET_TELEM, 0x1);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void SpacePacketDeframerTester ::testControlFieldAccepted(U16 pvn,
                                                          U16 packetType,
                                                          U16 secondaryHeaderFlag,
                                                          ComCfg::Apid::T expectedApid,
                                                          U16 sequenceFlags) {
    U8 payload[2] = {0xAA, 0xBB};
    const U16 seqCount = 0x0012;
    ComCfg::FrameContext nullContext;

    Fw::Buffer buffer = this->assemblePacketWithControlFields(
        pvn, packetType, secondaryHeaderFlag, static_cast<U16>(expectedApid), sequenceFlags, seqCount,
        static_cast<U16>(sizeof(payload) - 1), payload, sizeof(payload));

    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_validateApidSeqCount_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(2);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_apid(), expectedApid);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_sequenceCount(), seqCount);
    ASSERT_EVENTS_SIZE(0);
}

Fw::Buffer SpacePacketDeframerTester ::assemblePacket(U16 apid,
                                                      U16 seqCount,
                                                      U16 lengthToken,
                                                      U8* packetData,
                                                      U16 packetDataLen,
                                                      bool hasSecHdr) {
    return this->assemblePacketWithControlFields(0x0, 0x0, static_cast<U16>(hasSecHdr), apid, 0x3, seqCount,
                                                 lengthToken, packetData, packetDataLen);
}

Fw::Buffer SpacePacketDeframerTester ::assemblePacketWithControlFields(U16 pvn,
                                                                       U16 packetType,
                                                                       U16 secondaryHeaderFlag,
                                                                       U16 apid,
                                                                       U16 sequenceFlags,
                                                                       U16 seqCount,
                                                                       U16 lengthToken,
                                                                       U8* packetData,
                                                                       U16 packetDataLen) {
    SpacePacketHeader header;
    const U16 packetIdentification = static_cast<U16>(
        ((pvn << SpacePacketSubfields::PvnOffset) & SpacePacketSubfields::PvnMask) |
        ((packetType << SpacePacketSubfields::PktTypeOffset) & SpacePacketSubfields::PktTypeMask) |
        ((secondaryHeaderFlag << SpacePacketSubfields::SecHdrOffset) & SpacePacketSubfields::SecHdrMask) |
        (apid & SpacePacketSubfields::ApidMask));
    const U16 packetSequenceControl = static_cast<U16>(
        ((sequenceFlags << SpacePacketSubfields::SeqFlagsOffset) & SpacePacketSubfields::SeqFlagsMask) |
        (seqCount & SpacePacketSubfields::SeqCountMask));

    header.set_packetIdentification(packetIdentification);
    header.set_packetSequenceControl(packetSequenceControl);
    header.set_packetDataLength(lengthToken);

    Fw::ExternalSerializeBuffer serializer(static_cast<U8*>(this->m_packetBuffer), sizeof(this->m_packetBuffer));
    serializer.serializeFrom(header);
    serializer.serializeFrom(packetData, packetDataLen, Fw::Serialization::OMIT_LENGTH);
    return Fw::Buffer(this->m_packetBuffer,
                      static_cast<Fw::Buffer::SizeType>(packetDataLen + SpacePacketHeader::SERIALIZED_SIZE));
}

}  // namespace Ccsds
}  // namespace Svc
