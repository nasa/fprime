/*
 * LogPacket.cpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#include <Fw/Log/LogPacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    LogPacket::LogPacket() : m_id(0) {
        this->m_type = FW_PACKET_LOG;
    }

    LogPacket::~LogPacket() {
    }

    SerializeStatus LogPacket::serialize(SerializeBufferBase& buffer) const {

        SerializeStatus stat = ComPacket::serializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.serialize(this->m_id);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.serialize(this->m_timeTag);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        // We want to add data but not size for the ground software
        return buffer.serialize(this->m_logBuffer.getBuffAddr(),m_logBuffer.getBuffLength(),true);

    }

    SerializeStatus LogPacket::deserialize(SerializeBufferBase& buffer) {
        SerializeStatus stat = deserializeBase(buffer);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.deserialize(this->m_id);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.deserialize(this->m_timeTag);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        // remainder of buffer must be telemetry value
        NATIVE_UINT_TYPE size = buffer.getBuffLeft();
        stat = buffer.deserialize(this->m_logBuffer.getBuffAddr(),size,true);
        if (stat == FW_SERIALIZE_OK) {
            // Shouldn't fail
            stat = this->m_logBuffer.setBuffLen(size);
            FW_ASSERT(stat == FW_SERIALIZE_OK,static_cast<FwAssertArgType>(stat));
        }
        return stat;
    }

    void LogPacket::setId(FwEventIdType id) {
        this->m_id = id;
    }

    void LogPacket::setLogBuffer(const LogBuffer& buffer) {
        this->m_logBuffer = buffer;
    }

    void LogPacket::setTimeTag(const Fw::Time& timeTag) {
        this->m_timeTag = timeTag;
    }

    FwEventIdType LogPacket::getId() {
        return this->m_id;
    }

    Fw::Time& LogPacket::getTimeTag() {
        return this->m_timeTag;
    }

    LogBuffer& LogPacket::getLogBuffer() {
         return this->m_logBuffer;
    }


} /* namespace Fw */
