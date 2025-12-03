// ======================================================================
// \title  AsyncByteStreamBufferAdapter.cpp
// \author bocchino
// \brief  cpp file for AsyncByteStreamBufferAdapter component implementation class
// ======================================================================

#include "Drv/AsyncByteStreamBufferAdapter/AsyncByteStreamBufferAdapter.hpp"

namespace Drv {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AsyncByteStreamBufferAdapter::AsyncByteStreamBufferAdapter(const char* const compName)
    : AsyncByteStreamBufferAdapterComponentBase(compName) {}

AsyncByteStreamBufferAdapter::~AsyncByteStreamBufferAdapter() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AsyncByteStreamBufferAdapter::bufferIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // TODO: If m_driverIsReady then send fwBuffer on toByteStreamDriver_out
    // TODO: Otherwise
    // TODO:   Log the error
    // TODO:   Send fwBuffer on bufferInReturn_out
}

void AsyncByteStreamBufferAdapter::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // TODO: Send fwBuffer on fromByteStreamDriverReturn_out
}

void AsyncByteStreamBufferAdapter::byteStreamDriverReady_handler(FwIndexType portNum) {
    this->m_driverIsReady = true;
}

void AsyncByteStreamBufferAdapter::fromByteStreamDriver_handler(FwIndexType portNum,
                                                                Fw::Buffer& buffer,
                                                                const Drv::ByteStreamStatus& status) {
    // TODO: If the status is OK, then send buffer on toByteStreamDriver_out
    // TODO: Otherwise log the error and send buffer on fromByteStreamDriverReturn_out
}

void AsyncByteStreamBufferAdapter::toByteStreamDriverReturn_handler(FwIndexType portNum,
                                                                    Fw::Buffer& buffer,
                                                                    const Drv::ByteStreamStatus& status) {
    // TODO: Send fwBuffer on bufferInReturn_out
}

}  // namespace Drv
