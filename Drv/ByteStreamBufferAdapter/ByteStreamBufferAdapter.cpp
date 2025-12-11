// ======================================================================
// \title  ByteStreamBufferAdapter.cpp
// \author bocchino
// \brief  cpp file for ByteStreamBufferAdapter component implementation class
// ======================================================================

#include "Drv/ByteStreamBufferAdapter/ByteStreamBufferAdapter.hpp"

namespace Drv {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ByteStreamBufferAdapter::ByteStreamBufferAdapter(const char* const compName)
    : ByteStreamBufferAdapterComponentBase(compName) {}

ByteStreamBufferAdapter::~ByteStreamBufferAdapter() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ByteStreamBufferAdapter::bufferIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    if (this->m_driverIsReady) {
        Drv::ByteStreamStatus status = toByteStreamDriver_out(portNum, fwBuffer);
        if (status != Drv::ByteStreamStatus::OP_OK) {
            this->log_WARNING_LO_DataSendError(status);
        }
        bufferInReturn_out(portNum, fwBuffer);
    } else {
        this->log_WARNING_LO_DriverNotReady();
    }
}

void ByteStreamBufferAdapter::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    fromByteStreamDriverReturn_out(portNum, fwBuffer);
}

void ByteStreamBufferAdapter::fromByteStreamDriver_handler(FwIndexType portNum,
                                                           Fw::Buffer& buffer,
                                                           const Drv::ByteStreamStatus& status) {
    if (status == Drv::ByteStreamStatus::OP_OK) {
        bufferOut_out(portNum, buffer); 
    } else {
        this->log_WARNING_LO_DataReceiveError(status);
    }
    fromByteStreamDriverReturn_out(portNum, buffer); 
}

void ByteStreamBufferAdapter::byteStreamDriverReady_handler(FwIndexType portNum) {
    this->m_driverIsReady = true;
}

}  // namespace Drv
