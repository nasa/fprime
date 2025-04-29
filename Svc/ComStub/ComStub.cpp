// ======================================================================
// \title  ComStub.cpp
// \author mstarch
// \brief  cpp file for ComStub component implementation class
// ======================================================================

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

void ComStub::comDataIn_handler(const FwIndexType portNum, Fw::Buffer& sendBuffer, const ComCfg::FrameContext& context) {
    FW_ASSERT(!this->m_reinitialize || !this->isConnected_comStatusOut_OutputPort(0));  // A message should never get here if we need to reinitialize is needed
    this->m_storedContext = context;  // Store the context of the current message
    this->drvDataOut_out(0, sendBuffer);
}

void ComStub::drvConnected_handler(const FwIndexType portNum) {
    Fw::Success radioSuccess = Fw::Success::SUCCESS;
    if (this->isConnected_comStatusOut_OutputPort(0) && m_reinitialize) {
        this->m_reinitialize = false;
        this->comStatusOut_out(0, radioSuccess);
    }
}

void ComStub::drvDataIn_handler(const FwIndexType portNum,
                                Fw::Buffer& recvBuffer,
                                const Drv::ByteStreamStatus& recvStatus) {
    this->comDataOut_out(0, recvBuffer, recvStatus);
}

void ComStub ::dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                                        Fw::Buffer& fwBuffer,  //!< The buffer
                                        const Drv::ByteStreamStatus& sendStatus) {
    if (sendStatus != Drv::ByteStreamStatus::SEND_RETRY) {
        this->dataReturnOut_out(0, fwBuffer, this->m_storedContext);
        this->m_reinitialize = sendStatus.e != Drv::ByteStreamStatus::OP_OK;
        this->m_retry_count = 0; // Reset the retry count
        Fw::Success comSuccess = (sendStatus.e == Drv::ByteStreamStatus::OP_OK) ? Fw::Success::SUCCESS : Fw::Success::FAILURE;
        this->comStatusOut_out(0, comSuccess);
    } else {
        // If we have already retried more than the retry limit, there is no good answer
        FW_ASSERT(this->m_retry_count < this->RETRY_LIMIT, static_cast<FwAssertArgType>(this->m_retry_count));
        this->m_retry_count++;
        this->drvDataOut_out(0, fwBuffer);
    }
}

}  // end namespace Svc
