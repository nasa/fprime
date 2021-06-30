// ======================================================================
// \title  FramingProtocolInterface.hpp
// \author mstarch
// \brief  hpp file for framing protocol interface
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#ifndef FRAMING_PROTOCOLINTERFACE_HPP
#define FRAMING_PROTOCOLINTERFACE_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Time/Time.hpp>
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

#endif  // OWLS_PROTOCOLINTERFACE_HPP
