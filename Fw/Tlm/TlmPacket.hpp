/*
 * TlmPacket.hpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#ifndef TLMPACKET_HPP_
#define TLMPACKET_HPP_

#include <Fw/Com/ComPacket.hpp>
#include <Fw/Tlm/TlmBuffer.hpp>
#include <Fw/Time/Time.hpp>

namespace Fw {

    class TlmPacket : public ComPacket {
        public:

            TlmPacket();
            virtual ~TlmPacket();

            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialize contents
            // Buffer containing value must be remainder of buffer
            SerializeStatus deserialize(SerializeBufferBase& buffer);
            // setters
            void setId(FwChanIdType id);
            void setTlmBuffer(TlmBuffer& buffer);
            void setTimeTag(Time& timeTag);
            // getters
            FwChanIdType getId();
            Time& getTimeTag();
            TlmBuffer& getTlmBuffer();

        PROTECTED:
            FwChanIdType m_id; // !< Channel id
            Fw::Time m_timeTag; // !< time tag
            TlmBuffer m_tlmBuffer; // !< serialized data
    };

} /* namespace Fw */

#endif /* TLMPACKET_HPP_ */
