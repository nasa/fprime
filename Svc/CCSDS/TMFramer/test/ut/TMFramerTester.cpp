// ======================================================================
// \title  TMFramerTester.cpp
// \author thomas-bc
// \brief  cpp file for TMFramer component test harness implementation class
// ======================================================================

#include "TMFramerTester.hpp"
#include "Svc/CCSDS/Types/TMFrameHeaderSerializableAc.hpp"
#include "Svc/CCSDS/Types/TMFrameTrailerSerializableAc.hpp"
#include "Svc/CCSDS/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {

namespace CCSDS {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TMFramerTester ::TMFramerTester()
    : TMFramerGTestBase("TMFramerTester", TMFramerTester::MAX_HISTORY_SIZE), component("TMFramer") {
    this->initComponents();
    this->connectPorts();
}

TMFramerTester ::~TMFramerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TMFramerTester ::testComStatusPassthrough() {
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

void TMFramerTester ::testNominalFraming() {
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;

    // Fill the buffer with some data
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Invoke the dataIn handler
    this->invoke_to_dataIn(0, buffer, defaultContext);

    // Check that the dataOut handler was called with the correct data
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ComCfg::FrameContext outContext = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(outBuffer.getSize(), ComCfg::FppConstant_TmFrameFixedSize::TmFrameFixedSize);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.getvcId(), defaultContext.getvcId());

    U16 outScId = this->getFrameScId(outBuffer.getData());
    U8 outVcId = this->getFrameVcId(outBuffer.getData());
    U8 outMcCount = this->getFrameMcCount(outBuffer.getData());
    U8 outVcCount = this->getFrameVcCount(outBuffer.getData());

    ASSERT_EQ(outScId, ComCfg::FppConstant_SpacecraftId::SpacecraftId);
    ASSERT_EQ(outVcId, defaultContext.getvcId());
    ASSERT_EQ(outMcCount, 0);
    ASSERT_EQ(outVcCount, 0);
    ASSERT_EQ(this->component.m_masterFrameCount, outMcCount + 1);
    ASSERT_EQ(this->component.m_virtualFrameCount, outVcCount + 1);

    // Idle data should be filled at the offset of header + payload + the Space Packet Idle Packet header
    FwSizeType expectedIdleDataOffset = TMFrameHeader::SERIALIZED_SIZE + sizeof(bufferData) + SpacePacketHeader::SERIALIZED_SIZE;

    // The frame is composed of the payload + a SpacePacket Idle Packet (Header + idle_pattern)
    for (FwSizeType i = expectedIdleDataOffset; i < ComCfg::FppConstant_TmFrameFixedSize::TmFrameFixedSize - TMFrameTrailer::SERIALIZED_SIZE; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], 0x44);
    }
}

void TMFramerTester ::testSeqCountWrapAround() {
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;

    // Fill the buffer with some data
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Intentionally set the sequence count to 250 and iterate 10 times
    // to test the wrap around of the sequence counts
    this->component.m_masterFrameCount = 250;
    this->component.m_virtualFrameCount = 250;
    U8 countWrapAround = 250; // will wrap around to 0 after 255
    for (U32 iter = 0; iter < 10; iter++) {
        this->invoke_to_dataIn(0, buffer, defaultContext);
        ASSERT_from_dataOut_SIZE(iter + 1);
        Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(iter).data;
        U8 outMcCount = this->getFrameMcCount(outBuffer.getData());
        U8 outVcCount = this->getFrameVcCount(outBuffer.getData());
        ASSERT_EQ(outMcCount, countWrapAround);
        ASSERT_EQ(outVcCount, countWrapAround);
        countWrapAround++;
    }
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

U16 TMFramerTester::getFrameScId(U8* frameData) {
    return static_cast<U16>((frameData[0] & 0x3F) << 4 | (frameData[1] >> 4));
}
U8 TMFramerTester::getFrameVcId(U8* frameData) {
    return (frameData[1] & 0x0E) >> 1;
}
U8 TMFramerTester::getFrameMcCount(U8* frameData) {
    return frameData[2];
}
U8 TMFramerTester::getFrameVcCount(U8* frameData) {
    return frameData[3];
}

}  // namespace CCSDS

}  // namespace Svc
