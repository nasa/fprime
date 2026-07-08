// ======================================================================
// \title  CcsdsSdlsDeframer.hpp
// \author mstarch
// \brief  hpp file for CcsdsSdlsDeframer component implementation class
// ======================================================================

#ifndef Svc_Ccsds_CcsdsSdlsDeframer_HPP
#define Svc_Ccsds_CcsdsSdlsDeframer_HPP

#include "Svc/Ccsds/CcsdsSdlsDeframer/CcsdsSdlsDeframerComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class CcsdsSdlsDeframer final : public CcsdsSdlsDeframerComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct CcsdsSdlsDeframer object
    CcsdsSdlsDeframer(const char* const compName  //!< The component name
    );

    //! Destroy CcsdsSdlsDeframer object
    ~CcsdsSdlsDeframer();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for bufferReturnIn
    //!
    //! Port for receiving back the iv/data buffer sent on decryptOut for deallocation
    void bufferReturnIn_handler(FwIndexType portNum,  //!< The port number
                                Fw::Buffer& data,
                                const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataIn
    //!
    //! Port to receive framed data, with optional context
    void dataIn_handler(FwIndexType portNum,  //!< The port number
                        Fw::Buffer& data,
                        const ComCfg::FrameContext& context) override;

    //! Handler implementation for dataReturnIn
    //!
    //! Port receiving back ownership of sent buffers
    void dataReturnIn_handler(FwIndexType portNum,  //!< The port number
                              Fw::Buffer& data,
                              const ComCfg::FrameContext& context) override;

    //! Handler implementation for decryptIn
    //!
    //! Port for receiving decrypted data (possibly newly allocated)
    void decryptIn_handler(FwIndexType portNum,  //!< The port number
                           Fw::Buffer& data,
                           const ComCfg::FrameContext& context) override;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
