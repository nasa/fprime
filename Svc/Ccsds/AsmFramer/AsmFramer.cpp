// ======================================================================
// \title  AsmFramer.cpp
// \author devin
// \brief  cpp file for AsmFramer component implementation class
// ======================================================================

#include "Svc/Ccsds/AsmFramer/AsmFramer.hpp"
#include <cstring>

namespace Svc {

namespace Ccsds {

//! Default ASM pattern: 0x1ACFFC1D for uncoded, convolutional, Reed-Solomon,
//! concatenated, rate-7/8 LDPC (Transfer Frame), and LDPC (SMTF stream) coded
//! data (CCSDS 131.0-B-5 Section 9.3.1)
static constexpr U8 ASM_DEFAULT[AsmFramer::ASM_DEFAULT_SIZE] = {0x1A, 0xCF, 0xFC, 0x1D};

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AsmFramer ::AsmFramer(const char* const compName)
    : AsmFramerComponentBase(compName), m_asm(), m_asmSize(ASM_DEFAULT_SIZE) {
    (void)::memcpy(this->m_asm, ASM_DEFAULT, ASM_DEFAULT_SIZE);
}

AsmFramer ::~AsmFramer() {}

void AsmFramer ::configure(const U8* asmPattern, FwSizeType asmSize) {
    FW_ASSERT(asmPattern != nullptr);
    FW_ASSERT(asmSize > 0, static_cast<FwAssertArgType>(asmSize));
    FW_ASSERT(asmSize <= ASM_MAX_SIZE, static_cast<FwAssertArgType>(asmSize));
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::OWNED, static_cast<FwAssertArgType>(this->m_bufferState));
    (void)::memcpy(this->m_asm, asmPattern, static_cast<size_t>(asmSize));
    this->m_asmSize = asmSize;
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AsmFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::OWNED, static_cast<FwAssertArgType>(this->m_bufferState));
    FW_ASSERT(data.getSize() <= sizeof(this->m_frameBuffer) - this->m_asmSize,
              static_cast<FwAssertArgType>(data.getSize()));

    // Prepend the ASM to the incoming transfer frame, producing a
    // Sync-Marked Transfer Frame (CCSDS 131.0-B-5 Section 9.4: the ASM
    // precedes the data it delimits). Frame data is not modified.
    (void)::memcpy(this->m_frameBuffer, this->m_asm, static_cast<size_t>(this->m_asmSize));
    (void)::memcpy(&this->m_frameBuffer[this->m_asmSize], data.getData(), static_cast<size_t>(data.getSize()));

    Fw::Buffer frameBuffer(this->m_frameBuffer, this->m_asmSize + data.getSize());
    this->m_bufferState = BufferOwnershipState::NOT_OWNED;
    this->dataOut_out(0, frameBuffer, context);
    this->dataReturnOut_out(0, data, context);  // return ownership of the original data buffer
}

void AsmFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void AsmFramer ::dataReturnIn_handler(FwIndexType portNum,
                                      Fw::Buffer& frameBuffer,
                                      const ComCfg::FrameContext& context) {
    // Assert that the returned buffer is the member buffer, and reclaim ownership
    FW_ASSERT(frameBuffer.getData() >= &this->m_frameBuffer[0]);
    FW_ASSERT(frameBuffer.getData() < &this->m_frameBuffer[0] + sizeof(this->m_frameBuffer));
    this->m_bufferState = BufferOwnershipState::OWNED;
}

}  // namespace Ccsds

}  // namespace Svc
