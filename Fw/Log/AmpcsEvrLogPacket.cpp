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

    AmpcsEvrLogPacket::AmpcsEvrLogPacket() :
      m_eventID(0),
      m_overSeqNum(0),
      m_catSeqNum(0) {
        this->m_type = FW_PACKET_LOG;
    }

    AmpcsEvrLogPacket::~AmpcsEvrLogPacket() {
    }

    SerializeStatus AmpcsEvrLogPacket::serialize(SerializeBufferBase& buffer) const {

        SerializeStatus stat;

        stat = buffer.serialize(this->m_taskName, AMPCS_EVR_TASK_NAME_LEN, true);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.serialize(this->m_eventID);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.serialize(this->m_overSeqNum);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.serialize(this->m_catSeqNum);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        return buffer.serialize(this->m_logBuffer.getBuffAddr(),m_logBuffer.getBuffLength(),true);

    }

    SerializeStatus AmpcsEvrLogPacket::deserialize(SerializeBufferBase& buffer) {
        NATIVE_UINT_TYPE len;

        SerializeStatus stat;

        len = AMPCS_EVR_TASK_NAME_LEN;
        stat = buffer.deserialize(this->m_taskName, len, true);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.deserialize(this->m_eventID);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.deserialize(this->m_overSeqNum);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        stat = buffer.deserialize(this->m_catSeqNum);
        if (stat != FW_SERIALIZE_OK) {
            return stat;
        }

        NATIVE_UINT_TYPE size = buffer.getBuffLeft();
        stat = buffer.deserialize(this->m_logBuffer.getBuffAddr(),size,true);
        if (stat == FW_SERIALIZE_OK) {
            // Shouldn't fail
            stat = this->m_logBuffer.setBuffLen(size);
            FW_ASSERT(stat == FW_SERIALIZE_OK,static_cast<FwAssertArgType>(stat));
        }
        return stat;
    }

    void AmpcsEvrLogPacket::setTaskName(U8 *taskName, U8 len) {
        FW_ASSERT(taskName != nullptr);
        FW_ASSERT(len == AMPCS_EVR_TASK_NAME_LEN);

        memcpy(this->m_taskName, (const void*)taskName, len);
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
