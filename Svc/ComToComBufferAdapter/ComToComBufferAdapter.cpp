// ======================================================================
// \title  ComToComBufferAdapter.cpp
// \brief  cpp file for ComToComBufferAdapter component implementation class
// ======================================================================

#include <Fw/FPrimeBasicTypes.hpp>
#include <Svc/ComToComBufferAdapter/ComToComBufferAdapter.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ComToComBufferAdapter ::ComToComBufferAdapter(const char* compName) : ComToComBufferAdapterComponentBase(compName) {}

ComToComBufferAdapter ::~ComToComBufferAdapter() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void ComToComBufferAdapter ::comIn_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    FW_ASSERT(portNum == 0);

    // A legacy buffer must be large enough to hold the prepended descriptor
    FW_ASSERT(data.getSize() >= sizeof(FwPacketDescriptorType), static_cast<FwAssertArgType>(data.getSize()));

    // Read the FwPacketDescriptorType prepended at the front of the buffer
    FwPacketDescriptorType descriptor = 0;
    Fw::SerializeStatus status = data.deserializeTo(descriptor);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));

    // The remaining bytes (after the descriptor) form the payload
    const U8* payload = data.getBuffAddr() + sizeof(FwPacketDescriptorType);
    const FwSizeType payloadSize = data.getSize() - sizeof(FwPacketDescriptorType);
    Fw::ComBuffer payloadBuffer(payload, payloadSize);

    // Forward the payload with the descriptor carried as the explicit APID
    const ComCfg::Apid packetType(static_cast<ComCfg::Apid::T>(descriptor));
    this->comBufferSendOut_out(0, payloadBuffer, packetType, context);
}

}  // namespace Svc
