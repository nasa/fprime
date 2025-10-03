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

AsyncByteStreamBufferAdapter ::AsyncByteStreamBufferAdapter(const char* const compName)
    : AsyncByteStreamBufferAdapterComponentBase(compName) {}

AsyncByteStreamBufferAdapter ::~AsyncByteStreamBufferAdapter() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AsyncByteStreamBufferAdapter ::bufferIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // TODO
}

void AsyncByteStreamBufferAdapter ::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // TODO
}

void AsyncByteStreamBufferAdapter ::driverReady_handler(FwIndexType portNum) {
    // TODO
}

void AsyncByteStreamBufferAdapter ::fromDriver_handler(FwIndexType portNum,
                                                       Fw::Buffer& buffer,
                                                       const Drv::ByteStreamStatus& status) {
    // TODO
}

void AsyncByteStreamBufferAdapter ::toDriverReturn_handler(FwIndexType portNum,
                                                           Fw::Buffer& buffer,
                                                           const Drv::ByteStreamStatus& status) {
    // TODO
}

}  // namespace Drv
