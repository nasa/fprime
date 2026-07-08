// ======================================================================
// \title  ClearTextDecryptor.hpp
// \author lestarch-autobot
// \brief  hpp file for ClearTextDecryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_ClearTextDecryptor_HPP
#define Svc_Ccsds_ClearTextDecryptor_HPP

#include "Svc/Ccsds/ClearTextDecryptor/ClearTextDecryptorComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class ClearTextDecryptor final : public ClearTextDecryptorComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ClearTextDecryptor object
    ClearTextDecryptor(const char* const compName  //!< The component name
    );

    //! Destroy ClearTextDecryptor object
    ~ClearTextDecryptor();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for decryptIn
    //!
    //! Port to receive the security association index and iv/data buffer to decrypt
    Svc::Ccsds::SdlsStatus decryptIn_handler(FwIndexType portNum,  //!< The port number
                                             U16 securityAssociationIndex,
                                             Fw::Buffer& data,
                                             const ComCfg::FrameContext& context) override;

    //! Handler implementation for decryptReturnIn
    //!
    //! Port for receiving back ownership of buffers sent on decryptOut
    void decryptReturnIn_handler(FwIndexType portNum,  //!< The port number
                                 Fw::Buffer& data,
                                 const ComCfg::FrameContext& context) override;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
