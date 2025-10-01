/*
 * TlmPacket.hpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#ifndef TLMPACKET_HPP_
#define TLMPACKET_HPP_

#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Com/ComPacket.hpp>
#include <Fw/Time/Time.hpp>
#include <Fw/Tlm/TlmBuffer.hpp>

namespace Fw {

class TlmPacket : public ComPacket {
  public:
    //! Constructor
    TlmPacket();
    //! Destructor
    virtual ~TlmPacket();

    SerializeStatus serializeTo(SerializeBufferBase& buffer) const override;  //!< serialize contents
    SerializeStatus deserializeFrom(SerializeBufferBase& buffer) override;
    //! Add telemetry value to buffer.
    SerializeStatus addValue(FwChanIdType id, Time& timeTag, TlmBuffer& buffer);
    //! extract telemetry value - since there are potentially multiple channel values in the packet,
    //! the size of the entry must be known
    SerializeStatus extractValue(FwChanIdType& id, Time& timeTag, TlmBuffer& buffer, FwSizeType bufferSize);

    //! Reset serialization of values. This should be done when starting to accumulate a new set of values.
    SerializeStatus resetPktSer();
    //! Reset deserialization. This should be done before extracting values.
    SerializeStatus resetPktDeser();
    //! get buffer to send to the ground
    Fw::ComBuffer& getBuffer();
    //! set the internal buffer for deserializing values
    void setBuffer(Fw::ComBuffer& buffer);
    //! get the number of packets added via addValue()
    FwSizeType getNumEntries();

  private:
    ComBuffer m_tlmBuffer;    //!< serialized data
    FwSizeType m_numEntries;  //!< number of entries stored during addValue()
};

} /* namespace Fw */

#endif /* TLMPACKET_HPP_ */
