// ======================================================================
// \title  SpacePacketDeframerTester.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketDeframer component test harness implementation class
// ======================================================================

#include "SpacePacketDeframerTester.hpp"
#include "STest/Random/Random.hpp"
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
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(data));
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, nullContext);
}

void SpacePacketDeframerTester ::testNominalDeframing() {
    ComCfg::APID::T apid = static_cast<ComCfg::APID::T>(STest::Random::lowerUpper(0, 0x7FF));  // random 11 bit APID
    U16 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0x3FFF));  // random 14 bit sequence count
    U16 dataLength =
        static_cast<U8>(STest::Random::lowerUpper(1, MAX_TEST_PACKET_DATA_SIZE));  // bytes of data, random length
    U8 data[dataLength];
    U16 lengthToken = static_cast<U16>(dataLength - 1);  // Length token is length - 1
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(dataLength); ++i) {
        data[i] = static_cast<U8>(i);
    }

    Fw::Buffer buffer = this->assemblePacket(apid, seqCount, lengthToken, data, dataLength);
    ComCfg::FrameContext nullContext;

    this->invoke_to_dataIn(0, buffer, nullContext);

    // Check output packet payload
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), static_cast<Fw::Buffer::SizeType>(dataLength));
    for (U32 i = 0; i < dataLength; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], data[i]);
    }
    // Check output context (header info)
    ComCfg::FrameContext context = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(context.get_apid(), apid);
    ASSERT_EQ(context.get_sequenceCount(), seqCount);

    ASSERT_EVENTS_SIZE(0);  // No events should be generated in the nominal case
}

void SpacePacketDeframerTester ::testDeframingIncorrectLength() {
    ComCfg::APID::T apid = static_cast<ComCfg::APID::T>(STest::Random::lowerUpper(0, 0x7FF));  // random 11 bit APID
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
    Fw::Buffer returnedBuffer = this->fromPortHistory_dataReturnOut->at(0).data;
    ASSERT_EQ(returnedBuffer.getSize(), buffer.getSize());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, nullContext);  // Data should be the same as input

    // Event logging failure
    ASSERT_EVENTS_SIZE(1);                // No events should be generated in the nominal case
    ASSERT_EVENTS_InvalidLength_SIZE(1);  // No events should be generated in the nominal case
    ASSERT_EVENTS_InvalidLength(0, static_cast<U16>(invalidLengthToken + 1),
                                realDataLength);  // Event logs the size in bytes, so add 1 to length token
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

Fw::Buffer SpacePacketDeframerTester ::assemblePacket(U16 apid,
                                                      U16 seqCount,
                                                      U16 lengthToken,
                                                      U8* packetData,
                                                      U16 packetDataLen) {
    SpacePacketHeader header;
    header.set_packetIdentification(apid);
    header.set_packetSequenceControl(seqCount);  // Sequence Flags = 0b11 (unsegmented) & unused Seq count
    header.set_packetDataLength(lengthToken);

    Fw::ExternalSerializeBuffer serializer(static_cast<U8*>(this->m_packetBuffer), sizeof(this->m_packetBuffer));
    serializer.serialize(header);
    serializer.serialize(packetData, packetDataLen, Fw::Serialization::OMIT_LENGTH);
    return Fw::Buffer(this->m_packetBuffer,
                      static_cast<Fw::Buffer::SizeType>(packetDataLen + SpacePacketHeader::SERIALIZED_SIZE));
}

}  // namespace Ccsds
}  // namespace Svc
