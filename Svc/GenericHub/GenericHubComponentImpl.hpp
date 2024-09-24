// ======================================================================
// \title  GenericHubComponentImpl.hpp
// \author mstarch
// \brief  hpp file for GenericHub component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef GenericHub_HPP
#define GenericHub_HPP

#include "Svc/GenericHub/GenericHubComponentAc.hpp"

namespace Svc {

class GenericHubComponentImpl : public GenericHubComponentBase {
  public:
    /**
     * HubType:
     *
     * Type of serialized data on the wire. Allows for expanding them on the opposing end.
     */
    enum HubType {
        HUB_TYPE_PORT,    //!< Port type transmission
        HUB_TYPE_BUFFER,  //!< Buffer type transmission
        HUB_TYPE_EVENT,   //!< Event transmission
        HUB_TYPE_CHANNEL, //!< Telemetry channel type
        HUB_TYPE_MAX
    };

    const static U32 GENERIC_HUB_DATA_SIZE = 1024;
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object GenericHub
    //!
    GenericHubComponentImpl(const char* const compName /*!< The component name*/
    );

    //! Destroy object GenericHub
    //!
    ~GenericHubComponentImpl();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for buffersIn
    //!
    void buffersIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                           Fw::Buffer& fwBuffer);

    //! Handler implementation for dataIn
    //!
    void dataIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                        Fw::Buffer& fwBuffer);

    //! Handler implementation for LogRecv
    //!
    void LogRecv_handler(const NATIVE_INT_TYPE portNum,   /*!< The port number*/
                         FwEventIdType id,                /*!< Log ID */
                         Fw::Time& timeTag,               /*!< Time Tag */
                         const Fw::LogSeverity& severity, /*!< The severity argument */
                         Fw::LogBuffer& args              /*!< Buffer containing serialized log entry */
    );

    //! Handler implementation for TlmRecv
    //!
    void TlmRecv_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                         FwChanIdType id,               /*!< Telemetry Channel ID */
                         Fw::Time& timeTag,             /*!< Time Tag */
                         Fw::TlmBuffer& val             /*!< Buffer containing serialized telemetry value */
    );

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined serial input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for portIn
    //!
    void portIn_handler(NATIVE_INT_TYPE portNum,        /*!< The port number*/
                        Fw::SerializeBufferBase& Buffer /*!< The serialization buffer*/
    );

    // Helpers and members
    void send_data(const HubType type, const NATIVE_INT_TYPE port, const U8* data, const U32 size);
};

}  // end namespace Svc

#endif
