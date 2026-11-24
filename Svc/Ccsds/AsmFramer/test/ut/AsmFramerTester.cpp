// ======================================================================
// \title  AsmFramerTester.cpp
// \author devin
// \brief  cpp file for AsmFramer component test harness implementation class
// ======================================================================

#include "AsmFramerTester.hpp"

#include <cstring>

#include "Svc/Ccsds/TmFramer/TmFramer.hpp"
#include "Svc/Ccsds/Types/TMHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"

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

    // Largest standard pattern that fits the default Svc.AsmFramerCfg.AsmMaxSize: 128-bit rate-1/4 Turbo
    // (Blue Book 9.3.2, figure 9-4)
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
    // configure() minimum-size assert: a marker shorter than the shortest CCSDS pattern is rejected (sizes 1-3)
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(DEFAULT_ASM, AsmFramer::ASM_MIN_SIZE - 1)),
                              assertPattern(AsmFramer::ASM_MIN_SIZE - 1).toChar());
    // configure() null-pattern assert: a null pattern is only acceptable with size 0
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(nullptr, sizeof(DEFAULT_ASM))),
                              assertPattern(sizeof(DEFAULT_ASM)).toChar());
    // configure() maximum-size assert: a marker longer than Svc.AsmFramerCfg.AsmMaxSize is rejected
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(DEFAULT_ASM, AsmFramer::ASM_MAX_SIZE + 1)),
                              assertPattern(AsmFramer::ASM_MAX_SIZE + 1).toChar());
    // configure() OWNED assert: reconfiguring while the frame buffer is outstanding is rejected
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->component.configure(Fw::ConstByteArray(DEFAULT_ASM, sizeof(DEFAULT_ASM))),
                              assertPattern(AsmFramer::BufferOwnershipState::NOT_OWNED).toChar());
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::OWNED;
}

void AsmFramerTester ::testFrameSizeBounds() {
    ComCfg::FrameContext context;
    context.set_vcId(1);

    // An empty frame yields the bare marker
    U8 emptyData[1] = {};
    Fw::Buffer emptyBuffer(emptyData, 0);
    this->sendAndVerifyFrame(Fw::ConstByteArray(DEFAULT_ASM, sizeof(DEFAULT_ASM)), emptyBuffer, context, 0);

    // The largest configured frame (Svc.AsmFramerCfg.MaxFrameSize) is accepted with the largest
    // configured marker (Svc.AsmFramerCfg.AsmMaxSize), filling the internal buffer to its last byte
    U8 maxAsm[AsmFramer::ASM_MAX_SIZE] = {};
    for (U32 i = 0; i < sizeof(maxAsm); ++i) {
        maxAsm[i] = static_cast<U8>(0xC3u ^ i);
    }
    this->component.configure(Fw::ConstByteArray(maxAsm, sizeof(maxAsm)));
    U8 maxData[AsmFramer::FRAME_MAX_SIZE + 1] = {};
    for (U32 i = 0; i < sizeof(maxData); ++i) {
        maxData[i] = static_cast<U8>(i * 7u);
    }
    Fw::Buffer maxBuffer(maxData, AsmFramer::FRAME_MAX_SIZE);
    this->sendAndVerifyFrame(Fw::ConstByteArray(maxAsm, sizeof(maxAsm)), maxBuffer, context, 1);

    // dataIn FRAME_MAX_SIZE assert: one byte more than the largest supported frame is rejected
    Fw::Buffer tooLargeBuffer(maxData, AsmFramer::FRAME_MAX_SIZE + 1);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, tooLargeBuffer, context),
                              assertPattern(AsmFramer::FRAME_MAX_SIZE + 1).toChar());

    // dataIn null-data assert: a non-empty frame without backing storage is rejected
    Fw::Buffer nullBuffer(nullptr, 2);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, nullBuffer, context),
                              assertPattern(nullBuffer.getSize()).toChar());
}

