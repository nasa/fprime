//
// Created by Starch, Michael D (348C) on 12/30/20.
//
#include "Svc/FramingProtocol/DeframingProtocolInterface.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "Utils/Types/CircularBuffer.hpp"

#ifndef DEFRAMING_PROTOCOL_HPP
#define DEFRAMING_PROTOCOL_HPP

namespace Svc {



class DeframingProtocol {
  public:
    enum DeframingStatus {
        DEFRAMING_STATUS_SUCCESS,
        DEFRAMING_INVALID_SIZE,
        DEFRAMING_INVALID_CHECKSUM,
        DEFRAMING_MORE_NEEDED,
        DEFRAMING_MAX_STATUS
    };
    DeframingProtocol();

    void setup(DeframingProtocolInterface& interface);

    virtual DeframingStatus deframe(Types::CircularBuffer& buffer, U32& needed) = 0;

  PROTECTED:
    DeframingProtocolInterface* m_interface;
};
};
#endif  // OWLS_PROTOCOL_HPP
