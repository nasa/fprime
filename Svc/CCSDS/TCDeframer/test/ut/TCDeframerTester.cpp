// ======================================================================
// \title  TCDeframerTester.cpp
// \author thomas-bc
// \brief  cpp file for TCDeframer component test harness implementation class
// ======================================================================

#include "TCDeframerTester.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/CCSDS/Utils/CRC16.hpp"
#include "Svc/CCSDS/Types/TCFrameHeaderSerializableAc.hpp"
#include "Svc/CCSDS/Types/TCFrameTrailerSerializableAc.hpp"

namespace Svc {

namespace CCSDS {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TCDeframerTester ::TCDeframerTester()
    : TCDeframerGTestBase("TCDeframerTester", TCDeframerTester::MAX_HISTORY_SIZE), component("TCDeframer") {
    this->initComponents();
    this->connectPorts();
}

TCDeframerTester ::~TCDeframerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TCDeframerTester::testDataReturn() {
    U8 data[1];
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext nullContext;
    this->invoke_to_dataReturnIn(0, buffer, nullContext);
    ASSERT_from_dataReturnOut_SIZE(1); // incoming buffer should be deallocated
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(data));
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, nullContext);
}

void TCDeframerTester::testNominalDeframing() {
    // Frame: 5 bytes (header) + 1 byte (data) + 2 bytes (trailer)
    U16 scId = static_cast<U16>(STest::Random::lowerUpper(0, 0x3FF)); // random 10 bit Spacecraft ID
    U8 vcId = static_cast<U8>(STest::Random::lowerUpper(0, 0x3F)); // random 6 bit virtual channel ID
    U8 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0xFF)); // random 8 bit sequence count
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200)); // bytes of data, random length
    U8 data[dataLength];

    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength, scId, vcId, seqCount);
    ComCfg::FrameContext nullContext;

    this->setComponentState(scId, vcId, seqCount);
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), dataLength);
    for (FwIndexType i = 0; i < dataLength; i++) {
        ASSERT_EQ(outBuffer.getData()[i], data[i]);
    }
}

void TCDeframerTester::testInvalidScId() {
    // Frame: 5 bytes (header) + 1 byte (data) + 2 bytes (trailer)
    U16 scId = static_cast<U16>(STest::Random::lowerUpper(1, 0x3FF)); // random 10 bit Spacecraft ID
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200)); // bytes of data, random length
    U8 data[dataLength];

    // Assemble frame with incorrect scId value
    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength, scId - 1);
    ComCfg::FrameContext nullContext;

    this->setComponentState(scId);
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1); // invalid buffer was deallocated
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1); // exactly 1 event emitted
    ASSERT_EVENTS_InvalidSpacecraftId_SIZE(1); // event was emitted for invalid spacecraft ID
    ASSERT_EVENTS_InvalidSpacecraftId(0, scId - 1, scId); // event was emitted for invalid spacecraft ID
}


void TCDeframerTester::testInvalidVcId() {
    U8 vcId = static_cast<U8>(STest::Random::lowerUpper(1, 0x3F)); // random 6 bit VCID
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200)); // bytes of data, random length
    U8 data[dataLength];

    // Assemble frame with incorrect vcId value
    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength, 0, vcId - 1);
    ComCfg::FrameContext nullContext;

    this->setComponentState(0, vcId, 0, false); // set the component in mode where only one VCID is accepted
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1); // invalid buffer was deallocated
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1); // exactly 1 event emitted
    ASSERT_EVENTS_InvalidVcId_SIZE(1); // event was emitted for invalid VCID
    ASSERT_EVENTS_InvalidVcId(0, vcId - 1, vcId); // event was emitted for invalid VCID
}

void TCDeframerTester::testInvalidLengthToken() {
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200)); // bytes of data, random length
    U8 data[dataLength];
    U8 fakeLength = 255; // more than max dataLength value

    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength);
    buffer.getData()[3] = fakeLength; // Override length token to invalid value
    ComCfg::FrameContext nullContext;

    this->setComponentState();
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1); // invalid buffer was deallocated
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1); // exactly 1 event emitted
    ASSERT_EVENTS_InvalidFrameLength_SIZE(1); // event was emitted for invalid frame length
    ASSERT_EVENTS_InvalidFrameLength(0, fakeLength, dataLength);
}

void TCDeframerTester::testInvalidSequenceNumber() {
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200)); // bytes of data, random length
    U8 data[dataLength];
    U8 expectedSeqCount = static_cast<U8>(STest::Random::lowerUpper(1, 0xFF)); // random 8 bit sequence count
    U8 invalidSeqCount = expectedSeqCount - 1; // invalid sequence count

    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength, 0, 0, invalidSeqCount);
    ComCfg::FrameContext nullContext;

    this->setComponentState(0, 0, expectedSeqCount);
    this->invoke_to_dataIn(0, buffer, nullContext);

    // Invalid sequence number should emit a warning but deframe data (not dropping the frame)
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_dataReturnOut_SIZE(0);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), dataLength);
    ASSERT_EVENTS_SIZE(1); // exactly 1 event emitted
    ASSERT_EVENTS_UnexpectedSequenceNumber_SIZE(1);
    ASSERT_EVENTS_UnexpectedSequenceNumber(0, invalidSeqCount, expectedSeqCount);
}

void TCDeframerTester::testInvalidCrc() {
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200)); // bytes of data, random length
    U8 data[dataLength];
    
    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength);
    // Override CRC to invalid value
    buffer.getData()[TCFrameHeader::SERIALIZED_SIZE + dataLength + 1] = 0x00;
    ComCfg::FrameContext nullContext;

    this->setComponentState();
    this->invoke_to_dataIn(0, buffer, nullContext);

    // Invalid CRC drops the frame
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1); // exactly 1 event emitted
    ASSERT_EVENTS_InvalidCrc_SIZE(1);
}

void TCDeframerTester::setComponentState(U16 scid, U8 vcid, U8 sequenceNumber, bool acceptAllVcid) {
    this->component.m_spacecraftId = scid;
    this->component.m_vcId = vcid;
    this->component.m_sequenceCount = sequenceNumber;
    this->component.m_acceptAllVcid = acceptAllVcid;
}

Fw::Buffer TCDeframerTester::assembleFrameBuffer(U8* data, U8 dataLength, U16 scid, U8 vcid, U8 seqNumber){
    ::memset(this->m_frameData, 0, sizeof(this->m_frameData));
    U16 frameLength = TCFrameHeader::SERIALIZED_SIZE + dataLength + TCFrameTrailer::SERIALIZED_SIZE;

    // Header
    this->m_frameData[0] = scid >> 8;
    this->m_frameData[1] = scid & 0xFF;
    this->m_frameData[2] = static_cast<U8>((vcid << 2) | static_cast<U8>((dataLength >> 8) & 0x03));
    this->m_frameData[3] = dataLength & 0xFF;
    this->m_frameData[4] = seqNumber;

    // Data
    memcpy(&this->m_frameData[TCFrameHeader::SERIALIZED_SIZE], data, dataLength);

    // CRC trailer
    U16 crc = CCSDS::Utils::CRC16::compute(this->m_frameData, TCFrameHeader::SERIALIZED_SIZE + dataLength);
    this->m_frameData[TCFrameHeader::SERIALIZED_SIZE + dataLength] = crc >> 8;
    this->m_frameData[TCFrameHeader::SERIALIZED_SIZE + dataLength + 1] = crc & 0xFF;

    return Fw::Buffer(this->m_frameData, frameLength);
}

}  // namespace CCSDS
}  // namespace Svc