void AsmFramerTester ::testDataReturn() {
    U8 bufferData[10] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    Fw::Buffer internalBuffer(this->component.m_frameBuffer, sizeof(this->component.m_frameBuffer));
    ComCfg::FrameContext context;
    context.set_vcId(2);

    // dataReturnIn NOT_OWNED assert: returning a buffer while no frame is outstanding is a protocol violation
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::OWNED);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, internalBuffer, context),
                              assertPattern(AsmFramer::BufferOwnershipState::OWNED).toChar());

    // Frame a buffer so that the internal buffer is legitimately outstanding
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::NOT_OWNED);
    ASSERT_from_dataReturnOut_SIZE(0);

    // dataReturnIn lower-bound assert: storage below the internal buffer is rejected
    Fw::Buffer belowBuffer(this->component.m_asm, 1);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, belowBuffer, context), assertPattern().toChar());
    // dataReturnIn upper-bound assert: storage above the internal buffer is rejected
    Fw::Buffer aboveBuffer(&this->component.m_frameBuffer[0] + sizeof(this->component.m_frameBuffer), 1);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, aboveBuffer, context), assertPattern().toChar());
    // dataReturnIn bounds asserts (either): the upstream frame's own storage is not the internal buffer
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, buffer, context), assertPattern().toChar());

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
    // dataIn OWNED assert: force state to be NOT_OWNED and test that assertion is triggered
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context),
                              assertPattern(AsmFramer::BufferOwnershipState::NOT_OWNED).toChar());
    this->component.m_bufferState = AsmFramer::BufferOwnershipState::OWNED;
    this->invoke_to_dataIn(0, buffer, context);  // this should work now
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::NOT_OWNED);
    // The upstream frame stays held (no dataReturnOut) while the com adapter has our buffer
    ASSERT_from_dataReturnOut_SIZE(0);
}

void AsmFramerTester ::testReentrantReturn() {
    // AosFramer resumes packing when its buffer returns and may emit its next frame from inside
    // dataReturnIn. The context returned for the first frame must be the one captured at dataIn,
    // not whatever the re-entrant dataIn stored.
    U8 bufferData[10] = {};
    Fw::Buffer buffer(bufferData, sizeof(bufferData));
    ComCfg::FrameContext context;
    context.set_vcId(2);
    // Distinct non-zero patterns, so a missing copy of the re-entrant frame is detected
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(0x11u + i);
    }
    for (U32 i = 0; i < sizeof(this->m_reentrantData); ++i) {
        this->m_reentrantData[i] = static_cast<U8>(0xE0u ^ i);
    }
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_dataReturnOut_SIZE(0);

    this->m_reenterOnReturn = true;
    Fw::Buffer returned(this->component.m_frameBuffer, this->fromPortHistory_dataOut->at(0).data.getSize());
    this->invoke_to_dataReturnIn(0, returned, context);
    ASSERT_FALSE(this->m_reenterOnReturn);

    // The first frame came back with its own context, after the re-entrant frame went out
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_from_dataReturnOut(0, buffer, context);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(1).context.get_vcId(), context.get_vcId() + REENTRANT_VCID_OFFSET);
    // The re-entrant frame itself was framed correctly: ASM followed by its payload
    ASSERT_EQ(this->fromPortHistory_dataOut->at(1).data.getData(), this->component.m_frameBuffer);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(1).data.getSize(), sizeof(DEFAULT_ASM) + sizeof(this->m_reentrantData));
    ASSERT_EQ(0, ::memcmp(this->component.m_frameBuffer, DEFAULT_ASM, sizeof(DEFAULT_ASM)));
    ASSERT_EQ(0, ::memcmp(&this->component.m_frameBuffer[sizeof(DEFAULT_ASM)], this->m_reentrantData,
                          sizeof(this->m_reentrantData)));
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::NOT_OWNED);

    // Return the re-entrant frame's buffer: its own context comes back
    Fw::Buffer returned2(this->component.m_frameBuffer, this->fromPortHistory_dataOut->at(1).data.getSize());
    this->invoke_to_dataReturnIn(0, returned2, context);
    ASSERT_from_dataReturnOut_SIZE(2);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(1).data.getData(), this->m_reentrantData);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(1).context.get_vcId(),
              context.get_vcId() + REENTRANT_VCID_OFFSET);
    ASSERT_EQ(this->component.m_bufferState, AsmFramer::BufferOwnershipState::OWNED);
}

