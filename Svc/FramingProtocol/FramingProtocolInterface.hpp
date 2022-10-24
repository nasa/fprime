// ======================================================================
// \title  FramingProtocolInterface.hpp
// \author mstarch
// \brief  hpp file for framing protocol interface
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef SVC_FRAMING_PROTOCOL_INTERFACE_HPP
#define SVC_FRAMING_PROTOCOL_INTERFACE_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Time/Time.hpp>

namespace Svc {

/**
 * \brief interface supplied to the framing protocol
 *
 * In order to supply necessary fprime actions to framing implementations this allows the framing
 * implementation to call the functions to delegate the actions. Typically the FramerComponentImpl
 * is the concrete implementor of this interface.
 */
class FramingProtocolInterface {
  public:
    virtual ~FramingProtocolInterface(){};
    //! \brief allocation callback to allocate memory when framing
    //! \param size: size of the allocation request
    //! \return buffer wrapping allocated memory
    virtual Fw::Buffer allocate(const U32 size) = 0;

    //! \brief send framed data out of the framer
    //! \param outgoing: framed data wrapped in an Fw::Buffer
    virtual void send(Fw::Buffer& outgoing) = 0;

};

}
#endif  // SVC_FRAMING_PROTOCOL_INTERFACE_HPP

