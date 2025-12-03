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
    // TODO: If m_driverIsReady then
    // TODO:   Send fwBuffer on toByteStreamDriver_out
    // TODO:   Check the return status. If there is an error, then log it to the Logger.
    // TODO: Otherwise log the error
    // TODO: Send fwBuffer on bufferInReturn_out
}

void ByteStreamBufferAdapter::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // TODO: Send fwBuffer on fromByteStreamDriverReturn_out
}

void ByteStreamBufferAdapter::fromByteStreamDriver_handler(FwIndexType portNum,
                                                           Fw::Buffer& buffer,
                                                           const Drv::ByteStreamStatus& status) {
    // TODO: If the status is OK, then send buffer on toByteStreamDriver_out
    // TODO: Otherwise log the error and send buffer on fromByteStreamDriverReturn_out
}

void ByteStreamBufferAdapter::byteStreamDriverReady_handler(FwIndexType portNum) {
    this->m_driverIsReady = true;
}

}  // namespace Drv
