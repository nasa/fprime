// ======================================================================
// \title  FramingProtocol.hpp
// \author mstarch
// \brief  hpp file for FramingProtocol class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/FramingProtocol/FramingProtocolInterface.hpp"
#include "Fw/Com/ComPacket.hpp"

#ifndef FRAMING_PROTOCOL_HPP
#define FRAMING_PROTOCOL_HPP

namespace Svc {
/**
 * \brief abstract class representing a framing protocol
 *
 * This class defines the methods used to create a framed packet from Com and Fw::Buffers. The
 * framing protocol `frame` method is called with data and it in turn is expected to call the
 * `send` method of m_interface once a packet is constructed.
 *
 * There is no requirement that this be one-to-one and thus packetization, aggregation may all
 * be performed.  A call to `m_interface.allocate` can allocate memory such that framing tokens
 * may be added.
 */
class FramingProtocol {
  public:
    //! \brief constructor
    //!
    FramingProtocol();

    //! \brief setup function called to supply the interface used for allocation and sending
    //! \param interface: interface implementation, normally FramerComponentImpl
    void setup(FramingProtocolInterface& interface);

    //! \brief frame a given set of bytes
    //! \param data: pointer to a set of bytes to be framed
    //! \param size: size of data pointed to by `data`
    //! \param packet_type: type of data supplied for File downlink packets
    virtual void frame(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type) = 0;

  PROTECTED:
    FramingProtocolInterface* m_interface;
};
};
#endif  // OWLS_PROTOCOL_HPP
