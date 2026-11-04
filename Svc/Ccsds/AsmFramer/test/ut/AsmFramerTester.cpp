// ======================================================================
// \title  AsmFramerTester.cpp
// \author devin
// \brief  cpp file for AsmFramer component test harness implementation class
// ======================================================================

#include "AsmFramerTester.hpp"

namespace Svc {

namespace Ccsds {

//! CCSDS 131.0-B-5 Section 9.3.1 ASM pattern, as the component must apply it by default
static const U8 DEFAULT_ASM[] = {0x1A, 0xCF, 0xFC, 0x1D};
static_assert(sizeof(DEFAULT_ASM) == AsmFramer::ASM_DEFAULT_SIZE, "default pattern size");

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

AsmFramerTester ::AsmFramerTester()
    : AsmFramerGTestBase("AsmFramerTester", AsmFramerTester::MAX_HISTORY_SIZE), component("AsmFramer") {
    this->initComponents();
    this->connectPorts();
}

AsmFramerTester ::~AsmFramerTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void AsmFramerTester ::testComStatusPassthrough() {
    Fw::Success inputStatus = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, inputStatus);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, inputStatus);  // at index 0, received SUCCESS
    inputStatus = Fw::Success::FAILURE;
    this->invoke_to_comStatusIn(0, inputStatus);
    ASSERT_from_comStatusOut_SIZE(2);
    ASSERT_from_comStatusOut(1, inputStatus);  // at index 1, received FAILURE
}

void AsmFramerTester ::testNominalFraming() {
    U8 bufferData[100] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    context.set_vcId(5);  // non-default so a dropped/reset context is detected on dataOut/dataReturnOut
    context.set_apid(ComCfg::Apid::FW_PACKET_TELEM);

    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }
    this->sendAndVerifyFrame(Fw::ConstByteArray(DEFAULT_ASM, sizeof(DEFAULT_ASM)), buffer, context, 0);
}

void AsmFramerTester ::testConfiguredAsm() {
    // CCSDS 131.0-B-5 Section 9.3 rate-1/2 Turbo / TF-LDPC 1/2, 2/3, 4/5 pattern (64-bit)
    const U8 turboAsm[] = {0x03, 0x47, 0x76, 0xC7, 0x27, 0x28, 0x95, 0xB0};
    this->component.configure(Fw::ConstByteArray(turboAsm, sizeof(turboAsm)));

    U8 bufferData[32] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    context.set_vcId(3);
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(0xA5u ^ i);
    }
    this->sendAndVerifyFrame(Fw::ConstByteArray(turboAsm, sizeof(turboAsm)), buffer, context, 0);

    // Maximum supported size (128-bit): rate-1/4 Turbo pattern (Blue Book 9.3.2, figure 9-4)
    const U8 rate14Asm[] = {0x03, 0x47, 0x76, 0xC7, 0x27, 0x28, 0x95, 0xB0,
                            0xFC, 0xB8, 0x89, 0x38, 0xD8, 0xD7, 0x6A, 0x4F};
    this->component.configure(Fw::ConstByteArray(rate14Asm, sizeof(rate14Asm)));
    this->sendAndVerifyFrame(Fw::ConstByteArray(rate14Asm, sizeof(rate14Asm)), buffer, context, 1);
}

void AsmFramerTester ::testDisabledAsm() {
    // Size 0 disables the marker: the frame is forwarded unchanged
    this->component.configure(Fw::ConstByteArray(nullptr, 0));

    U8 bufferData[64] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    context.set_vcId(7);
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(0x5Au + i);
    }
    this->sendAndVerifyFrame(Fw::ConstByteArray(nullptr, 0), buffer, context, 0);

    // Re-enabling the default-size marker restores framing
    this->component.configure(Fw::ConstByteArray(DEFAULT_ASM, sizeof(DEFAULT_ASM)));
    this->sendAndVerifyFrame(Fw::ConstByteArray(DEFAULT_ASM, sizeof(DEFAULT_ASM)), buffer, context, 1);
}

void AsmFramerTester ::testConfigureInvalid() {
    // A null pattern is only acceptable with size 0
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(nullptr, sizeof(DEFAULT_ASM))),
                              "AsmFramer.cpp");
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(DEFAULT_ASM, AsmFramer::ASM_MAX_SIZE + 1)),
                              "AsmFramer.cpp");
    // Reconfiguring while the frame buffer is outstanding is rejected
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(DEFAULT_ASM, sizeof(DEFAULT_ASM))),
                              "AsmFramer.cpp");
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::OWNED;
}

