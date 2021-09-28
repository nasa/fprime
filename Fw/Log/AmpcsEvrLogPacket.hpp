/*
 * AmpcsEvrLogPacket.hpp
 *
 *  Created on: October 07, 2016
 *      Author: Kevin F. Ortega
 *              Aadil Rizvi
 */

#ifndef AMPCS_EVR_LOGPACKET_HPP_
#define AMPCS_EVR_LOGPACKET_HPP_

#include <Fw/Com/ComPacket.hpp>
#include <Fw/Log/LogBuffer.hpp>
#include <Fw/Time/Time.hpp>
#include <cstring>

#define AMPCS_EVR_TASK_NAME_LEN 6

namespace Fw {

    class AmpcsEvrLogPacket : public ComPacket {
        public:

            AmpcsEvrLogPacket();
            virtual ~AmpcsEvrLogPacket();

            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialize contents
            SerializeStatus deserialize(SerializeBufferBase& buffer);

            void setTaskName(U8 *taskName, U8 len);
            void setId(U32 eventID);
            void setOverSeqNum(U32 overSeqNum);
            void setCatSeqNum(U32 catSeqNum);
            void setLogBuffer(LogBuffer& buffer);

            const U8* getTaskName() const;
            U32 getId() const;
            U32 getOverSeqNum() const;
            U32 getCatSeqNum() const;
            LogBuffer& getLogBuffer();

        protected:
            U8 m_taskName[AMPCS_EVR_TASK_NAME_LEN];
            U32 m_eventID;
            U32 m_overSeqNum;
            U32 m_catSeqNum;
            LogBuffer m_logBuffer; // !< serialized argument data
    };

} /* namespace Fw */

#endif /* AMPCS_EVR_LOGPACKET_HPP_ */
