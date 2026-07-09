// ======================================================================
// \title  SdlsSaRouter.hpp
// \author lestarch-autobot
// \brief  hpp file for SdlsSaRouter component implementation class
// ======================================================================

#ifndef Svc_Ccsds_SdlsSaRouter_HPP
#define Svc_Ccsds_SdlsSaRouter_HPP

#include "Fw/DataStructures/ArrayMap.hpp"
#include "SdlsSaRouterConfig/FppConstantsAc.hpp"
#include "SdlsSaRouterConfig/SaMapArrayAc.hpp"
#include "Svc/Ccsds/SdlsSaRouter/SdlsSaRouterComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class SdlsSaRouter final : public SdlsSaRouterComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct SdlsSaRouter object
    SdlsSaRouter(const char* const compName  //!< The component name
    );

    //! Destroy SdlsSaRouter object
    ~SdlsSaRouter();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for decryptIn
    //!
    //! Port to receive the security association index and iv/data buffer to decrypt
    void decryptIn_handler(FwIndexType portNum,  //!< The port number
                           U16 securityAssociationIndex,
                           Fw::Buffer& data,
                           const ComCfg::FrameContext& context) override;

    //! Handler implementation for decryptReturnIn
    //!
    //! Port for receiving back ownership of buffers sent on decryptOut
    void decryptReturnIn_handler(FwIndexType portNum,  //!< The port number
                                 Fw::Buffer& data,
                                 const ComCfg::FrameContext& context) override;

    //! Handler implementation for saBufferReturnIn
    //!
    //! Ports for receiving back iv/data buffers from downstream decryptors for deallocation
    void saBufferReturnIn_handler(FwIndexType portNum,  //!< The port number
                                  Fw::Buffer& data,
                                  const ComCfg::FrameContext& context) override;

    //! Handler implementation for saDecryptIn
    //!
    //! Ports for receiving the operation status and decrypted data (possibly newly allocated) from downstream
    //! decryptors
    void saDecryptIn_handler(FwIndexType portNum,  //!< The port number
                             const Svc::Ccsds::SdlsStatus& status,
                             Fw::Buffer& data,
                             const ComCfg::FrameContext& context) override;

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! Map from SA index to downstream port index
    Fw::ArrayMap<U16, FwIndexType, SdlsCfg::SaRouterMapEntryCount> m_saMap;

    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! Sentinel port index marking buffers forwarded by the router itself on routing errors
    static constexpr FwIndexType ROUTER_ERROR_PORT = -1;

    //! Table of outstanding decrypted data buffers to their originating port index
    Fw::ArrayMap<const U8*, FwIndexType, SdlsCfg::SaRouterMaxOutstandingBuffers> m_outstanding;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
