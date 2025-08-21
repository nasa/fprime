// ======================================================================
// \title  FprimeFramerTester.cpp
// \author thomas-bc
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

void FprimeFramerTester ::testFrameDeallocation() {
    // When receiving a buffer on dataReturnIn, the buffer should be deallocated
    Fw::Buffer buffer;
    ComCfg::FrameContext context;
    this->invoke_to_dataReturnIn(0, buffer, context);
    ASSERT_from_bufferDeallocate_SIZE(1);
    ASSERT_from_bufferDeallocate(0, buffer);
}

void FprimeFramerTester ::testComStatusPassThrough() {
    // Send a status message to the component
    Fw::Success inputStatus = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, inputStatus);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, inputStatus);  // at index 0, received SUCCESS
    inputStatus = Fw::Success::FAILURE;
    this->invoke_to_comStatusIn(0, inputStatus);
    ASSERT_from_comStatusOut_SIZE(2);
    ASSERT_from_comStatusOut(1, inputStatus);  // at index 1, received FAILURE
}

void FprimeFramerTester ::testNominalFraming() {
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;

    // Fill the buffer with some data
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Send the buffer to the component
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_dataOut_SIZE(1);        // One frame emitted
    ASSERT_from_dataReturnOut_SIZE(1);  // Original data buffer ownership returned

    Fw::Buffer outputBuffer = this->fromPortHistory_dataOut->at(0).data;
    // Check the size of the output buffer
    ASSERT_EQ(outputBuffer.getSize(), sizeof(bufferData) + FprimeProtocol::FrameHeader::SERIALIZED_SIZE +
                                          FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
    // Check header
    FprimeProtocol::FrameHeader defaultHeader;
    FprimeProtocol::FrameHeader outputHeader;
    outputBuffer.getDeserializer().deserialize(outputHeader);
    ASSERT_EQ(outputHeader.get_startWord(), defaultHeader.get_startWord());
    ASSERT_EQ(outputHeader.get_lengthField(), sizeof(bufferData));
    // Check data
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        ASSERT_EQ(outputBuffer.getData()[i + FprimeProtocol::FrameHeader::SERIALIZED_SIZE], bufferData[i]);
    }
}

// ----------------------------------------------------------------------
// Test Harness: Handler implementations for output ports
// ----------------------------------------------------------------------

Fw::Buffer FprimeFramerTester::from_bufferAllocate_handler(FwIndexType portNum, FwSizeType size) {
    this->pushFromPortEntry_bufferAllocate(size);
    this->m_buffer.setData(this->m_buffer_slot);
    this->m_buffer.setSize(size);
    ::memset(this->m_buffer.getData(), 0, size);
    return this->m_buffer;
}

}  // namespace Svc
