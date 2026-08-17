/*
 * AmpcsEvrLogPacket.cpp
 *
 *  Created on: October 07, 2016
 *      Author: Kevin F. Ortega
 *              Aadil Rizvi
 */

#include <Fw/Log/AmpcsEvrLogPacket.hpp>
#include <Fw/Types/Assert.hpp>

namespace Fw {

AmpcsEvrLogPacket::AmpcsEvrLogPacket() : m_eventID(0), m_overSeqNum(0), m_catSeqNum(0) {
    this->m_type = ComPacketType::FW_PACKET_LOG;
}
AmpcsEvrLogPacket::~AmpcsEvrLogPacket() {}

SerializeStatus AmpcsEvrLogPacket::serializeTo(SerialBufferBase& buffer, Fw::Endianness mode) const {
    SerializeStatus stat;

    stat = buffer.serializeFrom(this->m_taskName, AMPCS_EVR_TASK_NAME_LEN, Fw::Serialization::OMIT_LENGTH, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    stat = buffer.serializeFrom(this->m_eventID, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    stat = buffer.serializeFrom(this->m_overSeqNum, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    stat = buffer.serializeFrom(this->m_catSeqNum, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    return buffer.serializeFrom(this->m_logBuffer.getBuffAddr(), m_logBuffer.getSize(), Fw::Serialization::OMIT_LENGTH,
                                mode);
}

SerializeStatus AmpcsEvrLogPacket::deserializeFrom(SerialBufferBase& buffer, Fw::Endianness mode) {
    FwSizeType len;

    SerializeStatus stat;

    len = AMPCS_EVR_TASK_NAME_LEN;
    stat = buffer.deserializeTo(this->m_taskName, len, Fw::Serialization::OMIT_LENGTH, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    stat = buffer.deserializeTo(this->m_eventID, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    stat = buffer.deserializeTo(this->m_overSeqNum, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    stat = buffer.deserializeTo(this->m_catSeqNum, mode);
    if (stat != FW_SERIALIZE_OK) {
        return stat;
    }

    FwSizeType size = buffer.getDeserializeSizeLeft();
    if (size > this->m_logBuffer.getCapacity()) {
        return FW_DESERIALIZE_SIZE_MISMATCH;
    }
    stat = buffer.deserializeTo(this->m_logBuffer.getBuffAddr(), size, Fw::Serialization::OMIT_LENGTH, mode);
    if (stat == FW_SERIALIZE_OK) {
        // Shouldn't fail
        stat = this->m_logBuffer.setBuffLen(size);
        FW_ASSERT(stat == FW_SERIALIZE_OK, static_cast<FwAssertArgType>(stat));
    }
    return stat;
}

void AmpcsEvrLogPacket::setTaskName(U8* taskName, U8 len) {
    FW_ASSERT(taskName != nullptr);
    FW_ASSERT(len == AMPCS_EVR_TASK_NAME_LEN);

    memcpy(this->m_taskName, static_cast<const void*>(taskName), len);
}

void AmpcsEvrLogPacket::setId(U32 eventID) {
    this->m_eventID = eventID;
}

void AmpcsEvrLogPacket::setOverSeqNum(U32 overSeqNum) {
    this->m_overSeqNum = overSeqNum;
}

void AmpcsEvrLogPacket::setCatSeqNum(U32 catSeqNum) {
    this->m_catSeqNum = catSeqNum;
}

void AmpcsEvrLogPacket::setLogBuffer(LogBuffer& buffer) {
    this->m_logBuffer = buffer;
}

const U8* AmpcsEvrLogPacket::getTaskName() const {
    return this->m_taskName;
}

U32 AmpcsEvrLogPacket::getId() const {
    return this->m_eventID;
}

U32 AmpcsEvrLogPacket::getOverSeqNum() const {
    return this->m_overSeqNum;
}

U32 AmpcsEvrLogPacket::getCatSeqNum() const {
    return this->m_catSeqNum;
}

LogBuffer& AmpcsEvrLogPacket::getLogBuffer() {
    return this->m_logBuffer;
}

} /* namespace Fw */
