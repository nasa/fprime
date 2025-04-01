// ======================================================================
// \title  FprimePacketizer.cpp
// \author thomas-bc
// \brief  cpp file for FprimePacketizer component implementation class
// ======================================================================

#include "Svc/FprimePacketizer/FprimePacketizer.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

FprimePacketizer ::FprimePacketizer(const char* const compName)
    : FprimePacketizerComponentBase(compName), m_packetBuffer(m_internalBuffer, sizeof(m_internalBuffer)) {}

FprimePacketizer ::~FprimePacketizer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void FprimePacketizer ::comBufferIn_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    FW_ASSERT(data.getBuffLength() < FW_COM_BUFFER_MAX_SIZE);
    this->packetizeData(data.getBuffAddr(), data.getBuffLength(), NeedTypeSerialization::NO,
                        Fw::ComPacket::FW_PACKET_UNKNOWN);
}

void FprimePacketizer ::fileBufferIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    FW_ASSERT(data.getBuffLength() < FW_COM_BUFFER_MAX_SIZE);
    this->packetizeData(fwBuffer.getData(), fwBuffer.getSize(), NeedTypeSerialization::YES,
                        Fw::ComPacket::FW_PACKET_FILE);
    this->fileBufferReturn_out(0, fwBuffer);  // hand ownership back to FileDownlink component
}

void FprimePacketizer ::rawDataIn_handler(FwIndexType portNum, Fw::Buffer& data, Fw::Buffer& context) {
    if (data.getSize() > INTERNAL_PACKET_BUFF_MAX_SIZE) {
        Fw::Logger::log("FprimePacketizer: rawDataIn buffer too large, not sending (size=%d)\n", data.getSize());
    } else {
        this->packetizeData(data.getData(), data.getSize(), NeedTypeSerialization::YES, Fw::ComPacket::FW_PACKET_UNKNOWN);
    }
    // What to check size against? Should we allocate a new buffer if size is too big? (probably yes?)
    // memory ownership... what if FprimePacketizer accepts no ownership? Makes interface easier to think about
    // copies out all the time, and ownership is left to the caller
}

void FprimePacketizer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

void FprimePacketizer::packetizeData(const U8* const data,
                                     const FwSizeType size,
                                     FprimePacketizer::NeedTypeSerialization needs_type_serialization,
                                     Fw::ComPacket::ComPacketType packet_type) {
    FW_ASSERT(data != nullptr);
    FW_ASSERT(size < std::numeric_limits<Fw::Buffer::SizeType>::max(), static_cast<FwAssertArgType>(size));

    Fw::Buffer::SizeType packetSize = static_cast<Fw::Buffer::SizeType>(size) +
                                      (needs_type_serialization == NeedTypeSerialization::YES ? sizeof(I32) : 0);
    Fw::SerializeBufferBase& serializer = m_packetBuffer.getSerializeRepr();
    m_packetBuffer.setSize(packetSize);

    Fw::SerializeStatus status;

    // Serialize packet type if needed, otherwise it *must* be present in the data
    if (needs_type_serialization == NeedTypeSerialization::YES) {
        status = serializer.serialize(static_cast<I32>(packet_type));  // I32 used for enum storage
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    }

    status = serializer.serialize(data, size, Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    Fw::Buffer context;
    this->packetOut_out(0, m_packetBuffer, context);
    // TODO: should we zero the buffer here out of safety ?
}

}  // namespace Svc
