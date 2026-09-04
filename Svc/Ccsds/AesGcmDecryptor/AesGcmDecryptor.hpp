// ======================================================================
// \title  AesGcmDecryptor.hpp
// \author cadena and claradavisb
// \brief  hpp file for AesGcmDecryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AesGcmDecryptor_HPP
#define Svc_Ccsds_AesGcmDecryptor_HPP

#include "AesGcmDecryptorConfig/FppConstantsAc.hpp"
#include "Svc/Ccsds/Utils/SdlsAuthMask.hpp"
#include "Svc/Ccsds/AesGcmDecryptor/AesGcmDecryptorComponentAc.hpp"

#include <openssl/evp.h>

namespace Svc {

namespace Ccsds {

class AesGcmDecryptor final : public AesGcmDecryptorComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AesGcmDecryptor object
    explicit AesGcmDecryptor(const char* const compName  //!< The component name
    );

    //! Destroy AesGcmDecryptor object
    ~AesGcmDecryptor();

    // The component owns an OpenSSL cipher context and a fetched algorithm, it must not be copied or moved
    AesGcmDecryptor(const AesGcmDecryptor&) = delete;
    AesGcmDecryptor& operator=(const AesGcmDecryptor&) = delete;
    AesGcmDecryptor(AesGcmDecryptor&&) = delete;
    AesGcmDecryptor& operator=(AesGcmDecryptor&&) = delete;

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

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! AES-256-GCM implementation, fetched once at construction rather than per frame
    EVP_CIPHER* m_cipher;

    //! Cipher context, created once at construction and re-keyed per frame
    EVP_CIPHER_CTX* m_ctx;

    //! AAD for the VC and SA in m_aadVcId and m_aadSaIndex.
    Svc::Ccsds::Utils::SdlsTcAuthMask m_aad;

    //! Virtual channel m_aad was built for
    U8 m_aadVcId;

    //! Security association index m_aad was built for
    U16 m_aadSaIndex;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
