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
        toByteStreamDriver_out(portNum, fwBuffer);
    } else {
        this->log_WARNING_LO_DriverNotReady();
    }
}

void AsyncByteStreamBufferAdapter ::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    fromByteStreamDriverReturn_out(portNum, fwBuffer);
}

void AsyncByteStreamBufferAdapter ::fromByteStreamDriver_handler(FwIndexType portNum,
                                                                 Fw::Buffer& buffer,
                                                                 const Drv::ByteStreamStatus& status) {
    if (status == Drv::ByteStreamStatus::OP_OK) {
        bufferOut_out(portNum, buffer); 
    } else {
        this->log_WARNING_LO_DataReceiveError(status);
    }
    fromByteStreamDriverReturn_out(portNum, buffer); 
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
    bufferInReturn_out(portNum, buffer);
}

}  // namespace Drv
