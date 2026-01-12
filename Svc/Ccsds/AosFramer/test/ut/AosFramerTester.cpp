// ======================================================================
// \title  AosFramerTester.cpp
// \author thomas-bc
// \brief  cpp file for AosFramer component test harness implementation class
// ======================================================================

#include "AosFramerTester.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMTrailerSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

AosFramerTester ::AosFramerTester()
    : AosFramerGTestBase("AosFramerTester", AosFramerTester::MAX_HISTORY_SIZE), component("AosFramer") {
    this->initComponents();
    this->connectPorts();

    // Configure our Framer
    this->component.configure(ComCfg::AosMaxFrameFixedSize, true);
}

AosFramerTester ::~AosFramerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void AosFramerTester ::testComStatusPassthrough() {
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

void AosFramerTester ::testNominalFraming() {
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;

    defaultContext.set_sendNow(true);

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
    const FwSizeType expectedFrameSize = ComCfg::TmFrameFixedSize;
    ASSERT_EQ(outBuffer.getSize(), expectedFrameSize);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_vcId(), defaultContext.get_vcId());

    U16 outScId = this->getFrameScId(outBuffer.getData());
    U8 outVcId = this->getFrameVcId(outBuffer.getData());
    U8 outMcCount = this->getFrameMcCount(outBuffer.getData());
    U8 outVcCount = this->getFrameVcCount(outBuffer.getData());

    const U16 expectedScId = ComCfg::SpacecraftId;
    ASSERT_EQ(outScId, expectedScId);
    ASSERT_EQ(outVcId, defaultContext.get_vcId());
    ASSERT_EQ(outMcCount, 0);
    ASSERT_EQ(outVcCount, 0);
    ASSERT_EQ(this->component.m_vcs[0].virtualFrameCount, outVcCount + 1);

    // Idle data should be filled at the offset of header + payload + the Space Packet Idle Packet header
    FwSizeType expectedIdleDataOffset =
        TMHeader::SERIALIZED_SIZE + sizeof(bufferData) + SpacePacketHeader::SERIALIZED_SIZE;

    // The frame is composed of the payload + a SpacePacket Idle Packet (Header + idle_pattern)
    const U8 idlePattern = this->component.SPP_IDLE_DATA_PATTERN;
    const FwSizeType ideDataEndOffset = ComCfg::TmFrameFixedSize - TMTrailer::SERIALIZED_SIZE;
    for (FwSizeType i = expectedIdleDataOffset; i < ideDataEndOffset; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], idlePattern)
            << "Idle data at index " << i << " does not match expected idle pattern";
    }
}

void AosFramerTester ::testSeqCountWrapAround() {
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;

    // Fill the buffer with some data
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Intentionally set the sequence count to 250 and iterate 10 times
    // to test the wrap around of the sequence counts
    this->component.m_vcs[0].virtualFrameCount = 250;
    U8 countWrapAround = 250;  // will wrap around to 0 after 255
    for (U32 iter = 0; iter < 10; iter++) {
        this->component.m_vcs[0].frame.state = AosFramer::BufferOwnershipState::OWNED;  // reset state to OWNED
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

void AosFramerTester ::testInputBufferTooLarge() {
    const FwSizeType tooLargeSize =
        ComCfg::TmFrameFixedSize;  // This is too large since we need room for header+trailer as well
    U8 bufferData[tooLargeSize];
    Fw::Buffer buffer(bufferData, tooLargeSize);
    ComCfg::FrameContext defaultContext;
    // Send a buffer larger than the
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, defaultContext), "AosFramer.cpp");
}

void AosFramerTester ::testDataReturn() {
    U8 bufferData[10];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;
    // Send a buffer that is not the internal buffer of the component, and expect an assertion
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, buffer, defaultContext), "AosFramer.cpp");

    // Now send the expected buffer and expect state to go back to OWNED
    this->component.m_vcs[0].frame.state = AosFramer::BufferOwnershipState::NOT_OWNED;
    Fw::Buffer internalBuffer(this->component.m_vcs[0].frame.backer, sizeof(this->component.m_vcs[0].frame.backer));
    this->invoke_to_dataReturnIn(0, internalBuffer, defaultContext);
    ASSERT_EQ(this->component.m_vcs[0].frame.state, AosFramer::BufferOwnershipState::OWNED);
}

void AosFramerTester ::testBufferOwnershipState() {
    U8 bufferData[10];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    // force state to be NOT_OWNED and test that assertion is triggered
    this->component.m_vcs[0].frame.state = AosFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context), "AosFramer.cpp");
    this->component.m_vcs[0].frame.state = AosFramer::BufferOwnershipState::OWNED;
    this->invoke_to_dataIn(0, buffer, context);  // this should work now
    ASSERT_EQ(this->component.m_vcs[0].frame.state, AosFramer::BufferOwnershipState::NOT_OWNED);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

U16 AosFramerTester::getFrameScId(U8* frameData) {
    return static_cast<U16>((frameData[0] & 0x3F) << 4 | (frameData[1] >> 4));
}
U8 AosFramerTester::getFrameVcId(U8* frameData) {
    return static_cast<U8>((frameData[1] & 0x0E) >> 1);
}
U8 AosFramerTester::getFrameMcCount(U8* frameData) {
    return frameData[2];
}
U8 AosFramerTester::getFrameVcCount(U8* frameData) {
    return frameData[3];
}

}  // namespace Ccsds

}  // namespace Svc
