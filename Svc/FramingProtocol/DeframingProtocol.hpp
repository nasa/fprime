// ======================================================================
// \title  DeframingProtocol.hpp
// \author mstarch
// \brief  hpp file for DeframingProtocol class
//
// \copyright
// Copyright 2009-2022, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/FramingProtocol/DeframingProtocolInterface.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "Utils/Types/CircularBuffer.hpp"

#ifndef SVC_DEFRAMING_PROTOCOL_HPP
#define SVC_DEFRAMING_PROTOCOL_HPP

namespace Svc {


/**
 * \brief Abstract base class representing a deframing protocol
 *
 * This class represents the basic interface for writing a deframing protocol. This class may be
 * subclassed to provide concrete implementations for the protocol. A DeframingProtocolInterface is
 * be supplied using the `setup` call. This instance is usually the DeframingComponentImpl.
 *
 * Implementations are expected to call `m_interface.route` to send the deframed data and may call
 * `m_interface.allocate` to allocate new memory.
 */
class DeframingProtocol {
  public:
    virtual ~DeframingProtocol(){};
    /**
     * \brief Status of the deframing call
     */
    enum DeframingStatus {
        DEFRAMING_STATUS_SUCCESS, /*!< Successful deframing */
        DEFRAMING_INVALID_SIZE, /*!< Invalid size found */
        DEFRAMING_INVALID_CHECKSUM, /*!< Invalid checksum */
        DEFRAMING_MORE_NEEDED, /*!< Successful deframing likely with more data */
        DEFRAMING_INVALID_FORMAT, /*!< Invalid format */
        DEFRAMING_MAX_STATUS /*!< The number of status enumerations */
    };
    //! Constructor
    //!
    DeframingProtocol();

    //! Setup the deframing protocol with the deframing interface
    //!
    void setup(DeframingProtocolInterface& interface /*!< Deframing interface */
    );

    //! Deframe packets from within the circular buffer
    //! \return deframing status of this deframe attempt
    virtual DeframingStatus deframe(Types::CircularBuffer& buffer,  /*!< Deframe from circular buffer */
                                    U32& needed  /*!< Return needed number of bytes */
    ) = 0;

  PROTECTED:
    DeframingProtocolInterface* m_interface;
};
}
#endif  // SVC_DEFRAMING_PROTOCOL_HPP
