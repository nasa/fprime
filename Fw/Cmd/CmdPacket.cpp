/*
 * CmdPacket.cpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#include <Fw/Cmd/CmdPacket.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstdio>

namespace Fw {

CmdPacket::CmdPacket() : m_opcode(0) {
    this->m_type = ComPacketType::FW_PACKET_COMMAND;
}

CmdPacket::~CmdPacket() {}

// New serialization interface methods
SerializeStatus CmdPacket::serializeTo(SerializeBufferBase& buffer) const {
    // Shouldn't be called, no use case for serializing CmdPackets in FSW (currently)
    FW_ASSERT(0);
    return FW_SERIALIZE_OK;  // for compiler
}

SerializeStatus CmdPacket::deserializeFrom(SerializeBufferBase& buffer) {
    SerializeStatus stat = ComPacket::deserializeBase(buffer);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    // double check packet type
    if (this->m_type != ComPacketType::FW_PACKET_COMMAND) {
        return FW_DESERIALIZE_TYPE_MISMATCH;
    }

    stat = buffer.deserializeTo(this->m_opcode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    // if non-empty, copy data
    if (buffer.getBuffLeft()) {
        // copy the serialized arguments to the buffer
        stat = buffer.copyRaw(this->m_argBuffer, buffer.getBuffLeft());
    }

    return stat;
}

FwOpcodeType CmdPacket::getOpCode() const {
    return this->m_opcode;
}

CmdArgBuffer& CmdPacket::getArgBuffer() {
    return this->m_argBuffer;
}

} /* namespace Fw */
