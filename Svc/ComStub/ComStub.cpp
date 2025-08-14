// ======================================================================
// \title  ComStub.cpp
// \author mstarch
// \brief  cpp file for ComStub component implementation class
// ======================================================================

#include <Fw/Logger/Logger.hpp>
#include <Svc/ComStub/ComStub.hpp>
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ComStub::ComStub(const char* const compName) : ComStubComponentBase(compName), m_reinitialize(true), m_retry_count(0) {}

ComStub::~ComStub() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void ComStub::dataIn_handler(const FwIndexType portNum, Fw::Buffer& sendBuffer, const ComCfg::FrameContext& context) {
    // A message should never get here if we need to reinitialize
    FW_ASSERT(!this->m_reinitialize || !this->isConnected_comStatusOut_OutputPort(0));
    if (this->isConnected_drvSendOut_OutputPort(0)) {
        this->handleSynchronousSend(sendBuffer, context);
    } else if (this->isConnected_drvAsyncSendOut_OutputPort(0)) {
        this->handleAsynchronousSend(sendBuffer, context);
    } else {
        FW_ASSERT(0);  // Neither send port is connected, this should never happen
    }
}

void ComStub::drvConnected_handler(const FwIndexType portNum) {
    if (this->isConnected_comStatusOut_OutputPort(0) && m_reinitialize) {
        Fw::Success radioSuccess = Fw::Success::SUCCESS;
        this->m_reinitialize = false;
        this->comStatusOut_out(0, radioSuccess);
    }
}

void ComStub::drvReceiveIn_handler(const FwIndexType portNum,
                                   Fw::Buffer& recvBuffer,
                                   const Drv::ByteStreamStatus& recvStatus) {
    if (recvStatus != Drv::ByteStreamStatus::OP_OK) {
        // Receive failed - return buffer without processing
        this->drvReceiveReturnOut_out(0, recvBuffer);
    } else {
        // Receive successful - forward data with empty context
        ComCfg::FrameContext emptyContext;  // ComStub knows nothing about the received bytes, empty context
        this->dataOut_out(0, recvBuffer, emptyContext);
    }
}

void ComStub::drvAsyncSendReturnIn_handler(FwIndexType portNum,   //!< The port number
                                           Fw::Buffer& fwBuffer,  //!< The buffer
                                           const Drv::ByteStreamStatus& sendStatus) {
    // This should never be called if the drvAsyncSendOut port is not connected
    FW_ASSERT(this->isConnected_drvAsyncSendOut_OutputPort(0));
    if (sendStatus == Drv::ByteStreamStatus::SEND_RETRY) {
        // Driver indicates we should retry
        this->handleAsyncRetry(fwBuffer);
    } else {
        // Return buffer ownership and send status
        this->dataReturnOut_out(0, fwBuffer, this->m_storedContext);
        this->m_reinitialize = (sendStatus.e != Drv::ByteStreamStatus::OP_OK);
        this->m_retry_count = 0;  // Reset retry count
        // Send Com status
        Fw::Success comSuccess =
            (sendStatus.e == Drv::ByteStreamStatus::OP_OK) ? Fw::Success::SUCCESS : Fw::Success::FAILURE;
        this->comStatusOut_out(0, comSuccess);
    }
}

void ComStub ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer, const ComCfg::FrameContext& context) {
    this->drvReceiveReturnOut_out(0, fwBuffer);
}

// ----------------------------------------------------------------------
// Helper method implementations
// ----------------------------------------------------------------------

void ComStub::handleSynchronousSend(Fw::Buffer& sendBuffer, const ComCfg::FrameContext& context) {
    Drv::ByteStreamStatus sendStatus = Drv::ByteStreamStatus::SEND_RETRY;
    Fw::Success comSuccess = Fw::Success::FAILURE;

    // Send to driver (and retry up to the retry limit)
    for (FwIndexType i = 0; sendStatus == Drv::ByteStreamStatus::SEND_RETRY && i < RETRY_LIMIT; i++) {
        sendStatus = this->drvSendOut_out(0, sendBuffer);
    }

    // Handle the send status
    if (sendStatus == Drv::ByteStreamStatus::OP_OK) {
        comSuccess = Fw::Success::SUCCESS;
    } else if (sendStatus == Drv::ByteStreamStatus::SEND_RETRY) {
        Fw::Logger::log("ComStub RETRY_LIMIT exceeded, skipped sending data");
    } else {
        // Other error - need to reinitialize
        this->m_reinitialize = true;
    }

    // Return buffer and send status
    this->dataReturnOut_out(0, sendBuffer, context);
    this->comStatusOut_out(0, comSuccess);
}

void ComStub::handleAsynchronousSend(Fw::Buffer& sendBuffer, const ComCfg::FrameContext& context) {
    this->m_storedContext = context;  // Store the context for async callback
    this->drvAsyncSendOut_out(0, sendBuffer);
}

void ComStub::handleAsyncRetry(Fw::Buffer& fwBuffer) {
    if (this->m_retry_count < this->RETRY_LIMIT) {
        // Attempt retry if under the limit
        this->m_retry_count++;
        this->drvAsyncSendOut_out(0, fwBuffer);
    } else {
        // Exceeded retry limit - return buffer and notify failure
        this->dataReturnOut_out(0, fwBuffer, this->m_storedContext);
        Fw::Success comStatus = Fw::Success::FAILURE;
        this->comStatusOut_out(0, comStatus);
        Fw::Logger::log("ComStub RETRY_LIMIT exceeded, skipped sending data");
        this->m_retry_count = 0;  // Reset retry count
    }
}

}  // end namespace Svc
