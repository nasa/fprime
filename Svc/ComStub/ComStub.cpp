// ======================================================================
// \title  ComStub.cpp
// \author mstarch
// \brief  cpp file for ComStub component implementation class
// ======================================================================

#include <Svc/ComStub/ComStub.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include "Fw/Types/Assert.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ComStub ::ComStub(const char* const compName) : ComStubComponentBase(compName) {}

void ComStub ::init(const NATIVE_INT_TYPE instance) {
    ComStubComponentBase::init(instance);
}

ComStub ::~ComStub() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Drv::SendStatus ComStub ::comDataIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& sendBuffer) {
    Drv::SendStatus driverStatus = Drv::SendStatus::SEND_RETRY;
    for (NATIVE_UINT_TYPE i = 0; driverStatus == Drv::SendStatus::SEND_RETRY && i < retryLimit; i++) {
        driverStatus = drvDataOut_out(0, sendBuffer);
    }
    FW_ASSERT(driverStatus != Drv::SendStatus::SEND_RETRY); // If it is still in retry state, there is no good answer
    Svc::ComSendStatus radioReady = (driverStatus.e == Drv::SendStatus::SEND_OK) ? Svc::ComSendStatus::READY : Svc::ComSendStatus::FAIL;
    if (isConnected_comStatus_OutputPort(0)) {
        comStatus_out(0, radioReady);
    }
    return Drv::SendStatus::SEND_OK; // Always send ok to deframer as it does not handle this anyway
}


void ComStub ::drvConnected_handler(const NATIVE_INT_TYPE portNum) {
    Svc::ComSendStatus radioReady = Svc::ComSendStatus::READY;
    if (isConnected_comStatus_OutputPort(0)) {
        comStatus_out(0, radioReady);
    }
}

void ComStub ::drvDataIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer &recvBuffer, const Drv::RecvStatus &recvStatus) {
    comDataOut_out(0, recvBuffer, recvStatus);
}

}  // end namespace Svc
