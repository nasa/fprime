// ======================================================================
// \title  FprimePacketizer.hpp
// \author thomas-bc
// \brief  hpp file for FprimePacketizer component implementation class
// ======================================================================

#ifndef Svc_FprimePacketizer_HPP
#define Svc_FprimePacketizer_HPP

#include "Svc/FprimePacketizer/FprimePacketizerComponentAc.hpp"
#include "Fw/Com/ComPacket.hpp"
#include <Fw/Buffer/Buffer.hpp>

namespace Svc {

class FprimePacketizer final : public FprimePacketizerComponentBase {

  enum NeedTypeSerialization {
    NO,    //!< Indicates that the packet type does not need to be serialized
    YES    //!< Indicates that the packet type needs to be serialized (prepended) before the data
  };

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct FprimePacketizer object
    FprimePacketizer(const char* const compName  //!< The component name
    );

    //! Destroy FprimePacketizer object
    ~FprimePacketizer();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for comBufferIn
    //!
    //! Port for receiving data in Fw::Com buffers
    void comBufferIn_handler(FwIndexType portNum,  //!< The port number
                             Fw::ComBuffer& data,  //!< Buffer containing packet data
                             U32 context           //!< Call context value; meaning chosen by user
                             ) override;

    //! Handler implementation for comStatusIn
    //!
    //! Port receiving the general status from the downstream component
    //! indicating it is ready or not-ready for more input
    void comStatusIn_handler(FwIndexType portNum,    //!< The port number
                             Fw::Success& condition  //!< Condition success/failure
                             ) override;

    //! Handler implementation for fileBufferIn
    //!
    //! Port for receiving file data stored in Fw::Buffer
    void fileBufferIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& fwBuffer  //!< The buffer
                              ) override;

    //! Handler implementation for rawDataIn
    //!
    //! Port to receive data to packetize in Fw::Buffer, with optional context
    void rawDataIn_handler(FwIndexType portNum,  //!< The port number
                           Fw::Buffer& data,
                           Fw::Buffer& context) override;


    void packetizeData(const U8* const data, const FwSizeType size, NeedTypeSerialization need_serialization, Fw::ComPacket::ComPacketType packet_type);

    // TODO: figure out correct MAX_SIZE
    // should be max of COM_BUFFER_MAX and FILE_BUFFER_MAX (and set a RAW_BUFFER_MAX)
    U8 m_internalBuffer[1024];  //!< Internal buffer for packetization
    Fw::Buffer m_packetBuffer;                    //!< Buffer for packetization
};
}  // namespace Svc

#endif
