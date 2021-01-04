//
// Created by Starch, Michael D (348C) on 12/30/20.
//
#include "Svc/FramingProtocol/FramingProtocolInterface.hpp"
#include "Fw/Com/ComPacket.hpp"

#ifndef FRAMING_PROTOCOL_HPP
#define FRAMING_PROTOCOL_HPP

namespace Svc {
class FramingProtocol {
  public:
    FramingProtocol();

    void setup(FramingProtocolInterface& interface);

    virtual void frame(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type) = 0;

  PROTECTED:
    FramingProtocolInterface* m_interface;
};
};
#endif  // OWLS_PROTOCOL_HPP
