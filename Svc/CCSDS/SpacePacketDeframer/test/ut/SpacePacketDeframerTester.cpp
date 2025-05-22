// ======================================================================
// \title  SpacePacketDeframerTester.cpp
// \author chammard
// \brief  cpp file for SpacePacketDeframer component test harness implementation class
// ======================================================================

#include "SpacePacketDeframerTester.hpp"
#include "STest/Random/Random.hpp"

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
    // // Frame: 5 bytes (header) + 1 byte (data) + 2 bytes (trailer)
    // U16 scId = static_cast<U16>(STest::Random::lowerUpper(0, 0x3FF));  // random 10 bit Spacecraft ID
    // U8 vcId = static_cast<U8>(STest::Random::lowerUpper(0, 0x3F));      // random 6 bit virtual channel ID
    // U8 seqCount = static_cast<U8>(STest::Random::lowerUpper(0, 0xFF));  // random 8 bit sequence count
    // U8 dataLength = static_cast<U8>(STest::Random::lowerUpper(1, 200)); // bytes of data, random length
    // U8 data[dataLength];

    // Fw::Buffer buffer = this->assembleFrameBuffer(data, dataLength, scId, vcId, seqCount);
    // ComCfg::FrameContext nullContext;

    // this->setComponentState(scId, vcId, seqCount);
    // this->invoke_to_dataIn(0, buffer, nullContext);

    // ASSERT_from_dataOut_SIZE(1);
    // Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    // ASSERT_EQ(outBuffer.getSize(), dataLength);
}


}  // namespace CCSDS
}  // namespace Svc
