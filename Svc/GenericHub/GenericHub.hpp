// ======================================================================
// \title  GenericHub.hpp
// \author mstarch
// \brief  hpp file for GenericHub component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_GenericHub_HPP
#define Svc_GenericHub_HPP

#include "Svc/GenericHub/GenericHubComponentAc.hpp"

namespace Svc {

class GenericHub final : public GenericHubComponentBase {
  public:
    /**
     * HubType:
     *
     * Type of serialized data on the wire. Allows for expanding them on the opposing end.
     */
    enum HubType {
        HUB_TYPE_PORT,      //!< Port type transmission
        HUB_TYPE_BUFFER,    //!< Buffer type transmission
        HUB_TYPE_EVENT,     //!< Event transmission
        HUB_TYPE_CHANNEL,   //!< Telemetry channel type
        HUB_TYPE_CMD_DISP,  //!< Command dispatch type
        HUB_TYPE_CMD_RESP,  //!< Command response type
        HUB_TYPE_MAX
    };

    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object GenericHub
    //!
    GenericHub(const char* const compName /*!< The component name*/
    );

    //! Destroy object GenericHub
    //!
    ~GenericHub();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferIn
    //!
    void bufferIn_handler(const FwIndexType portNum, /*!< The port number*/
                          Fw::Buffer& fwBuffer) override;

    //! Handler implementation for bufferOutReturn
    //!
    //! Ports for receiving buffers sent on bufferOut and then returned
    void bufferOutReturn_handler(FwIndexType portNum,  //!< The port number
                                 Fw::Buffer& fwBuffer  //!< The buffer
                                 ) override;

    //! Handler implementation for cmdDispIn
    //!
    //! Command buffer input port for sequencers or other sources of command buffers
    void cmdDispIn_handler(FwIndexType portNum,  //!< The port number
                           Fw::ComBuffer& data,  //!< Buffer containing packet data
                           U32 context           //!< Call context value; meaning chosen by user
                           ) override;

    //! Handler implementation for cmdRespIn
    //!
    //! Input port for command response
    void cmdRespIn_handler(FwIndexType portNum,             //!< The port number
                           FwOpcodeType opCode,             //!< Command Op Code
                           U32 cmdSeq,                      //!< Command Sequence
                           const Fw::CmdResponse& response  //!< The command response argument
                           ) override;

    //! Handler implementation for fromBufferDriver
    //!
    void fromBufferDriver_handler(const FwIndexType portNum, /*!< The port number*/
                                  Fw::Buffer& fwBuffer) override;

    //! Handler implementation for toBufferDriverReturn
    //!
    //! Port for receiving buffers sent on toBufferDriver and then returned
    void toBufferDriverReturn_handler(FwIndexType portNum,  //!< The port number
                                      Fw::Buffer& fwBuffer  //!< The buffer
                                      ) override;

    //! Handler implementation for eventIn
    //!
    void eventIn_handler(const FwIndexType portNum,       /*!< The port number*/
                         FwEventIdType id,                /*!< Log ID */
                         Fw::Time& timeTag,               /*!< Time Tag */
                         const Fw::LogSeverity& severity, /*!< The severity argument */
                         Fw::LogBuffer& args              /*!< Buffer containing serialized log entry */
                         ) override;

    //! Handler implementation for tlmIn
    //!
    void tlmIn_handler(const FwIndexType portNum, /*!< The port number*/
                       FwChanIdType id,           /*!< Telemetry Channel ID */
                       Fw::Time& timeTag,         /*!< Time Tag */
                       Fw::TlmBuffer& val         /*!< Buffer containing serialized telemetry value */
                       ) override;

    // ----------------------------------------------------------------------
    // Handler implementations for user-defined serial input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for serialIn
    //!
    void serialIn_handler(FwIndexType portNum,         /*!< The port number*/
                          Fw::LinearBufferBase& Buffer /*!< The serialization buffer*/
                          ) override;

    // Helpers and members
    void send_data(const HubType type, const FwIndexType port, const U8* data, const FwSizeType size);
};

}  // end namespace Svc

#endif
