/*
 * ComPacket.cpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#include <Fw/Com/ComPacket.hpp>

namespace Fw {

    ComPacket::ComPacket() : m_type(FW_PACKET_UNKNOWN) {
    }

    ComPacket::~ComPacket() {
    }

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


} /* namespace Fw */
