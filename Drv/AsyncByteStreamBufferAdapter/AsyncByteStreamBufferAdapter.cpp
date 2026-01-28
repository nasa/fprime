// ======================================================================
// \title  AsyncByteStreamBufferAdapter.cpp
// \author shahab
// \brief  cpp file for AsyncByteStreamBufferAdapter component implementation class
// ======================================================================

#include "Drv/AsyncByteStreamBufferAdapter/AsyncByteStreamBufferAdapter.hpp"

namespace Drv {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AsyncByteStreamBufferAdapter ::AsyncByteStreamBufferAdapter(const char* const compName)
    : AsyncByteStreamBufferAdapterComponentBase(compName) {}

AsyncByteStreamBufferAdapter ::~AsyncByteStreamBufferAdapter() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AsyncByteStreamBufferAdapter ::bufferIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    if (this->m_driverIsReady) {
        this->toByteStreamDriver_out(portNum, fwBuffer);
        // ByteStreamDriver is Async; don't return buffer here but wait for toByteStreamDriverReturn
    } else {
        // If not ready, log warning and return buffer back immediately
        this->log_WARNING_LO_DriverNotReady();
        this->bufferInReturn_out(portNum, fwBuffer);
    }
}

void AsyncByteStreamBufferAdapter ::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    this->fromByteStreamDriverReturn_out(portNum, fwBuffer);
}

void AsyncByteStreamBufferAdapter ::fromByteStreamDriver_handler(FwIndexType portNum,
                                                                 Fw::Buffer& buffer,
                                                                 const Drv::ByteStreamStatus& status) {
    if (status == Drv::ByteStreamStatus::OP_OK) {
        this->bufferOut_out(portNum, buffer);
        // buffer ownership will come back through bufferOutReturn so we do **not** return buffer here
    } else {
        // If error, log and return buffer back immediately
        this->log_WARNING_LO_DataReceiveError(status);
        this->fromByteStreamDriverReturn_out(portNum, buffer);
    }
}

void AsyncByteStreamBufferAdapter ::byteStreamDriverReady_handler(FwIndexType portNum) {
    this->m_driverIsReady = true;
}

void AsyncByteStreamBufferAdapter ::toByteStreamDriverReturn_handler(FwIndexType portNum,
                                                                     Fw::Buffer& buffer,
                                                                     const Drv::ByteStreamStatus& status) {
    if (status != Drv::ByteStreamStatus::OP_OK) {
        this->log_WARNING_LO_DataSendError(status);
    }
    this->bufferInReturn_out(portNum, buffer);
}

}  // namespace Drv
