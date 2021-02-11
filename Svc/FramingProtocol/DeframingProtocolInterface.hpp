//
// Created by Starch, Michael D (348C) on 12/30/20.
//

#ifndef DEFRAMING_PROTOCOLINTERFACE_HPP
#define DEFRAMING_PROTOCOLINTERFACE_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Time/Time.hpp>

class DeframingProtocolInterface {
  public:

    virtual Fw::Buffer allocate(const U32 size) = 0;

    virtual void route(Fw::Buffer& data) = 0;

};

#endif  // OWLS_PROTOCOLINTERFACE_HPP
