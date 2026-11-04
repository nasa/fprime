// ======================================================================
// \title  AsmFramerTester.cpp
// \author devin
// \brief  cpp file for AsmFramer component test harness implementation class
// ======================================================================

#include "AsmFramerTester.hpp"
#include <cstring>

namespace Svc {

namespace Ccsds {

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
    // CCSDS 131.0-B-5 Section 9.3.1 ASM pattern
    const U8 expectedAsm[AsmFramer::ASM_DEFAULT_SIZE] = {0x1A, 0xCF, 0xFC, 0x1D};
    U8 bufferData[100] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    context.set_vcId(5);  // non-default so a dropped/reset context is detected on dataOut/dataReturnOut
    context.set_apid(ComCfg::Apid::FW_PACKET_TELEM);

    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }
    this->sendAndVerifyFrame(expectedAsm, sizeof(expectedAsm), buffer, context, 0);
}

void AsmFramerTester ::testConfiguredAsm() {
    // CCSDS 131.0-B-5 Section 9.3 rate-1/2 Turbo / TF-LDPC 1/2, 2/3, 4/5 pattern (64-bit)
    const U8 turboAsm[8] = {0x03, 0x47, 0x76, 0xC7, 0x27, 0x28, 0x95, 0xB0};
    this->component.configure(Fw::ConstByteArray(turboAsm, sizeof(turboAsm)));

    U8 bufferData[32] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    context.set_vcId(3);
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(0xA5u ^ i);
    }
    this->sendAndVerifyFrame(turboAsm, sizeof(turboAsm), buffer, context, 0);

    // Maximum supported size (128-bit): rate-1/4 Turbo pattern (Blue Book 9.3.2, figure 9-4)
    const U8 rate14Asm[AsmFramer::ASM_MAX_SIZE] = {0x03, 0x47, 0x76, 0xC7, 0x27, 0x28, 0x95, 0xB0,
                                                   0xFC, 0xB8, 0x89, 0x38, 0xD8, 0xD7, 0x6A, 0x4F};
    this->component.configure(Fw::ConstByteArray(rate14Asm, sizeof(rate14Asm)));
    this->sendAndVerifyFrame(rate14Asm, sizeof(rate14Asm), buffer, context, 1);
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
    this->sendAndVerifyFrame(nullptr, 0, buffer, context, 0);

    // Re-enabling the default-size marker restores framing
    const U8 defaultAsm[AsmFramer::ASM_DEFAULT_SIZE] = {0x1A, 0xCF, 0xFC, 0x1D};
    this->component.configure(Fw::ConstByteArray(defaultAsm, sizeof(defaultAsm)));
    this->sendAndVerifyFrame(defaultAsm, sizeof(defaultAsm), buffer, context, 1);
}

void AsmFramerTester ::testConfigureInvalid() {
    const U8 pattern[AsmFramer::ASM_DEFAULT_SIZE] = {0x1A, 0xCF, 0xFC, 0x1D};
    // A null pattern is only acceptable with size 0
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(nullptr, sizeof(pattern))), "AsmFramer.cpp");
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(pattern, AsmFramer::ASM_MAX_SIZE + 1)),
                              "AsmFramer.cpp");
    // Reconfiguring while the frame buffer is outstanding is rejected
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(pattern, sizeof(pattern))), "AsmFramer.cpp");
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::OWNED;
    // The default marker is untouched by the rejected calls
    ASSERT_EQ(this->component.m_asmSize, AsmFramer::ASM_DEFAULT_SIZE);
    ASSERT_EQ(0, ::memcmp(this->component.m_asm, pattern, sizeof(pattern)));
}

