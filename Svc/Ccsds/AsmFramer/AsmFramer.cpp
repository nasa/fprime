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

// Out-of-class definitions so the constants may be odr-used (C++14)
constexpr FwSizeType AsmFramer::ASM_MAX_SIZE;
constexpr FwSizeType AsmFramer::ASM_DEFAULT_SIZE;
constexpr FwSizeType AsmFramer::FRAME_MAX_SIZE;

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AsmFramer ::AsmFramer(const char* const compName)
    : AsmFramerComponentBase(compName), m_asm(), m_asmSize(ASM_DEFAULT_SIZE), m_frameBuffer() {
    (void)::memcpy(this->m_asm, ASM_DEFAULT, ASM_DEFAULT_SIZE);
}

AsmFramer ::~AsmFramer() {}

void AsmFramer ::configure(const Fw::ConstByteArray& asmPattern) {
    FW_ASSERT(asmPattern.size <= ASM_MAX_SIZE, static_cast<FwAssertArgType>(asmPattern.size));
    FW_ASSERT((asmPattern.size == 0) || (asmPattern.bytes != nullptr), static_cast<FwAssertArgType>(asmPattern.size));
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::OWNED, static_cast<FwAssertArgType>(this->m_bufferState));
    if (asmPattern.size > 0) {
        (void)::memcpy(this->m_asm, asmPattern.bytes, static_cast<size_t>(asmPattern.size));
    }
    this->m_asmSize = asmPattern.size;
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AsmFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::OWNED, static_cast<FwAssertArgType>(this->m_bufferState));
    FW_ASSERT(data.getSize() <= FRAME_MAX_SIZE, static_cast<FwAssertArgType>(data.getSize()));
    FW_ASSERT((data.getSize() == 0) || (data.getData() != nullptr), static_cast<FwAssertArgType>(data.getSize()));

    // Prepend the ASM to the incoming transfer frame, producing a
    // Sync-Marked Transfer Frame (CCSDS 131.0-B-5 Section 9.4: the ASM
    // precedes the data it delimits). Frame data is not modified. With the
    // marker disabled (size 0) the frame is forwarded unchanged.
    if (this->m_asmSize > 0) {
        (void)::memcpy(this->m_frameBuffer, this->m_asm, static_cast<size_t>(this->m_asmSize));
    }
    if (data.getSize() > 0) {
        (void)::memcpy(&this->m_frameBuffer[this->m_asmSize], data.getData(), static_cast<size_t>(data.getSize()));
    }

    // Hold the upstream buffer until our own buffer comes back on dataReturnIn. Returning
    // it earlier would signal the upstream framer that the frame was consumed while the
    // com adapter may still hold it (asynchronous drivers), letting the upstream framer
    // emit its next frame into a buffer this component does not own.
    this->m_upstreamBuffer = data;
    this->m_upstreamContext = context;
    Fw::Buffer frameBuffer(this->m_frameBuffer, this->m_asmSize + data.getSize());
    this->m_bufferState = BufferOwnershipState::NOT_OWNED;
    this->dataOut_out(0, frameBuffer, context);
}

void AsmFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void AsmFramer ::dataReturnIn_handler(FwIndexType portNum,
                                      Fw::Buffer& frameBuffer,
                                      const ComCfg::FrameContext& context) {
    // Assert that a frame is outstanding and the returned buffer is the member buffer, then reclaim ownership
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::NOT_OWNED,
              static_cast<FwAssertArgType>(this->m_bufferState));
    FW_ASSERT(frameBuffer.getData() >= &this->m_frameBuffer[0]);
    FW_ASSERT(frameBuffer.getData() < &this->m_frameBuffer[0] + sizeof(this->m_frameBuffer));
    this->m_bufferState = BufferOwnershipState::OWNED;

    // Now that the com adapter has released our buffer, return the upstream frame
    Fw::Buffer upstream = this->m_upstreamBuffer;
    this->m_upstreamBuffer = Fw::Buffer();
    this->dataReturnOut_out(0, upstream, this->m_upstreamContext);
}

}  // namespace Ccsds

}  // namespace Svc
