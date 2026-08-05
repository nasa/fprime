// ======================================================================
// \title  CcsdsSdlsFramer.hpp
// \author devin
// \brief  hpp file for CcsdsSdlsFramer component implementation class
// ======================================================================

#ifndef Svc_Ccsds_CcsdsSdlsFramer_HPP
#define Svc_Ccsds_CcsdsSdlsFramer_HPP

#include "Svc/Ccsds/CcsdsSdlsFramer/CcsdsSdlsFramerComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class CcsdsSdlsFramer final : public CcsdsSdlsFramerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct CcsdsSdlsFramer object
    CcsdsSdlsFramer(const char* const compName  //!< The component name
    );

    //! Destroy CcsdsSdlsFramer object
    ~CcsdsSdlsFramer();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferReturnIn
    //!
    //! Port for receiving back the iv/data buffer sent on encryptOut for deallocation
    void bufferReturnIn_handler(FwIndexType portNum,  //!< The port number
                                Fw::Buffer& data,
                                const ComCfg::FrameContext& context) override;

    //! Handler implementation for comStatusIn
    //!
    //! Port receiving the general status from the downstream component
    void comStatusIn_handler(FwIndexType portNum,  //!< The port number
                             Fw::Success& condition) override;

    //! Handler implementation for dataIn
    //!
    //! Port to receive data to frame, in a Fw::Buffer with optional context
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Port receiving back ownership of sent frame buffers
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,
                              const ComCfg::FrameContext& context) override;

    //! Handler implementation for encryptIn
    //!
    //! Port for receiving the operation status and encrypted data (possibly newly allocated)
    void encryptIn_handler(FwIndexType portNum,  //!< The port number
                           const Svc::Ccsds::SdlsStatus& status,
                           Fw::Buffer& data,
                           const ComCfg::FrameContext& context) override;

    // ----------------------------------------------------------------------
    // Private helpers
    // ----------------------------------------------------------------------

    //! Send a ready-for-more com status when a frame is dropped
    void sendComStatusOnDrop();
};

}  // namespace Ccsds

}  // namespace Svc

#endif
