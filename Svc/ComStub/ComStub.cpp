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
    // If the synchronous send port is connected, send the data synchronously
    if (this->isConnected_drvSendOut_OutputPort(0)) {
        Drv::ByteStreamStatus sendStatus = Drv::ByteStreamStatus::SEND_RETRY;
        Fw::Success comSuccess = Fw::Success::FAILURE;

        for (FwIndexType i = 0; sendStatus == Drv::ByteStreamStatus::SEND_RETRY && i < RETRY_LIMIT; i++) {
            sendStatus = this->drvSendOut_out(0, sendBuffer);
        }
        if (sendStatus == Drv::ByteStreamStatus::SEND_RETRY) {
            Fw::Logger::log("ComStub RETRY_LIMIT exceeded, skipped sending data");
        } else if (sendStatus == Drv::ByteStreamStatus::OP_OK) {
            comSuccess = Fw::Success::SUCCESS;  // If send was successful, set success
        } else {
            this->m_reinitialize = true;
        }
        this->dataReturnOut_out(0, sendBuffer, context);
        this->comStatusOut_out(0, comSuccess);
    } else if (this->isConnected_drvAsyncSendOut_OutputPort(0)) {
        // If the asynchronous send port is connected, send the data asynchronously
        this->m_storedContext = context;  // Store the context of the current message for async callback
        this->drvAsyncSendOut_out(0, sendBuffer);
    } else {
        FW_ASSERT(0);  // Neither send port is connected, this should never happen
    }
}

void ComStub::drvConnected_handler(const FwIndexType portNum) {
    Fw::Success radioSuccess = Fw::Success::SUCCESS;
    if (this->isConnected_comStatusOut_OutputPort(0) && m_reinitialize) {
        this->m_reinitialize = false;
        this->comStatusOut_out(0, radioSuccess);
    }
}

void ComStub::drvReceiveIn_handler(const FwIndexType portNum,
                                   Fw::Buffer& recvBuffer,
                                   const Drv::ByteStreamStatus& recvStatus) {
    if (recvStatus.e == Drv::ByteStreamStatus::OP_OK) {
        ComCfg::FrameContext emptyContext;  // ComStub knows nothing about the received bytes, so use an empty context
        this->dataOut_out(0, recvBuffer, emptyContext);
    } else {
        this->drvReceiveReturnOut_out(0, recvBuffer);
    }
}

void ComStub::drvAsyncSendReturnIn_handler(FwIndexType portNum,   //!< The port number
                                           Fw::Buffer& fwBuffer,  //!< The buffer
                                           const Drv::ByteStreamStatus& sendStatus) {
    // This should never be called if the drvAsyncSendOut port is not connected
    FW_ASSERT(this->isConnected_drvAsyncSendOut_OutputPort(0));
    if (sendStatus != Drv::ByteStreamStatus::SEND_RETRY) {
        // Not retrying - return buffer ownership and send status
        this->dataReturnOut_out(0, fwBuffer, this->m_storedContext);
        this->m_reinitialize = sendStatus.e != Drv::ByteStreamStatus::OP_OK;
        this->m_retry_count = 0;  // Reset the retry count
        Fw::Success comSuccess =
            (sendStatus.e == Drv::ByteStreamStatus::OP_OK) ? Fw::Success::SUCCESS : Fw::Success::FAILURE;
        this->comStatusOut_out(0, comSuccess);
    } else {
        // Driver indicates we should retry (SEND_RETRY)
        if (this->m_retry_count < this->RETRY_LIMIT) {
            // If we have not yet retried more than the retry limit, attempt to retry
            this->m_retry_count++;
            this->drvAsyncSendOut_out(0, fwBuffer);
        } else {
            // If retried too many times, return buffer and log failure
            this->dataReturnOut_out(0, fwBuffer, this->m_storedContext);
            Fw::Success comStatus = Fw::Success::FAILURE;
            this->comStatusOut_out(0, comStatus);
            Fw::Logger::log("ComStub RETRY_LIMIT exceeded, skipped sending data");
            this->m_retry_count = 0;  // Reset the retry count
        }
    }
}

void ComStub ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer, const ComCfg::FrameContext& context) {
    this->drvReceiveReturnOut_out(0, fwBuffer);
}

}  // end namespace Svc
