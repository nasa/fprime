/*
 * ComPacket.cpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#include <Fw/Com/ComPacket.hpp>

namespace Fw {

ComPacket::ComPacket() : m_type(ComPacketType::FW_PACKET_UNKNOWN) {}

ComPacket::~ComPacket() {}

SerializeStatus ComPacket::serializeBase(SerializeBufferBase& buffer) const {
    return buffer.serialize(static_cast<FwPacketDescriptorType>(this->m_type));
}

SerializeStatus ComPacket::deserializeBase(SerializeBufferBase& buffer) {
    FwPacketDescriptorType serVal;
    SerializeStatus stat = buffer.deserialize(serVal);
    if (FW_SERIALIZE_OK == stat) {
        this->m_type = static_cast<ComPacketType>(serVal);
    }
    return stat;
}

SerializeStatus ComPacket::serializeTo(SerializeBufferBase& buffer) const {
    // Default implementation for base class - derived classes should override this method
    return FW_SERIALIZE_FORMAT_ERROR;
}

SerializeStatus ComPacket::deserializeFrom(SerializeBufferBase& buffer) {
    // Default implementation for base class - derived classes should override this method
    return FW_DESERIALIZE_FORMAT_ERROR;
}

// Deprecated methods for backward compatibility
SerializeStatus ComPacket::serialize(SerializeBufferBase& buffer) const {
    return this->serializeTo(buffer);
}

SerializeStatus ComPacket::deserialize(SerializeBufferBase& buffer) {
    return this->deserializeFrom(buffer);
}

} /* namespace Fw */
