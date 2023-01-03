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

ComStub::ComStub(const char* const compName) : ComStubComponentBase(compName), m_reinitialize(true) {}

void ComStub::init(const NATIVE_INT_TYPE instance) {
    ComStubComponentBase::init(instance);
}

ComStub::~ComStub() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Drv::SendStatus ComStub::comDataIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& sendBuffer) {
    FW_ASSERT(!this->m_reinitialize || !this->isConnected_comStatus_OutputPort(0));  // A message should never get here if we need to reinitialize is needed
    Drv::SendStatus driverStatus = Drv::SendStatus::SEND_RETRY;
    for (NATIVE_UINT_TYPE i = 0; driverStatus == Drv::SendStatus::SEND_RETRY && i < RETRY_LIMIT; i++) {
        driverStatus = this->drvDataOut_out(0, sendBuffer);
    }
    FW_ASSERT(driverStatus != Drv::SendStatus::SEND_RETRY);  // If it is still in retry state, there is no good answer
    Fw::Success comSuccess = (driverStatus.e == Drv::SendStatus::SEND_OK) ? Fw::Success::SUCCESS : Fw::Success::FAILURE;
    this->m_reinitialize = driverStatus.e != Drv::SendStatus::SEND_OK;
    if (this->isConnected_comStatus_OutputPort(0)) {
        this->comStatus_out(0, comSuccess);
    }
    return Drv::SendStatus::SEND_OK;  // Always send ok to deframer as it does not handle this anyway
}

void ComStub::drvConnected_handler(const NATIVE_INT_TYPE portNum) {
    Fw::Success radioSuccess = Fw::Success::SUCCESS;
    if (this->isConnected_comStatus_OutputPort(0) && m_reinitialize) {
        this->m_reinitialize = false;
        this->comStatus_out(0, radioSuccess);
    }
}

void ComStub::drvDataIn_handler(const NATIVE_INT_TYPE portNum,
                                Fw::Buffer& recvBuffer,
                                const Drv::RecvStatus& recvStatus) {
    this->comDataOut_out(0, recvBuffer, recvStatus);
}

}  // end namespace Svc
