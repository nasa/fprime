// ======================================================================
// \title  ClearTextEncryptor.hpp
// \author lestarch-autobot
// \brief  hpp file for ClearTextEncryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_ClearTextEncryptor_HPP
#define Svc_Ccsds_ClearTextEncryptor_HPP

#include "Svc/Ccsds/ClearTextEncryptor/ClearTextEncryptorComponentAc.hpp"

namespace Svc {

namespace Ccsds {

class ClearTextEncryptor final : public ClearTextEncryptorComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ClearTextEncryptor object
    ClearTextEncryptor(const char* const compName  //!< The component name
    );

    //! Destroy ClearTextEncryptor object
    ~ClearTextEncryptor();

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
