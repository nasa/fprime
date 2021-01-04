// ======================================================================
// \title  FramerComponentImpl.hpp
// \author mstarch
// \brief  hpp file for Framer component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Framer_HPP
#define Framer_HPP

#include "Svc/Framer/FramerComponentAc.hpp"
#include "Svc/FramingProtocol/FramingProtocol.hpp"
#include "Svc/FramingProtocol/FramingProtocolInterface.hpp"

namespace Svc {

class FramerComponentImpl : public FramerComponentBase, public FramingProtocolInterface {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object Framer
    //!
    FramerComponentImpl(const char* const compName /*!< The component name*/
    );

    //! Initialize object Framer
    //!
    void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! Setup the object
    //!
    void setup(FramingProtocol& protocol);

    //! Destroy object Framer
    //!
    ~FramerComponentImpl(void);

    Fw::Buffer allocate(const U32 size);

    Fw::Time time();


  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comIn
    //!
    void comIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                       Fw::ComBuffer& data,           /*!< Buffer containing packet data*/
                       U32 context                    /*!< Call context value; meaning chosen by user*/
    );

    //! Handler implementation for bufferIn
    //!
    void bufferIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                          Fw::Buffer& fwBuffer);


    void send(Fw::Buffer& outgoing);

    FramingProtocol* m_protocol;
};

}  // end namespace Svc

#endif
