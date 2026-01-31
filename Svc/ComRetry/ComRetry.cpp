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
      m_num_retries(3),
      m_retry_count(0),
      m_retry_state(WAITING_FOR_SEND),
      m_bufferState(Fw::Buffer::OwnershipState::OWNED) {}

ComRetry ::~ComRetry() {}

void ComRetry::configure(U32 num_retries) {
    this->m_num_retries = num_retries;
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ComRetry ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    FW_ASSERT(this->m_bufferState == Fw::Buffer::OwnershipState::OWNED);

    // When waiting for send, just pass the status up the stack as the buffer should still be upstream
    if (this->m_retry_state == WAITING_FOR_SEND) {
        FW_ASSERT(!this->m_buffer.isValid());
        this->comStatusOut_out(0, condition);
    }
    // Nominal case where delivery of buffer is successful, and everything is passed back up the stack
    else if ((this->m_retry_state == WAITING_FOR_STATUS) && (condition == Fw::Success::SUCCESS)) {
        FW_ASSERT(this->m_buffer.isValid());
        this->m_retry_state = WAITING_FOR_SEND;  // Successful transmission, reset state
        this->dataReturnOut_out(0, this->m_buffer, this->m_context);
        this->m_buffer = Fw::Buffer();  // Clear buffer
        this->comStatusOut_out(0, condition);
    }
    // When retrying, and "success" is received, this is the retry case
    else if ((this->m_retry_state == RETRYING) && (condition == Fw::Success::SUCCESS)) {
        FW_ASSERT(this->m_buffer.isValid());
        this->m_retry_count++;
        this->m_retry_state = WAITING_FOR_STATUS;
        this->m_bufferState = Fw::Buffer::OwnershipState::NOT_OWNED;
        this->dataOut_out(0, this->m_buffer, this->m_context);
    } else {
        // When a failure has been seen, it can **only** be in WAITING_FOR_STATUS state
        FW_ASSERT(this->m_retry_state == WAITING_FOR_STATUS);
        FW_ASSERT(condition == Fw::Success::FAILURE);

        // If we have retries left then switch to RETRYING, and wait for success
        if (this->m_retry_count < this->m_num_retries) {
            this->m_retry_state = RETRYING;
        }
        // If no retries left, pass failure back up the stack and reset state
        else {
            this->m_retry_state = WAITING_FOR_SEND;
            this->dataReturnOut_out(0, this->m_buffer, this->m_context);
            this->m_buffer = Fw::Buffer();  // Clear buffer
            this->comStatusOut_out(0, condition);
        }
    }
}

void ComRetry ::dataIn_handler(FwIndexType portNum, Fw::Buffer& buffer, const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_bufferState == Fw::Buffer::OwnershipState::OWNED);
    FW_ASSERT(this->m_retry_state == WAITING_FOR_SEND);
    this->m_bufferState = Fw::Buffer::OwnershipState::NOT_OWNED;
    this->m_retry_state = WAITING_FOR_STATUS;
    this->m_retry_count = 0;
    this->dataOut_out(0, buffer, context);
}

void ComRetry ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& buffer, const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_bufferState == Fw::Buffer::OwnershipState::NOT_OWNED);
    FW_ASSERT(this->m_retry_state == WAITING_FOR_STATUS);
    this->m_bufferState = Fw::Buffer::OwnershipState::OWNED;
    this->m_buffer = buffer;
    this->m_context = context;
}

}  // namespace Svc
