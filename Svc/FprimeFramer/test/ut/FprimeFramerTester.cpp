// ======================================================================
// \title  FprimeFramerTester.cpp
// \author chammard
// \brief  cpp file for FprimeFramer component test harness implementation class
// ======================================================================

#include "FprimeFramerTester.hpp"
#include "Svc/FprimeProtocol/FrameHeaderSerializableAc.hpp"
#include "Svc/FprimeProtocol/FrameTrailerSerializableAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FprimeFramerTester ::FprimeFramerTester()
    : FprimeFramerGTestBase("FprimeFramerTester", FprimeFramerTester::MAX_HISTORY_SIZE), component("FprimeFramer") {
    this->initComponents();
    this->connectPorts();
}

FprimeFramerTester ::~FprimeFramerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void FprimeFramerTester ::testComStatusPassThrough() {
    // Send a status message to the component
    Fw::Success inputStatus = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, inputStatus);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, inputStatus); // at index 0, received SUCCESS
    inputStatus = Fw::Success::FAILURE;
    this->invoke_to_comStatusIn(0, inputStatus);
    ASSERT_from_comStatusOut_SIZE(2);
    ASSERT_from_comStatusOut(1, inputStatus); // at index 1, received FAILURE
}

void FprimeFramerTester ::testNominalFraming() {
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    FprimeProtocol::DataLinkContext context;


    // Fill the buffer with some data
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }
    buffer.setSize(sizeof(bufferData));

    // Send the buffer to the component
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_framedDataOut_SIZE(1);
    ASSERT_from_framedStreamOut_SIZE(1);

    // Check the size of the output buffer
    Fw::Buffer outputBuffer = this->fromPortHistory_framedStreamOut->at(0).sendBuffer;
    ASSERT_EQ(outputBuffer.getSize(), sizeof(bufferData) + FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
}

}  // namespace Svc
