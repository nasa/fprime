/*
 * TraceEntry.cpp
 *
 */

#include <Fw/Trace/TraceEntry.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

    TraceEntry::TraceEntry() : m_id(0) {
    }

    TraceEntry::~TraceEntry() {
    }

    SerializeStatus TraceEntry::serialize(SerializeBufferBase& buffer) const {

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
        return buffer.serialize(this->m_traceBuffer.getBuffAddr(),m_traceBuffer.getBuffLength(),true);

    }

    SerializeStatus TraceEntry::deserialize(SerializeBufferBase& buffer) {
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
        stat = buffer.deserialize(this->m_traceBuffer.getBuffAddr(),size,true);
        if (stat == FW_SERIALIZE_OK) {
            // Shouldn't fail
            stat = this->m_traceBuffer.setBuffLen(size);
            FW_ASSERT(stat == FW_SERIALIZE_OK,static_cast<NATIVE_INT_TYPE>(stat));
        }
        return stat;
    }

    void TraceEntry::setId(FwTraceIdType id) {
        this->m_id = id;
    }

    void TraceEntry::setTraceBuffer(const TraceBuffer& buffer) {
        this->m_traceBuffer = buffer;
    }

    void TraceEntry::setTimeTag(const Fw::Time& timeTag) {
        this->m_timeTag = timeTag;
    }

    FwTraceIdType TraceEntry::getId() {
        return this->m_id;
    }

    Fw::Time& TraceEntry::getTimeTag() {
        return this->m_timeTag;
    }

    TraceBuffer& TraceEntry::getTraceBuffer() {
         return this->m_traceBuffer;
    }


} /* namespace Fw */
