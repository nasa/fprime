// ======================================================================
// \title  ComBufferToComAdapter.cpp
// \brief  cpp file for ComBufferToComAdapter component implementation class
// ======================================================================

#include <Fw/FPrimeBasicTypes.hpp>
#include <Svc/ComBufferToComAdapter/ComBufferToComAdapter.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ComBufferToComAdapter ::ComBufferToComAdapter(const char* compName) : ComBufferToComAdapterComponentBase(compName) {}

ComBufferToComAdapter ::~ComBufferToComAdapter() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void ComBufferToComAdapter ::comBufferSendIn_handler(FwIndexType portNum,
                                                     Fw::ComBuffer& data,
                                                     const ComCfg::Apid& packetType,
                                                     U32 context) {
    FW_ASSERT(portNum == 0);

    Fw::ComBuffer legacyBuffer;

    // Prepend the descriptor (APID). An enum serializes as its representation
    // type (FwPacketDescriptorType), matching the legacy on-wire descriptor.
    Fw::SerializeStatus status = legacyBuffer.serializeFrom(packetType);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));

    // Append the original payload bytes (no length prefix)
    status = legacyBuffer.serializeFrom(data.getBuffAddr(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));

    this->comOut_out(0, legacyBuffer, context);
}

}  // namespace Svc
