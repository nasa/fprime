// ======================================================================
// \title  ComRetry.cpp
// \author valdaarhun
// \brief  cpp file for ComRetry component implementation class
// ======================================================================

#include "Svc/ComRetry/ComRetry.hpp"
#include "ComRetry.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ComRetry ::ComRetry(const char* const compName)
    : ComRetryComponentBase(compName),
      m_num_retries(1),
      m_retry_count(0),
      m_bufferState(Fw::Buffer::OwnershipState::OWNED) {}

ComRetry ::~ComRetry() {}

void ComRetry::configure(U32 num_retries) {
    this->m_num_retries = num_retries;
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ComRetry ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (condition == Fw::Success::SUCCESS) {
        this->m_retry_count = 0;
        this->dataReturnOut_out(0, this->m_buffer, this->m_context);
        this->comStatusOut_out(0, condition);
    }
    // Delivery of last message failed
    else if (this->m_retry_count < this->m_num_retries) {
        FW_ASSERT(this->m_bufferState == Fw::Buffer::OwnershipState::OWNED);
        this->m_retry_count++;
        this->m_bufferState = Fw::Buffer::OwnershipState::NOT_OWNED;
        this->dataOut_out(0, this->m_buffer, this->m_context);
    }
    // All retries failed, send FAILURE to upstream component
    else {
        this->m_retry_count = 0;
        condition = Fw::Success::FAILURE;
        this->dataReturnOut_out(0, this->m_buffer, this->m_context);
        this->comStatusOut_out(0, condition);
    }
}

void ComRetry ::dataIn_handler(FwIndexType portNum, Fw::Buffer& buffer, const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_bufferState == Fw::Buffer::OwnershipState::OWNED);
    this->m_bufferState = Fw::Buffer::OwnershipState::NOT_OWNED;
    this->dataOut_out(0, buffer, context);
}

void ComRetry ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& buffer, const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_bufferState == Fw::Buffer::OwnershipState::NOT_OWNED);
    this->m_bufferState = Fw::Buffer::OwnershipState::OWNED;
    this->m_buffer = buffer;
    this->m_context = context;
}

}  // namespace Svc
