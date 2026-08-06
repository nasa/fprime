// ======================================================================
// \title  AESEncryptor.hpp
// \author cadena
// \brief  hpp file for AESEncryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AESEncryptor_HPP
#define Svc_Ccsds_AESEncryptor_HPP

#include "Svc/Encryption/AESEncryptor/AESEncryptorComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class AESEncryptor final : public AESEncryptorComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AESEncryptor object
    AESEncryptor(const char* const compName  //!< The component name
    );

    //! Destroy AESEncryptor object
    ~AESEncryptor();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for encryptIn
    //!
    //! Port to receive the security association index and iv/data buffer to encrypt
    void encryptIn_handler(FwIndexType portNum,  //!< The port number
                           U16 securityAssociationIndex,
                           Fw::Buffer& data,
                           const ComCfg::FrameContext& context) override;

    //! Handler implementation for encryptReturnIn
    //!
    //! Port for receiving back ownership of buffers sent on encryptOut
    void encryptReturnIn_handler(FwIndexType portNum,  //!< The port number
                                 Fw::Buffer& data,
                                 const ComCfg::FrameContext& context) override;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
