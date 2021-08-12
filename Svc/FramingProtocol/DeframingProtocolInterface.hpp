// ======================================================================
// \title  DeframingProtocolInterface.hpp
// \author mstarch
// \brief  hpp file for deframing protocol interface
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#ifndef DEFRAMING_PROTOCOLINTERFACE_HPP
#define DEFRAMING_PROTOCOLINTERFACE_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Time/Time.hpp>
/**
 * \brief interface supplied to the deframing protocol
 *
 * In order to supply necessary fprime actions to deframing implementations this class provides
 * the necessary functions. Typically the DeframerComponentImpl is the concrete implementor of this
 * interface.
 */
class DeframingProtocolInterface {
  public:
    virtual ~DeframingProtocolInterface(){};
    /**
     * \brief called to allocate memory, typically delegating to an allocate port call
     * \param size: size of the allocation request
     * \return Fw::Buffer wrapping allocated memory
     */
    virtual Fw::Buffer allocate(const U32 size) = 0;

    /**
     * \brief send deframed data into the system
     * \param data: deframed buffer
     */
    virtual void route(Fw::Buffer& data) = 0;

};

#endif  // DEFRAMING_PROTOCOLINTERFACE_HPP
