// ======================================================================
// \title  DeframerComponentImpl.hpp
// \author mstarch
// \brief  hpp file for Deframer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
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
    ~DeframerComponentImpl(void);

    //! Setup the object
    //!
    void setup(DeframingProtocol& protocol);


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
    void framedIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                          Fw::Buffer& recvBuffer,
                          Drv::RecvStatus recvStatus);

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
