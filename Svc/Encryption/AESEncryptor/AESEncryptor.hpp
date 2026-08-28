// ======================================================================
// \title  AESEncryptor.hpp
// \author cadena and claradavisb
// \brief  hpp file for AESEncryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AESEncryptor_HPP
#define Svc_Ccsds_AESEncryptor_HPP

#include "AESEncryptorConfig/FppConstantsAc.hpp"
#include "Svc/Encryption/AESEncryptor/AESEncryptorComponentAc.hpp"

#include <openssl/evp.h>

namespace Svc {

namespace Ccsds {

class AESEncryptor final : public AESEncryptorComponentBase {
    //! If  m_outBuf is available, or still in flight downstream
    enum class BufferOwnershipState {
        NOT_OWNED,  //!< m_outBuf has been emitted and not yet returned
        OWNED,      //!< m_outBuf is free for the next frame
    };

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AESEncryptor object
    explicit AESEncryptor(const char* const compName  //!< The component name
    );

    //! Destroy AESEncryptor object
    ~AESEncryptor();


    AESEncryptor(const AESEncryptor&) = delete;
    AESEncryptor& operator=(const AESEncryptor&) = delete;
    AESEncryptor(AESEncryptor&&) = delete;
    AESEncryptor& operator=(AESEncryptor&&) = delete;

    //! Set the virtual channel ID authenticated in the AAD, and build the cipher context.
    void configure(U8 vcId  //!< Virtual channel ID (3 bits, TM)
    );

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

    //! AES-256-GCM implementation, fetched once by configure() rather than per frame
    EVP_CIPHER* m_cipher;

    //! Cipher context, created once by configure() and re-keyed per frame
    EVP_CIPHER_CTX* m_ctx;

    //! Virtual channel ID authenticated in the AAD
    U8 m_vcId;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
