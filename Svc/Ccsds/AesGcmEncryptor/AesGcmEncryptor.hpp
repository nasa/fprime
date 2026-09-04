// ======================================================================
// \title  AesGcmEncryptor.hpp
// \author cadena and claradavisb
// \brief  hpp file for AesGcmEncryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AesGcmEncryptor_HPP
#define Svc_Ccsds_AesGcmEncryptor_HPP

#include "AesGcmEncryptorConfig/FppConstantsAc.hpp"
#include "Svc/Ccsds/Utils/SdlsAuthMask.hpp"
#include "Svc/Ccsds/AesGcmEncryptor/AesGcmEncryptorComponentAc.hpp"

#include <openssl/evp.h>

namespace Svc {

namespace Ccsds {

class AesGcmEncryptor final : public AesGcmEncryptorComponentBase {
    //! If  m_outBuf is available, or still in flight downstream
    enum class BufferOwnershipState {
        NOT_OWNED,  //!< m_outBuf has been emitted and not yet returned
        OWNED,      //!< m_outBuf is free for the next frame
    };

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AesGcmEncryptor object
    explicit AesGcmEncryptor(const char* const compName  //!< The component name
    );

    //! Destroy AesGcmEncryptor object
    ~AesGcmEncryptor();


    AesGcmEncryptor(const AesGcmEncryptor&) = delete;
    AesGcmEncryptor& operator=(const AesGcmEncryptor&) = delete;
    AesGcmEncryptor(AesGcmEncryptor&&) = delete;
    AesGcmEncryptor& operator=(AesGcmEncryptor&&) = delete;

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

    // ----------------------------------------------------------------------
    // Helpers
    // ----------------------------------------------------------------------

    //! Report a failed frame, passing the incoming buffer out with the status
    void failFrame(Fw::Buffer& data, const ComCfg::FrameContext& context, Svc::Ccsds::SdlsStatus status);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! Storage for the assembled IV | ciphertext | MAC output, sized by
    //! SdlsCfg.AesMaxOutputSize
    U8 m_outBuf[SdlsCfg::AesMaxOutputSize];

    //! If  m_outBuf is free for the next frame
    BufferOwnershipState m_bufferState;

    //! AES-256-GCM implementation, fetched once at construction rather than per frame
    EVP_CIPHER* m_cipher;

    //! Cipher context, created once at construction and re-keyed per frame
    EVP_CIPHER_CTX* m_ctx;

    //! AAD for the VC and SA in m_aadVcId and m_aadSaIndex.
    Svc::Ccsds::Utils::SdlsTmAuthMask m_aad;

    //! Virtual channel m_aad was built for
    U8 m_aadVcId;

    //! Security association index m_aad was built for
    U16 m_aadSaIndex;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
