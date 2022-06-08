/*
 * TlmPacket.cpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#include <Fw/Tlm/TlmPacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    TlmPacket::TlmPacket() {
        this->m_type = FW_PACKET_TELEM;
        this->m_tlmBuffer.resetSer();
    }

    TlmPacket::~TlmPacket() {
    }

    void TlmPacket::resetPktSer() {
        this->m_tlmBuffer.resetSer();
    }

    Fw::ComBuffer& TlmPacket::getBuffer() {
        return this->m_tlmBuffer;
    }

    SerializeStatus TlmPacket::addValue(FwChanIdType id, Time& timeTag, TlmBuffer& buffer) {
        // check to make sure there is room for all the fields
        NATIVE_UINT_TYPE left = this->m_tlmBuffer.getBuffLeft();
        if (
            sizeof(FwChanIdType) + Time::SERIALIZED_SIZE + buffer.getBuffLength() > left
        ) {
            return SerializeStatus::FW_SERIALIZE_NO_ROOM_LEFT;
        }

        // serialize items out of buffer

        // id
        SerializeStatus stat = this->m_tlmBuffer.serialize(id);
        if (stat != SerializeStatus::FW_SERIALIZE_OK) {
            return stat;
        }

        // time tag
        stat = this->m_tlmBuffer.serialize(timeTag);
        if (stat != SerializeStatus::FW_SERIALIZE_OK) {
            return stat;
        }

        // telemetry buffer
        stat = this->m_tlmBuffer.serialize(buffer.getBuffAddr(),buffer.getBuffLength(),true);
        if (stat != SerializeStatus::FW_SERIALIZE_OK) {
            return stat;
        }

        return SerializeStatus::FW_SERIALIZE_OK;
    }

            // extract telemetry value
    SerializeStatus TlmPacket::extractValue(FwChanIdType &id, Time& timeTag, TlmBuffer& buffer, NATIVE_UINT_TYPE bufferSize) {

        // deserialize items out of buffer

        // id
        SerializeStatus stat = this->m_tlmBuffer.deserialize(id);
        if (stat != SerializeStatus::FW_SERIALIZE_OK) {
            return stat;
        }

        // time tag
        stat = this->m_tlmBuffer.deserialize(timeTag);
        if (stat != SerializeStatus::FW_SERIALIZE_OK) {
            return stat;
        }

        // telemetry buffer
        stat = this->m_tlmBuffer.deserialize(buffer.getBuffAddr(),bufferSize,true);
        if (stat != SerializeStatus::FW_SERIALIZE_OK) {
            return stat;
        }

        return SerializeStatus::FW_SERIALIZE_OK;

    }

    SerializeStatus TlmPacket::serialize(SerializeBufferBase& buffer) const {
        SerializeStatus stat;

        stat = serializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        return buffer.serialize(this->m_tlmBuffer.getBuffAddr(),m_tlmBuffer.getBuffLength(),true);

    }

    SerializeStatus TlmPacket::deserialize(SerializeBufferBase& buffer) {

        SerializeStatus stat;
        stat = deserializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        // deserialize the channel value entry buffers
        NATIVE_UINT_TYPE size = buffer.getBuffLeft();
        stat = buffer.deserialize(this->m_tlmBuffer.getBuffAddr(),size,true);
        if (stat == FW_SERIALIZE_OK) {
            // Shouldn't fail
            stat = this->m_tlmBuffer.setBuffLen(size);
            FW_ASSERT(stat == FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));
        }
        return stat;
    }

} /* namespace Fw */
