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
    } else {
        this->log_WARNING_LO_DriverNotReady();
    }
    // ByteStreamDriver is Sync, so we return buffer back immediately regardless of status/readiness
    this->bufferInReturn_out(portNum, fwBuffer);
}

void ByteStreamBufferAdapter::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    this->fromByteStreamDriverReturn_out(portNum, fwBuffer);
}

void ByteStreamBufferAdapter::fromByteStreamDriver_handler(FwIndexType portNum,
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

void ByteStreamBufferAdapter::byteStreamDriverReady_handler(FwIndexType portNum) {
    this->m_driverIsReady = true;
}

}  // namespace Drv
