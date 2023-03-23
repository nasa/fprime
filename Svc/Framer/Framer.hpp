// ======================================================================
// \title  Framer.hpp
// \author mstarch, bocchino
// \brief  hpp file for Framer component implementation class
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_Framer_HPP
#define Svc_Framer_HPP

#include "Svc/Framer/FramerComponentAc.hpp"
#include "Svc/FramingProtocol/FramingProtocol.hpp"
#include "Svc/FramingProtocol/FramingProtocolInterface.hpp"

namespace Svc {
/**
 * \brief Generic framing component using FramingProtocol implementation for actual framing
 *
 * Framing component used to take Com and File packets and frame serialize them using a
 * framing protocol specified in a FramingProtocol instance.  The instance must be supplied
 * using the `setup` method.
 *
 * Using this component, projects can implement and supply a fresh FramingProtocol implementation
 * without changing the reference topology.
 */
class Framer : public FramerComponentBase, public FramingProtocolInterface {
  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object Framer
    //!
    Framer(const char* const compName /*!< The component name*/
    );

    //! Initialize object Framer
    //!
    void init(const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
    );

    //! \brief Setup this component with a supplied framing protocol
    //!
    void setup(FramingProtocol& protocol /*!< Protocol used in framing */);

    //! Destroy object Framer
    //!
    ~Framer();

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
                          Fw::Buffer& fwBuffer           /*!< The buffer*/
    );

    //! Handler implementation for comStatusIn
    //!
    void comStatusIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                             Fw::Success& condition /*!< The condition*/);

    // ----------------------------------------------------------------------
    // Implementation of FramingProtocolInterface
    // ----------------------------------------------------------------------

    //! \brief Allocation callback used to request memory for the framer
    //!
    //! Method used by the FramingProtocol to allocate memory for the framed buffer. Framing
    //! typically adds tokens on the beginning and end of the raw data so it must allocate new space
    //! to place those and a copy of the data in.
    //!
    //! \param size: size of allocation
    //! \return Fw::Buffer containing allocation to write into
    Fw::Buffer allocate(const U32 size);

    //! Send implementation
    //!
    void send(Fw::Buffer& outgoing  //!< The buffer to send
    );

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! \brief helper function to handle framing of the raw data
    //!
    void handle_framing(const U8* const data, const U32 size, Fw::ComPacket::ComPacketType packet_type);

    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The FramingProtocol implementation
    FramingProtocol* m_protocol;

    //! Flag determining if at least one frame was sent during framing
    bool m_frame_sent;
};

}  // end namespace Svc

#endif
