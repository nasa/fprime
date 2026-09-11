// ======================================================================
// \title  TmFramerTester.cpp
// \author thomas-bc
// \brief  cpp file for TmFramer component test harness implementation class
// ======================================================================

#include "TmFramerTester.hpp"
#include "Svc/Ccsds/Types/TMHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMTrailerSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

TmFramerTester ::TmFramerTester()
    : TmFramerGTestBase("TmFramerTester", TmFramerTester::MAX_HISTORY_SIZE), component("TmFramer") {
    this->initComponents();
    this->connectPorts();
}

TmFramerTester ::~TmFramerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void TmFramerTester ::testComStatusPassthrough() {
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

void TmFramerTester ::fillDataField(Fw::Buffer& buffer) {
    ASSERT_EQ(buffer.getSize(), static_cast<FwSizeType>(TmFramer::TmPayloadCapacity));
    U8* const bufferData = buffer.getData();
    for (FwSizeType i = 0; i < buffer.getSize(); ++i) {
        bufferData[i] = static_cast<U8>(i & 0xFF);
    }
}

void TmFramerTester ::testNominalFraming() {
    U8 bufferData[TmFramer::TmPayloadCapacity];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;
    this->fillDataField(buffer);

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
    ASSERT_EQ(this->component.m_masterFrameCount, outMcCount + 1);
    ASSERT_EQ(this->component.m_virtualFrameCount, outVcCount + 1);

    // The data field is carried exactly as delivered
    for (FwSizeType i = 0; i < sizeof(bufferData); ++i) {
        ASSERT_EQ(outBuffer.getData()[TMHeader::SERIALIZED_SIZE + i], bufferData[i]) << "Data mismatch at index " << i;
    }
}

void TmFramerTester ::testSeqCountWrapAround() {
    U8 bufferData[TmFramer::TmPayloadCapacity];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;
    this->fillDataField(buffer);

    // Intentionally set the sequence count to 250 and iterate 10 times
    // to test the wrap around of the sequence counts
    this->component.m_masterFrameCount = 250;
    this->component.m_virtualFrameCount = 250;
    U8 countWrapAround = 250;  // will wrap around to 0 after 255
    for (U32 iter = 0; iter < 10; iter++) {
        this->component.m_bufferState = TmFramer::BufferOwnershipState::OWNED;  // reset state to OWNED
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

void TmFramerTester ::testInputBufferTooLarge() {
    const FwSizeType tooLargeSize =
        ComCfg::TmFrameFixedSize;  // This is too large since we need room for header+trailer as well
    U8 bufferData[tooLargeSize];
    Fw::Buffer buffer(bufferData, tooLargeSize);
    ComCfg::FrameContext defaultContext;
    // Send a buffer larger than the
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, defaultContext), "TmFramer.cpp");
}

void TmFramerTester ::testDataReturn() {
    U8 bufferData[10];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;
    // Send a buffer that is not the internal buffer of the component, and expect an assertion
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, buffer, defaultContext), "TmFramer.cpp");

    // Now send the expected buffer and expect state to go back to OWNED
    this->component.m_bufferState = TmFramer::BufferOwnershipState::NOT_OWNED;
    Fw::Buffer internalBuffer(this->component.m_frameBuffer, sizeof(this->component.m_frameBuffer));
    this->invoke_to_dataReturnIn(0, internalBuffer, defaultContext);
    ASSERT_EQ(this->component.m_bufferState, TmFramer::BufferOwnershipState::OWNED);
}

void TmFramerTester ::testBufferOwnershipState() {
    U8 bufferData[TmFramer::TmPayloadCapacity];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    // force state to be NOT_OWNED and test that assertion is triggered
    this->component.m_bufferState = TmFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context), "TmFramer.cpp");
    this->component.m_bufferState = TmFramer::BufferOwnershipState::OWNED;
    this->invoke_to_dataIn(0, buffer, context);  // this should work now
    ASSERT_EQ(this->component.m_bufferState, TmFramer::BufferOwnershipState::NOT_OWNED);
}

void TmFramerTester ::testFirstHeaderPointerFromContext() {
    U8 bufferData[TmFramer::TmPayloadCapacity];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;

    // Default context: FHP of 0 (packet header at offset 0 of the data field)
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->getFrameFhp(this->fromPortHistory_dataOut->at(0).data.getData()), 0);

    // Context-provided FHP (spanning aggregator: first packet header at a non-zero offset)
    this->component.m_bufferState = TmFramer::BufferOwnershipState::OWNED;
    context.set_firstHeaderPointer(42);
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_EQ(this->getFrameFhp(this->fromPortHistory_dataOut->at(1).data.getData()), 42);

    // Continuation-only frame: no packet starts in this frame
    this->component.m_bufferState = TmFramer::BufferOwnershipState::OWNED;
    context.set_firstHeaderPointer(TMSubfields::FHP_NO_PACKET_START);
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_dataOut_SIZE(3);
    ASSERT_EQ(this->getFrameFhp(this->fromPortHistory_dataOut->at(2).data.getData()),
              static_cast<U16>(TMSubfields::FHP_NO_PACKET_START));

    // An FHP outside the 11-bit field is a caller error
    this->component.m_bufferState = TmFramer::BufferOwnershipState::OWNED;
    context.set_firstHeaderPointer(static_cast<U16>(TMSubfields::fhpMask + 1));
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context), "TmFramer.cpp");
}

void TmFramerTester ::testPartialDataFieldAsserts() {
    // The framer does not idle-fill: anything short of a full data field is a caller error
    const FwSizeType fullSize = TmFramer::TmPayloadCapacity;
    U8 bufferData[fullSize];
    ComCfg::FrameContext context;
    const FwSizeType shortSizes[] = {0, 1, 100, fullSize - 7, fullSize - 1};
    for (FwSizeType shortSize : shortSizes) {
        Fw::Buffer buffer(bufferData, shortSize);
        this->component.m_bufferState = TmFramer::BufferOwnershipState::OWNED;
        ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context), "TmFramer.cpp");
    }
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

U16 TmFramerTester::getFrameScId(U8* frameData) {
    return static_cast<U16>((frameData[0] & 0x3F) << 4 | (frameData[1] >> 4));
}
U8 TmFramerTester::getFrameVcId(U8* frameData) {
    return static_cast<U8>((frameData[1] & 0x0E) >> 1);
}
U8 TmFramerTester::getFrameMcCount(U8* frameData) {
    return frameData[2];
}
U8 TmFramerTester::getFrameVcCount(U8* frameData) {
    return frameData[3];
}
U16 TmFramerTester::getFrameFhp(U8* frameData) {
    return static_cast<U16>(((frameData[4] << 8) | frameData[5]) & TMSubfields::fhpMask);
}

}  // namespace Ccsds

}  // namespace Svc
