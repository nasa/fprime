/*
 * TlmPacket.cpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#include <Fw/Tlm/TlmPacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    TlmPacket::TlmPacket() : m_id(0) {
        this->m_type = FW_PACKET_TELEM;
    }

    TlmPacket::~TlmPacket() {
    }

    SerializeStatus TlmPacket::serialize(SerializeBufferBase& buffer) const {
        SerializeStatus stat;
#if !FW_AMPCS_COMPATIBLE
        stat = serializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
#endif
        stat = buffer.serialize(this->m_id);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

#if !FW_AMPCS_COMPATIBLE
        stat = buffer.serialize(this->m_timeTag);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
#endif

        return buffer.serialize(this->m_tlmBuffer.getBuffAddr(),m_tlmBuffer.getBuffLength(),true);

    }

    SerializeStatus TlmPacket::deserialize(SerializeBufferBase& buffer) {

        SerializeStatus stat;
#if !FW_AMPCS_COMPATIBLE
        stat = deserializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
#endif
        stat = buffer.deserialize(this->m_id);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

#if !FW_AMPCS_COMPATIBLE
        stat = buffer.deserialize(this->m_timeTag);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }
#endif

        // remainder of buffer must be telemetry value
        NATIVE_UINT_TYPE size = buffer.getBuffLeft();
        stat = buffer.deserialize(this->m_tlmBuffer.getBuffAddr(),size,true);
        if (stat == FW_SERIALIZE_OK) {
            // Shouldn't fail
            stat = this->m_tlmBuffer.setBuffLen(size);
            FW_ASSERT(stat == FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));
        }
        return stat;
    }

    void TlmPacket::setId(FwChanIdType id) {
        this->m_id = id;
    }

    void TlmPacket::setTlmBuffer(TlmBuffer& buffer) {
        this->m_tlmBuffer = buffer;
    }

    void TlmPacket::setTimeTag(Time& timeTag) {
        this->m_timeTag = timeTag;
    }

    FwChanIdType TlmPacket::getId() {
        return this->m_id;
    }

    Time& TlmPacket::getTimeTag() {
        return this->m_timeTag;
    }

    TlmBuffer& TlmPacket::getTlmBuffer() {
        return this->m_tlmBuffer;
    }

} /* namespace Fw */