void AsmFramerTester ::testTmFrameChain() {
    // Wire a real TmFramer upstream: tmFramer.dataOut -> asmFramer.dataIn. The TmFramer's
    // payload return is not under test and goes to a sink.
    TmFramer tmFramer("TmFramer");
    tmFramer.init(0);
    tmFramer.set_dataOut_OutputPort(0, this->component.get_dataIn_InputPort(0));
    Svc::InputComDataWithContextPort payloadReturnSink;
    payloadReturnSink.init();
    payloadReturnSink.addCallComp(this, AsmFramerTester::payloadReturnSinkHandler);
    tmFramer.set_dataReturnOut_OutputPort(0, &payloadReturnSink);

    U8 payload[TmDataFieldSize] = {};
    for (U32 i = 0; i < sizeof(payload); ++i) {
        payload[i] = static_cast<U8>(0x3Cu ^ (i * 5u));
    }
    Fw::Buffer payloadBuffer(payload, sizeof(payload));
    ComCfg::FrameContext context;
    context.set_vcId(1);

    // Expected bytes on the wire: the ASM, then the TM Transfer Frame (CCSDS 132.0-B-3 Section 4.1).
    // Primary header and data field are built independently of both framers; the FECF uses the
    // shared CRC16 utility
    U8 expected[sizeof(DEFAULT_ASM) + ComCfg::TmFrameFixedSize] = {};
    (void)::memcpy(expected, DEFAULT_ASM, sizeof(DEFAULT_ASM));
    U8* const tmFrame = &expected[sizeof(DEFAULT_ASM)];
    tmFrame[0] = static_cast<U8>(ComCfg::SpacecraftId >> 4);  // version 0b00, spacecraft id (upper bits)
    tmFrame[1] = static_cast<U8>(((ComCfg::SpacecraftId & 0xF) << 4) | (context.get_vcId() << 1));  // scid, vcid, OCF 0
    tmFrame[2] = 0;     // master frame count
    tmFrame[3] = 0;     // virtual frame count
    tmFrame[4] = 0x18;  // data field status: segment length id 0b11, first header pointer 0 (upper bits)
    tmFrame[5] = 0x00;  // first header pointer 0 (lower bits)
    (void)::memcpy(&tmFrame[TMHeader::SERIALIZED_SIZE], payload, sizeof(payload));
    const U16 fecf = Utils::CRC16::compute(tmFrame, ComCfg::TmFrameFixedSize - TMTrailer::SERIALIZED_SIZE);
    tmFrame[ComCfg::TmFrameFixedSize - 2] = static_cast<U8>(fecf >> 8);
    tmFrame[ComCfg::TmFrameFixedSize - 1] = static_cast<U8>(fecf & 0xFF);

    tmFramer.get_dataIn_InputPort(0)->invoke(payloadBuffer, context);

    // The full SMTF matches byte for byte
    ASSERT_from_dataOut_SIZE(1);
    const Fw::Buffer& smtf = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(smtf.getSize(), sizeof(expected));
    for (FwSizeType i = 0; i < sizeof(expected); ++i) {
        ASSERT_EQ(smtf.getData()[i], expected[i]) << "SMTF byte " << i << " mismatch";
    }
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context, context);

    // Releasing the SMTF returns the TM frame, which its framer accepts back
    Fw::Buffer returned(smtf);
    this->invoke_to_dataReturnIn(0, returned, context);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).context, context);
    Fw::Buffer tmFrameBuffer = this->fromPortHistory_dataReturnOut->at(0).data;
    ASSERT_EQ(tmFrameBuffer.getSize(), static_cast<FwSizeType>(ComCfg::TmFrameFixedSize));
    tmFramer.get_dataReturnIn_InputPort(0)->invoke(tmFrameBuffer, context);
}

// ----------------------------------------------------------------------
// Handler overrides
// ----------------------------------------------------------------------

void AsmFramerTester ::from_dataReturnOut_handler(FwIndexType portNum,
                                                  Fw::Buffer& data,
                                                  const ComCfg::FrameContext& context) {
    if (this->m_reenterOnReturn) {
        this->m_reenterOnReturn = false;
        Fw::Buffer next(this->m_reentrantData, sizeof(this->m_reentrantData));
        ComCfg::FrameContext nextContext;
        nextContext.set_vcId(static_cast<U8>(context.get_vcId() + REENTRANT_VCID_OFFSET));
        this->invoke_to_dataIn(0, next, nextContext);
    }
    this->pushFromPortEntry_dataReturnOut(data, context);
}

void AsmFramerTester ::payloadReturnSinkHandler(Fw::PassiveComponentBase* callComp,
                                                FwIndexType portNum,
                                                Fw::Buffer& data,
                                                const ComCfg::FrameContext& context) {}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

Fw::String AsmFramerTester ::assertPattern() {
    return Fw::String("AsmFramer\\.cpp:[0-9]+\"\n");
}

Fw::String AsmFramerTester ::assertPattern(FwSizeType argument) {
    Fw::String pattern;
    const Fw::FormatStatus status = pattern.format("AsmFramer\\.cpp:[0-9]+\" %" PRI_FwSizeType "\n", argument);
    FW_ASSERT(status == Fw::FormatStatus::SUCCESS, static_cast<FwAssertArgType>(status));
    return pattern;
}

Fw::String AsmFramerTester ::assertPattern(AsmFramer::BufferOwnershipState state) {
    return assertPattern(static_cast<FwSizeType>(state));
}

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
