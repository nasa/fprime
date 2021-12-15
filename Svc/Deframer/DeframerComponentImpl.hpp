// ======================================================================
// \title  DeframerComponentImpl.hpp
// \author mstarch
// \brief  hpp file for Deframer component implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Deframer_HPP
#define Deframer_HPP

#include "Svc/Deframer/DeframerComponentAc.hpp"
#include "Svc/FramingProtocol/DeframingProtocolInterface.hpp"
#include <Svc/FramingProtocol/DeframingProtocol.hpp>
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
class DeframerComponentImpl : public DeframerComponentBase, public DeframingProtocolInterface {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object Deframer
    //!
    DeframerComponentImpl(const char* const compName /*!< The component name*/
    );

    //! Initialize object Deframer
    //!
    void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Destroy object Deframer
    //!
    ~DeframerComponentImpl();

    //! Setup the object
    //!
    void setup(DeframingProtocol& protocol /*!< Deframing protocol instance*/
    );


  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    void route(Fw::Buffer& data);

    void processRing();

    void processBuffer(Fw::Buffer& buffer);

    Fw::Buffer allocate(const U32 size);


    //! Handler implementation for framedIn
    //!
    //! Handler for input port cmdResponseIn
    void cmdResponseIn_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        FwOpcodeType opcode, //!< The command opcode
        U32 cmdSeq, //!< The command sequence number
        const Fw::CmdResponse& response //!< The command response
    );
    void framedIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                          Fw::Buffer& recvBuffer,  /*!< The raw bytes */
                          const Drv::RecvStatus& recvStatus /*!< Status of the bytes */
                          );

    //! Handler implementation for schedIn
    //!
    void schedIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                         NATIVE_UINT_TYPE context       /*!< The call order*/
    );
    DeframingProtocol* m_protocol;
    Types::CircularBuffer m_in_ring;
    U8 m_ring_buffer[1024];
    U8 m_poll_buffer[1024];
};

}  // end namespace Svc

#endif
