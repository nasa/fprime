// ======================================================================
// \title  SpacePacketFramerTester.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketFramer component test harness implementation class
// ======================================================================

#include "SpacePacketFramerTester.hpp"
#include "STest/Random/Random.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SpacePacketFramerTester ::SpacePacketFramerTester()
    : SpacePacketFramerGTestBase("SpacePacketFramerTester", SpacePacketFramerTester::MAX_HISTORY_SIZE),
      component("SpacePacketFramer") {
    this->initComponents();
    this->connectPorts();
}

SpacePacketFramerTester ::~SpacePacketFramerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SpacePacketFramerTester::testComStatusPassthrough() {
    // Simulate a comStatusIn event and check comStatusOut
    Fw::Success status = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, status);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_comStatusOut->at(0).condition, status);
    this->clearHistory();
    status = Fw::Success::FAILURE;
    this->invoke_to_comStatusIn(0, status);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_comStatusOut->at(0).condition, status);
}

void SpacePacketFramerTester::testDataReturnPassthrough() {
    // Simulate a dataReturnIn event and check bufferDeallocate_out
    U8 data[8] = {0};
    Fw::Buffer buffer(data, sizeof(data));
    ComCfg::FrameContext context;
    this->invoke_to_dataReturnIn(0, buffer, context);
    ASSERT_from_bufferDeallocate_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferDeallocate->at(0).fwBuffer.getData(), data);
    ASSERT_EQ(this->fromPortHistory_bufferDeallocate->at(0).fwBuffer.getSize(), sizeof(data));
}

void SpacePacketFramerTester::testNominalFraming() {
    // Simulate framing a buffer and check output
    U8 payload[16];
    for (U32 i = 0; i < sizeof(payload); ++i) {
        payload[i] = static_cast<U8>(STest::Random::lowerUpper(0, 0xFF));
    }
    Fw::Buffer data(payload, sizeof(payload));
    ComCfg::APID::T apid = static_cast<ComCfg::APID::T>(STest::Random::lowerUpper(0, 0x7FF));  // random 11 bit APID
    U16 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0x3FFF));  // random 14 bit sequence count
    ComCfg::FrameContext context;
    context.set_apid(apid);
    this->m_nextSeqCount = seqCount;  // seqCount to be returned by getApidSeqCount output port

    this->invoke_to_dataIn(0, data, context);

    // Check dataOut
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), sizeof(payload) + SpacePacketHeader::SERIALIZED_SIZE);
    // Check that the payload is present at the correct offset
    for (U32 i = 0; i < sizeof(payload); ++i) {
        ASSERT_EQ(outBuffer.getData()[SpacePacketHeader::SERIALIZED_SIZE + i], payload[i]);
    }
    // Check that dataReturnOut is called for the original buffer
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), payload);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getSize(), sizeof(payload));
}

// ----------------------------------------------------------------------
// Output port handler overrides
// ----------------------------------------------------------------------

U16 SpacePacketFramerTester ::from_getApidSeqCount_handler(FwIndexType portNum,
                                                           const ComCfg::APID& apid,
                                                           U16 sequenceCount) {
    return this->m_nextSeqCount;
}

Fw::Buffer SpacePacketFramerTester ::from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) {
    return Fw::Buffer(this->m_internalDataBuffer, size);
}

}  // namespace Ccsds

}  // namespace Svc
