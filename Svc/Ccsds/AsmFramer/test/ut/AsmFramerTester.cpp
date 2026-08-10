// ======================================================================
// \title  AsmFramerTester.cpp
// \author devin
// \brief  cpp file for AsmFramer component test harness implementation class
// ======================================================================

#include "AsmFramerTester.hpp"

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
    const U8 expectedAsm[4] = {0x1A, 0xCF, 0xFC, 0x1D};
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;

    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    this->invoke_to_dataIn(0, buffer, defaultContext);

    // The SMTF is the ASM followed by the unmodified frame (Blue Book 9.4)
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), sizeof(expectedAsm) + sizeof(bufferData));
    for (FwSizeType i = 0; i < sizeof(expectedAsm); ++i) {
        ASSERT_EQ(outBuffer.getData()[i], expectedAsm[i]) << "ASM byte " << i << " mismatch";
    }
    for (FwSizeType i = 0; i < sizeof(bufferData); ++i) {
        ASSERT_EQ(outBuffer.getData()[sizeof(expectedAsm) + i], bufferData[i]) << "Payload byte " << i << " modified";
    }
    // Context is passed through unmodified
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_vcId(), defaultContext.get_vcId());

    // The original buffer is returned to its sender
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), bufferData);
}

void AsmFramerTester ::testConfiguredAsm() {
    // CCSDS 131.0-B-5 Section 9.3 rate-1/2 Turbo / TF-LDPC 1/2, 2/3, 4/5 pattern (64-bit)
    const U8 turboAsm[8] = {0x03, 0x47, 0x76, 0xC7, 0x27, 0x28, 0x95, 0xB0};
    this->component.configure(turboAsm, sizeof(turboAsm));

    U8 bufferData[32];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(0xA5u ^ i);
    }

    this->invoke_to_dataIn(0, buffer, defaultContext);

    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), sizeof(turboAsm) + sizeof(bufferData));
    for (FwSizeType i = 0; i < sizeof(turboAsm); ++i) {
        ASSERT_EQ(outBuffer.getData()[i], turboAsm[i]) << "ASM byte " << i << " mismatch";
    }
    for (FwSizeType i = 0; i < sizeof(bufferData); ++i) {
        ASSERT_EQ(outBuffer.getData()[sizeof(turboAsm) + i], bufferData[i]) << "Payload byte " << i << " modified";
    }

    // Maximum supported size (128-bit): rate-1/4 Turbo pattern (Blue Book 9.3.2, figure 9-4)
    const U8 rate14Asm[16] = {0x03, 0x47, 0x76, 0xC7, 0x27, 0x28, 0x95, 0xB0,
                              0xFC, 0xB8, 0x89, 0x38, 0xD8, 0xD7, 0x6A, 0x4F};
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::OWNED;
    this->component.configure(rate14Asm, sizeof(rate14Asm));
    this->invoke_to_dataIn(0, buffer, defaultContext);
    ASSERT_from_dataOut_SIZE(2);
    outBuffer = this->fromPortHistory_dataOut->at(1).data;
    ASSERT_EQ(outBuffer.getSize(), sizeof(rate14Asm) + sizeof(bufferData));
    for (FwSizeType i = 0; i < sizeof(rate14Asm); ++i) {
        ASSERT_EQ(outBuffer.getData()[i], rate14Asm[i]) << "ASM byte " << i << " mismatch";
    }
}

void AsmFramerTester ::testConfigureInvalid() {
    const U8 pattern[4] = {0x1A, 0xCF, 0xFC, 0x1D};
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(nullptr, 4), "AsmFramer.cpp");
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(pattern, 0), "AsmFramer.cpp");
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(pattern, AsmFramer::ASM_MAX_SIZE + 1), "AsmFramer.cpp");
}

void AsmFramerTester ::testInputBufferTooLarge() {
    // Too large: no room for the ASM in the internal buffer
    const FwSizeType tooLargeSize = sizeof(this->component.m_frameBuffer) - AsmFramer::ASM_DEFAULT_SIZE + 1;
    U8 bufferData[sizeof(this->component.m_frameBuffer)];
    Fw::Buffer buffer(bufferData, tooLargeSize);
    ComCfg::FrameContext defaultContext;
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, defaultContext), "AsmFramer.cpp");
}

void AsmFramerTester ::testDataReturn() {
    U8 bufferData[10];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext defaultContext;
    // Send a buffer that is not the internal buffer of the component, and expect an assertion
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, buffer, defaultContext), "AsmFramer.cpp");

    // Now send the expected buffer and expect state to go back to OWNED
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::NOT_OWNED;
    Fw::Buffer internalBuffer(this->component.m_frameBuffer, sizeof(this->component.m_frameBuffer));
    this->invoke_to_dataReturnIn(0, internalBuffer, defaultContext);
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::OWNED);
}

void AsmFramerTester ::testBufferOwnershipState() {
    U8 bufferData[10];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    // force state to be NOT_OWNED and test that assertion is triggered
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context), "AsmFramer.cpp");
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::OWNED;
    this->invoke_to_dataIn(0, buffer, context);  // this should work now
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::NOT_OWNED);
}

}  // namespace Ccsds

}  // namespace Svc
