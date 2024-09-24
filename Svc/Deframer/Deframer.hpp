// ======================================================================
// \title  Deframer.hpp
// \author mstarch, bocchino
// \brief  hpp file for Deframer component implementation class
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_Deframer_HPP
#define Svc_Deframer_HPP

#include <DeframerCfg.hpp>

#include "Svc/Deframer/DeframerComponentAc.hpp"
#include "Svc/FramingProtocol/DeframingProtocol.hpp"
#include "Svc/FramingProtocol/DeframingProtocolInterface.hpp"
#include "Utils/Types/CircularBuffer.hpp"

namespace Svc {

/**
 * \brief Generic deframing component using DeframingProtocol implementation for actual deframing
 *
 * Deframing component used to take byte streams and expand them into Com/File buffers. This is
 * done using a deframing protocol specified in a DeframingProtocol instance.  The instance must be
 * supplied using the `setup` method.
 *
 * Using this component, projects can implement and supply a fresh DeframingProtocol implementation
 * without changing the reference topology.
 *
 * Implementation uses a circular buffer to store incoming data, which is drained one framed packet
 * at a time into buffers dispatched to the rest of the system.
 */
class Deframer :
  public DeframerComponentBase,
  public DeframingProtocolInterface
{
  public:

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct Deframer instance
    Deframer(
        const char* const compName //!< The component name
    );

    //! Destroy Deframer instance
    ~Deframer();

    //! Set up the instance
    void setup(
        DeframingProtocol& protocol //!< Deframing protocol instance
    );

  PRIVATE:

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler for input port cmdResponseIn
    void cmdResponseIn_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        FwOpcodeType opcode, //!< The command opcode
        U32 cmdSeq, //!< The command sequence number
        const Fw::CmdResponse& response //!< The command response
    );

    //! Handler implementation for framedIn
    void framedIn_handler(
        const NATIVE_INT_TYPE portNum, //!< The port number
        Fw::Buffer& recvBuffer, //!< Buffer containing framed data
        const Drv::RecvStatus& recvStatus //!< Status of the bytes
    );

    //! Handler implementation for schedIn
    void schedIn_handler(
        const NATIVE_INT_TYPE portNum, //!< The port number
        U32 context //!< The call order
    );

    // ----------------------------------------------------------------------
    // Implementation of DeframingProtocolInterface
    // ----------------------------------------------------------------------

    //! The implementation of DeframingProtocolInterface::route
    //! Send a data packet
    void route(
        Fw::Buffer& packetBuffer //!< The packet buffer
    );

    //! The implementation of DeframingProtocolInterface::allocate
    //! Allocate a packet buffer
    //! \return The packet buffer
    Fw::Buffer allocate(
        const U32 size //!< The number of bytes to request
    );

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Copy data from an incoming frame buffer into the internal
    //! circular buffer
    void processBuffer(
        Fw::Buffer& buffer //!< The frame buffer
    );

    //! Process data in the circular buffer
    void processRing();

    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The DeframingProtocol implementation
    DeframingProtocol* m_protocol;

    //! The circular buffer
    Types::CircularBuffer m_inRing;

    //! Memory for the circular buffer
    U8 m_ringBuffer[DeframerCfg::RING_BUFFER_SIZE];

    //! Memory for the polling buffer
    U8 m_pollBuffer[DeframerCfg::POLL_BUFFER_SIZE];

};

}  // end namespace Svc

#endif
