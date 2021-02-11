//
// Created by Starch, Michael D (348C) on 12/30/20.
//

#ifndef FRAMING_PROTOCOLINTERFACE_HPP
#define FRAMING_PROTOCOLINTERFACE_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Time/Time.hpp>

class FramingProtocolInterface {
  public:

    virtual Fw::Buffer allocate(const U32 size) = 0;

    virtual Fw::Time time() = 0;

    virtual void send(Fw::Buffer& outgoing) = 0;

};

#endif  // OWLS_PROTOCOLINTERFACE_HPP