void AsmFramerTester ::testFrameSizeBounds() {
    ComCfg::FrameContext context;
    context.set_vcId(1);

    // An empty frame yields the bare marker
    U8 emptyData[1] = {};
    Fw::Buffer emptyBuffer(emptyData, 0);
    this->sendAndVerifyFrame(Fw::ConstByteArray(DEFAULT_ASM, sizeof(DEFAULT_ASM)), emptyBuffer, context, 0);

    // The largest configured frame (ComCfg.AsmFramerMaxFrameSize) is accepted with the largest marker
    const U8 rate14Asm[] = {0x03, 0x47, 0x76, 0xC7, 0x27, 0x28, 0x95, 0xB0,
                            0xFC, 0xB8, 0x89, 0x38, 0xD8, 0xD7, 0x6A, 0x4F};
    this->component.configure(Fw::ConstByteArray(rate14Asm, sizeof(rate14Asm)));
    U8 maxData[AsmFramer::FRAME_MAX_SIZE + 1] = {};
    for (U32 i = 0; i < sizeof(maxData); ++i) {
        maxData[i] = static_cast<U8>(i * 7u);
    }
    Fw::Buffer maxBuffer(maxData, AsmFramer::FRAME_MAX_SIZE);
    this->sendAndVerifyFrame(Fw::ConstByteArray(rate14Asm, sizeof(rate14Asm)), maxBuffer, context, 1);

    // One byte more than the largest supported frame is rejected
    Fw::Buffer tooLargeBuffer(maxData, AsmFramer::FRAME_MAX_SIZE + 1);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, tooLargeBuffer, context), "AsmFramer.cpp");

    // A non-empty frame without backing storage is rejected
    Fw::Buffer nullBuffer(nullptr, 1);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, nullBuffer, context), "AsmFramer.cpp");
}

void AsmFramerTester ::testDataReturn() {
    U8 bufferData[10] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    Fw::Buffer internalBuffer(this->component.m_frameBuffer, sizeof(this->component.m_frameBuffer));
    ComCfg::FrameContext context;
    context.set_vcId(2);

    // Returning a buffer while no frame is outstanding is a protocol violation
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::OWNED);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, internalBuffer, context), "AsmFramer.cpp");

    // Frame a buffer so that the internal buffer is legitimately outstanding
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::NOT_OWNED);
    ASSERT_from_dataReturnOut_SIZE(0);

    // Returning storage below or above the internal buffer is rejected (both bounds checks)
    Fw::Buffer belowBuffer(this->component.m_asm, 1);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, belowBuffer, context), "AsmFramer.cpp");
    Fw::Buffer aboveBuffer(&this->component.m_frameBuffer[0] + sizeof(this->component.m_frameBuffer), 1);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, aboveBuffer, context), "AsmFramer.cpp");
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, buffer, context), "AsmFramer.cpp");

    // Returning the internal buffer restores ownership and releases the upstream frame with the
    // context captured at dataIn, not the context the com adapter hands back
    ComCfg::FrameContext returnContext;
    returnContext.set_vcId(6);
    this->invoke_to_dataReturnIn(0, internalBuffer, returnContext);
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::OWNED);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, buffer, context);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), bufferData);
}

void AsmFramerTester ::testBufferOwnershipState() {
    U8 bufferData[10] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    // force state to be NOT_OWNED and test that assertion is triggered
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context), "AsmFramer.cpp");
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::OWNED;
    this->invoke_to_dataIn(0, buffer, context);  // this should work now
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::NOT_OWNED);
    // The upstream frame stays held (no dataReturnOut) while the com adapter has our buffer
    ASSERT_from_dataReturnOut_SIZE(0);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void AsmFramerTester ::sendAndVerifyFrame(const Fw::ConstByteArray& expectedAsm,
                                          Fw::Buffer& frame,
                                          const ComCfg::FrameContext& context,
                                          U32 index) {
    const FwSizeType expectedAsmSize = expectedAsm.size;
    this->invoke_to_dataIn(0, frame, context);

    // The SMTF is the ASM followed by the unmodified frame (Blue Book 9.4), carrying the input context
    ASSERT_from_dataOut_SIZE(index + 1);
    const Fw::Buffer& outBuffer = this->fromPortHistory_dataOut->at(index).data;
    ASSERT_EQ(outBuffer.getData(), this->component.m_frameBuffer);
    ASSERT_EQ(outBuffer.getSize(), expectedAsmSize + frame.getSize());
    for (FwSizeType i = 0; i < expectedAsmSize; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], expectedAsm.bytes[i]) << "ASM byte " << i << " mismatch";
    }
    for (FwSizeType i = 0; i < frame.getSize(); ++i) {
        ASSERT_EQ(outBuffer.getData()[expectedAsmSize + i], frame.getData()[i]) << "Payload byte " << i << " modified";
    }
    ASSERT_EQ(this->fromPortHistory_dataOut->at(index).context, context);

    // The internal buffer is outstanding, and the original buffer is held, until the SMTF buffer is returned
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::NOT_OWNED);
    ASSERT_from_dataReturnOut_SIZE(index);
    Fw::Buffer returned(outBuffer);
    ComCfg::FrameContext returnContext;  // a different context, to prove the dataIn context is forwarded
    returnContext.set_vcId(static_cast<U8>(context.get_vcId() + 1));
    this->invoke_to_dataReturnIn(0, returned, returnContext);
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::OWNED);

    // The original buffer (identity, size, and the dataIn context) is then returned to its sender
    ASSERT_from_dataReturnOut_SIZE(index + 1);
    ASSERT_from_dataReturnOut(index, frame, context);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(index).data.getData(), frame.getData());
}

}  // namespace Ccsds

}  // namespace Svc
