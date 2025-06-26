// ======================================================================
// \title  TcDeframerTester.cpp
// \author thomas-bc
// \brief  cpp file for TcDeframer component test harness implementation class
// ======================================================================

#include "TcDeframerTester.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/Types/TCHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TCTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TcDeframerTester ::TcDeframerTester()
    : TcDeframerGTestBase("TcDeframerTester", TcDeframerTester::MAX_HISTORY_SIZE), component("TcDeframer") {
    this->initComponents();
    this->connectPorts();
}

TcDeframerTester ::~TcDeframerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TcDeframerTester::testDataReturn() {
    U8 data[1] = {0};
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext nullContext;
    this->invoke_to_dataReturnIn(0, buffer, nullContext);
    ASSERT_from_dataReturnOut_SIZE(1);  // incoming buffer should be deallocated
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), data);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(data));
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, nullContext);
}

void TcDeframerTester::testNominalDeframing() {
    // Frame: 5 bytes (header) + bytes (data) + 2 bytes (trailer)
    U16 scId = static_cast<U16>(STest::Random::lowerUpper(0, 0x3FF));       // random 10 bit Spacecraft ID
    U8 vcId = static_cast<U8>(STest::Random::lowerUpper(0, 0x3F));          // random 6 bit virtual channel ID
    U8 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0xFF));      // random 8 bit sequence count
    U8 payloadLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 payload[payloadLength];
    // Initialize payload with some data
    for (FwIndexType i = 0; i < payloadLength; i++) {
        payload[i] = static_cast<U8>(i % std::numeric_limits<U8>::max());
    }

    Fw::Buffer buffer = this->assembleFrameBuffer(payload, payloadLength, scId, vcId, seqCount);
    ComCfg::FrameContext nullContext;

    this->setComponentState(scId, vcId, seqCount);
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), payloadLength);
    for (FwIndexType i = 0; i < payloadLength; i++) {
        ASSERT_EQ(outBuffer.getData()[i], payload[i]);
    }
}

void TcDeframerTester::testInvalidScId() {
    // Frame: 5 bytes (header) + 1 byte (data) + 2 bytes (trailer)
    U16 scId = static_cast<U16>(STest::Random::lowerUpper(1, 0x3FF));    // random 10 bit Spacecraft ID
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 data[dataLength];

    // Assemble frame with incorrect scId value
    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength, static_cast<U16>(scId - 1));
    ComCfg::FrameContext nullContext;

    this->setComponentState(scId);
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // invalid buffer was deallocated
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1);                      // exactly 1 event emitted
    ASSERT_EVENTS_InvalidSpacecraftId_SIZE(1);  // event was emitted for invalid spacecraft ID
    ASSERT_EVENTS_InvalidSpacecraftId(0, static_cast<U16>(scId - 1),
                                      scId);  // event was emitted for invalid spacecraft ID
}

void TcDeframerTester::testInvalidVcId() {
    U8 vcId = static_cast<U8>(STest::Random::lowerUpper(1, 0x3F));       // random 6 bit VCID
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 data[dataLength];

    // Assemble frame with incorrect vcId value
    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength, 0, static_cast<U8>(vcId - 1));
    ComCfg::FrameContext nullContext;

    this->setComponentState(0, vcId, 0, false);  // set the component in mode where only one VCID is accepted
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // invalid buffer was deallocated
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1);                                           // exactly 1 event emitted
    ASSERT_EVENTS_InvalidVcId_SIZE(1);                               // event was emitted for invalid VCID
    ASSERT_EVENTS_InvalidVcId(0, static_cast<U16>(vcId - 1), vcId);  // event was emitted for invalid VCID
}

void TcDeframerTester::testInvalidLengthToken() {
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 data[dataLength];
    U8 incorrectLengthToken = static_cast<U8>(dataLength + TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE + 1);

    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength);
    buffer.getData()[3] = incorrectLengthToken;  // Override length token to invalid value
    ComCfg::FrameContext nullContext;

    this->setComponentState();
    this->invoke_to_dataIn(0, buffer, nullContext);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);  // invalid buffer was deallocated
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1);                     // exactly 1 event emitted
    ASSERT_EVENTS_InvalidFrameLength_SIZE(1);  // event was emitted for invalid frame length
    // event logs size in bytes which is length token + 1
    ASSERT_EVENTS_InvalidFrameLength(
        0, static_cast<U16>(incorrectLengthToken + 1),
        static_cast<FwSizeType>(dataLength + TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE));
}

void TcDeframerTester::testInvalidCrc() {
    U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200));  // bytes of data, random length
    U8 data[dataLength];

    Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength);
    // Override CRC to invalid value
    buffer.getData()[TCHeader::SERIALIZED_SIZE + dataLength + 1] = 0x00;
    ComCfg::FrameContext nullContext;

    this->setComponentState();
    this->invoke_to_dataIn(0, buffer, nullContext);

    // Invalid CRC drops the frame
    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), buffer.getData());
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), buffer.getSize());
    ASSERT_EVENTS_SIZE(1);  // exactly 1 event emitted
    ASSERT_EVENTS_InvalidCrc_SIZE(1);
}

void TcDeframerTester::setComponentState(U16 scid, U8 vcid, U8 sequenceNumber, bool acceptAllVcid) {
    this->component.configure(vcid, scid, acceptAllVcid);
    // this->component.m_spacecraftId = scid;
    // this->component.m_vcId = vcid;
    // // this->component.m_sequenceCount = sequenceNumber;
    // this->component.m_acceptAllVcid = acceptAllVcid;
}

Fw::Buffer TcDeframerTester::assembleFrameBuffer(U8* data, U8 dataLength, U16 scid, U8 vcid, U8 seqNumber) {
    ::memset(this->m_frameData, 0, sizeof(this->m_frameData));
    U16 frameLength = static_cast<U16>(TCHeader::SERIALIZED_SIZE + dataLength + TCTrailer::SERIALIZED_SIZE);
    U16 frameLengthToken = static_cast<U16>(frameLength - 1);  // length token is length - 1
    // Header
    this->m_frameData[0] = static_cast<U8>(scid >> 8);
    this->m_frameData[1] = static_cast<U8>(scid & 0xFF);
    this->m_frameData[2] = static_cast<U8>((vcid << 2) | static_cast<U8>((frameLengthToken >> 8) & 0x03));
    this->m_frameData[3] = static_cast<U8>(frameLengthToken & 0xFF);
    this->m_frameData[4] = seqNumber;

    // Data
    memcpy(&this->m_frameData[TCHeader::SERIALIZED_SIZE], data, dataLength);

    // CRC trailer
    U16 crc = Ccsds::Utils::CRC16::compute(this->m_frameData, TCHeader::SERIALIZED_SIZE + dataLength);
    this->m_frameData[TCHeader::SERIALIZED_SIZE + dataLength] = static_cast<U8>(crc >> 8);
    this->m_frameData[TCHeader::SERIALIZED_SIZE + dataLength + 1] = static_cast<U8>(crc & 0xFF);

    return Fw::Buffer(this->m_frameData, frameLength);
}

}  // namespace Ccsds
}  // namespace Svc
