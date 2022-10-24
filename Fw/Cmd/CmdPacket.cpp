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
        this->m_type = FW_PACKET_COMMAND;
    }

    CmdPacket::~CmdPacket() {
    }

    SerializeStatus CmdPacket::serialize(SerializeBufferBase& buffer) const {

        // Shouldn't be called
        FW_ASSERT(0);
        return FW_SERIALIZE_OK; // for compiler

    }

    SerializeStatus CmdPacket::deserialize(SerializeBufferBase& buffer) {

        SerializeStatus stat = ComPacket::deserializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        // double check packet type
        if (this->m_type != FW_PACKET_COMMAND) {
            return FW_DESERIALIZE_TYPE_MISMATCH;
        }

        stat = buffer.deserialize(this->m_opcode);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        // if non-empty, copy data
        if (buffer.getBuffLeft()) {
            // copy the serialized arguments to the buffer
            stat = buffer.copyRaw(this->m_argBuffer,buffer.getBuffLeft());
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