void AsmFramerTester ::testFrameSizeBounds() {
    const U8 expectedAsm[AsmFramer::ASM_DEFAULT_SIZE] = {0x1A, 0xCF, 0xFC, 0x1D};
    ComCfg::FrameContext context;
    context.set_vcId(1);

    // An empty frame yields the bare marker
    U8 emptyData[1] = {};
    Fw::Buffer emptyBuffer(emptyData, 0);
    this->sendAndVerifyFrame(expectedAsm, sizeof(expectedAsm), emptyBuffer, context, 0);

    // The largest frame either upstream framer emits is accepted with the largest marker
    const U8 rate14Asm[AsmFramer::ASM_MAX_SIZE] = {0x03, 0x47, 0x76, 0xC7, 0x27, 0x28, 0x95, 0xB0,
                                                   0xFC, 0xB8, 0x89, 0x38, 0xD8, 0xD7, 0x6A, 0x4F};
    this->component.configure(Fw::ConstByteArray(rate14Asm, sizeof(rate14Asm)));
    static_assert(AsmFramer::FRAME_MAX_SIZE >= ComCfg::AosMaxFrameFixedSize, "AOS frames must fit");
    static_assert(AsmFramer::FRAME_MAX_SIZE >= ComCfg::TmFrameFixedSize, "TM frames must fit");
    U8 maxData[AsmFramer::FRAME_MAX_SIZE + 1] = {};
    for (U32 i = 0; i < sizeof(maxData); ++i) {
        maxData[i] = static_cast<U8>(i * 7u);
    }
    Fw::Buffer maxBuffer(maxData, AsmFramer::FRAME_MAX_SIZE);
    this->sendAndVerifyFrame(rate14Asm, sizeof(rate14Asm), maxBuffer, context, 1);

    // One byte more than the largest supported frame is rejected
    Fw::Buffer tooLargeBuffer(maxData, AsmFramer::FRAME_MAX_SIZE + 1);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, tooLargeBuffer, context), "AsmFramer.cpp");
}

void AsmFramerTester ::testDataReturn() {
    U8 bufferData[10] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    Fw::Buffer internalBuffer(this->component.m_frameBuffer, sizeof(this->component.m_frameBuffer));
    ComCfg::FrameContext defaultContext;

    // Returning a buffer while no frame is outstanding is a protocol violation
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::OWNED);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, internalBuffer, defaultContext), "AsmFramer.cpp");

    // Send a buffer that is not the internal buffer of the component, and expect an assertion
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, buffer, defaultContext), "AsmFramer.cpp");

    // Now send the expected buffer and expect state to go back to OWNED
    this->invoke_to_dataReturnIn(0, internalBuffer, defaultContext);
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::OWNED);
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
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void AsmFramerTester ::sendAndVerifyFrame(const U8* expectedAsm,
                                          FwSizeType expectedAsmSize,
                                          Fw::Buffer& frame,
                                          const ComCfg::FrameContext& context,
                                          U32 index) {
    this->invoke_to_dataIn(0, frame, context);

    // The SMTF is the ASM followed by the unmodified frame (Blue Book 9.4), carrying the input context
    ASSERT_from_dataOut_SIZE(index + 1);
    const Fw::Buffer& outBuffer = this->fromPortHistory_dataOut->at(index).data;
    ASSERT_EQ(outBuffer.getData(), this->component.m_frameBuffer);
    ASSERT_EQ(outBuffer.getSize(), expectedAsmSize + frame.getSize());
    for (FwSizeType i = 0; i < expectedAsmSize; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], expectedAsm[i]) << "ASM byte " << i << " mismatch";
    }
    for (FwSizeType i = 0; i < frame.getSize(); ++i) {
        ASSERT_EQ(outBuffer.getData()[expectedAsmSize + i], frame.getData()[i]) << "Payload byte " << i << " modified";
    }
    ASSERT_EQ(this->fromPortHistory_dataOut->at(index).context, context);

    // The original buffer (identity, size, and context) is returned to its sender
    ASSERT_from_dataReturnOut_SIZE(index + 1);
    ASSERT_from_dataReturnOut(index, frame, context);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(index).data.getData(), frame.getData());

    // The internal buffer is outstanding until returned
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::NOT_OWNED);
    Fw::Buffer returned(outBuffer);
    this->invoke_to_dataReturnIn(0, returned, context);
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::OWNED);
}

}  // namespace Ccsds

}  // namespace Svc
