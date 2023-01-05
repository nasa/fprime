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

            //! Constructor
            TlmPacket();
            //! Destructor
            virtual ~TlmPacket();

            //! Serialize the packet before sending. For use internally in software. To send to the ground, use getBuffer() below.
            SerializeStatus serialize(SerializeBufferBase& buffer) const; //!< serialize contents
            //! Deserialize the packet. For use internally in software. To extract channels, use setBuffer() and extractValue() below. This is NOT typically used.
            SerializeStatus deserialize(SerializeBufferBase& buffer);
            //! Add telemetry value to buffer. 
            SerializeStatus addValue(FwChanIdType id, Time& timeTag, TlmBuffer& buffer);
            //! extract telemetry value - since there are potentially multiple channel values in the packet, 
            //! the size of the entry must be known
            SerializeStatus extractValue(FwChanIdType &id, Time& timeTag, TlmBuffer& buffer, NATIVE_UINT_TYPE bufferSize);

            //! Reset serialization of values. This should be done when starting to accumulate a new set of values.
            SerializeStatus resetPktSer(); 
            //! Reset deserialization. This should be done before extracting values.
            SerializeStatus resetPktDeser(); 
            //! get buffer to send to the ground
            Fw::ComBuffer& getBuffer();
            //! set the internal buffer for deserializing values
            void setBuffer(Fw::ComBuffer& buffer);
            //! get the number of packets added via addValue()
            NATIVE_UINT_TYPE getNumEntries();

        PRIVATE:
            ComBuffer m_tlmBuffer; //!< serialized data
            NATIVE_UINT_TYPE m_numEntries; //!< number of entries stored during addValue()
    };

} /* namespace Fw */

#endif /* TLMPACKET_HPP_ */
