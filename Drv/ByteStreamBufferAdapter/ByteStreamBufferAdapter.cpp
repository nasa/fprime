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

ByteStreamBufferAdapter ::ByteStreamBufferAdapter(const char* const compName)
    : ByteStreamBufferAdapterComponentBase(compName) {}

ByteStreamBufferAdapter ::~ByteStreamBufferAdapter() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ByteStreamBufferAdapter ::bufferIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // TODO
}

void ByteStreamBufferAdapter ::bufferOutReturn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // TODO
}

void ByteStreamBufferAdapter ::byteStreamIn_handler(FwIndexType portNum,
                                                    Fw::Buffer& buffer,
                                                    const Drv::ByteStreamStatus& status) {
    // TODO
}

void ByteStreamBufferAdapter ::byteStreamReady_handler(FwIndexType portNum) {
    // TODO
}

}  // namespace Drv
