// ======================================================================
// \title  SpacePacketDeframerTester.cpp
// \author chammard
// \brief  cpp file for SpacePacketDeframer component test harness implementation class
// ======================================================================

#include "SpacePacketDeframerTester.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/CCSDS/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {

namespace CCSDS {

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
    U16 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, MAX_TEST_PACKET_DATA_SIZE)); // bytes of data, random length
    U8 data[dataLength];

    Fw::Buffer buffer = this->assemblePacket(apid, seqCount, dataLength, data);
    ComCfg::FrameContext nullContext;

    // Add the APID to the tracked APID list for nominal deframing
    this->component.m_apidSequences[0].apid = apid;
    this->component.m_apidSequences[0].sequenceCount = seqCount;
    
    this->invoke_to_dataIn(0, buffer, nullContext);

    // Check output packet payload
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), dataLength);
    for (U32 i = 0; i < dataLength; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], data[i]);
    }
    // Check output context (header info)
    ComCfg::FrameContext context = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(context.getapid(), apid);
    ASSERT_EQ(context.getsequenceCount(), seqCount);

    ASSERT_EVENTS_SIZE(0);  // No events should be generated in the nominal case
}

void SpacePacketDeframerTester ::testDeframingUntrackedApid() {
    U16 apid = 0x7FF - 2; // Unspecified APID
    U16 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0x3FFF));  // random 14 bit sequence count
    U16 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, MAX_TEST_PACKET_DATA_SIZE)); // bytes of data, random length
    U8 data[dataLength];

    Fw::Buffer buffer = this->assemblePacket(apid, seqCount, dataLength, data);
    ComCfg::FrameContext nullContext;

    this->invoke_to_dataIn(0, buffer, nullContext);

    // Check output packet payload
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), dataLength);
    for (U32 i = 0; i < dataLength; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], data[i]);
    }
    // Check output context (header info)
    ComCfg::FrameContext outContext = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(outContext.getapid(), apid);
    ASSERT_EQ(outContext.getsequenceCount(), seqCount);
    // Check event logging that untracked APID was received
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_UntrackedApid_SIZE(1);
}

void SpacePacketDeframerTester ::testDeframingIncorrectSeqCount() {
    U16 apid = ComCfg::APID::FW_PACKET_COMMAND;
    U16 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, MAX_TEST_PACKET_DATA_SIZE)); // bytes of data, random length
    U8 data[dataLength];
    U16 receivedSeqCount = static_cast<U8>(STest::Random::lowerUpper(1, 0x3FFF));  // random 14 bit sequence count
    U16 expectedSeqCount = receivedSeqCount - 1; // effectively mocks a packet drop

    Fw::Buffer buffer = this->assemblePacket(apid, receivedSeqCount, dataLength, data);
    ComCfg::FrameContext nullContext;

    // Add the APID to the tracked APID list for nominal deframing
    this->component.m_apidSequences[0].apid = ComCfg::APID::FW_PACKET_COMMAND;
    this->component.m_apidSequences[0].sequenceCount = expectedSeqCount;

    this->invoke_to_dataIn(0, buffer, nullContext);

    // Check output packet payload
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), dataLength);
    for (U32 i = 0; i < dataLength; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], data[i]);
    }
    // Check output context (header info)
    ComCfg::FrameContext outContext = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(outContext.getapid(), apid);
    ASSERT_EQ(outContext.getsequenceCount(), receivedSeqCount);
    // Check event logging that untracked APID was received
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_UnexpectedSequenceCount_SIZE(1);
    ASSERT_EVENTS_UnexpectedSequenceCount(0, receivedSeqCount, expectedSeqCount);
    // Check that the sequence count was updated to the next after the received one
    ASSERT_EQ(this->component.m_apidSequences[0].sequenceCount, receivedSeqCount + 1);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

Fw::Buffer SpacePacketDeframerTester ::assemblePacket(U16 apid, U16 seqCount, U16 packetLength, U8* packetData) {
    SpacePacketHeader header;
    header.setpacketIdentification(apid);
    header.setpacketSequenceControl(seqCount); // Sequence Flags = 0b11 (unsegmented) & unused Seq count
    header.setpacketDataLength(packetLength);

    Fw::ExternalSerializeBuffer serializer(static_cast<U8*>(this->m_packetBuffer), sizeof(this->m_packetBuffer));
    serializer.serialize(header);
    serializer.serialize(packetData, packetLength, Fw::Serialization::OMIT_LENGTH);
    return Fw::Buffer(this->m_packetBuffer, static_cast<Fw::Buffer::SizeType>(sizeof(this->m_packetBuffer)));
}

}  // namespace CCSDS
}  // namespace Svc
