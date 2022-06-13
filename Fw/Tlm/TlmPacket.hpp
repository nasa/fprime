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
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Time/Time.hpp>

namespace Fw {

    class TlmPacket : public ComPacket {
        public:

            TlmPacket();
            virtual ~TlmPacket();

            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialize contents
            // Buffer containing value must be remainder of buffer
            SerializeStatus deserialize(SerializeBufferBase& buffer);
            // add telemetry value
            SerializeStatus addValue(FwChanIdType id, Time& timeTag, TlmBuffer& buffer);
            // extract telemetry value - since there are potentially multiple channel values in the packet, 
            // the size of the entry must be known
            SerializeStatus extractValue(FwChanIdType &id, Time& timeTag, TlmBuffer& buffer, NATIVE_UINT_TYPE bufferSize);

            // reset serialization
            SerializeStatus resetPktSer(); 
            // reset serialization
            SerializeStatus resetPktDeser(); 
            // get buffer to send
            Fw::ComBuffer& getBuffer();
            // set the internal buffer for deserializing
            void setBuffer(Fw::ComBuffer& buffer);

        PRIVATE:
            ComBuffer m_tlmBuffer; // !< serialized data
    };

} /* namespace Fw */

#endif /* TLMPACKET_HPP_ */
