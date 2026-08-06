// ======================================================================
// \title  AESDecryptor.hpp
// \author cadena
// \brief  hpp file for AESDecryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AESDecryptor_HPP
#define Svc_Ccsds_AESDecryptor_HPP

#include "Svc/Encryption/AESDecryptor/AESDecryptorComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class AESDecryptor final : public AESDecryptorComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AESDecryptor object
    AESDecryptor(const char* const compName  //!< The component name
    );

    //! Destroy AESDecryptor object
    ~AESDecryptor();

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
};

}  // namespace Ccsds

}  // namespace Svc

#endif
