// ======================================================================
// \title  AESDecryptor.hpp
// \author cadena and claradavisb
// \brief  hpp file for AESDecryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AESDecryptor_HPP
#define Svc_Ccsds_AESDecryptor_HPP

#include "AESDecryptorConfig/FppConstantsAc.hpp"
#include "Svc/Encryption/AESDecryptor/AESDecryptorComponentAc.hpp"

#include <openssl/evp.h>

namespace Svc {

namespace Ccsds {

class AESDecryptor final : public AESDecryptorComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct AESDecryptor object
    explicit AESDecryptor(const char* const compName  //!< The component name
    );

    //! Destroy AESDecryptor object
    ~AESDecryptor();

    // The component owns an OpenSSL cipher context and a fetched algorithm, it must not be copied or moved
    AESDecryptor(const AESDecryptor&) = delete;
    AESDecryptor& operator=(const AESDecryptor&) = delete;
    AESDecryptor(AESDecryptor&&) = delete;
    AESDecryptor& operator=(AESDecryptor&&) = delete;

    //! Set the virtual channel this component decrypts for, and build the cipher context.
    //!
    //! vcId MUST match the value passed to Svc::Ccsds::TcDeframer::configure(), 
    //! or every frame fails its MAC check. Calling it again changes the VC and keeps the context.
    //!
    //! This limits the component to a single virtual channel. Supporting several would
    //! require the VCID to travel on the frame context.
    void configure(U8 vcId  //!< Virtual channel ID (6 bits)
    );

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

    //! AES-256-GCM implementation, fetched once by configure() rather than per frame
    EVP_CIPHER* m_cipher;

    //! Cipher context, created once by configure() and re-keyed per frame 
    EVP_CIPHER_CTX* m_ctx;

    //! Virtual channel ID authenticated in the SDLS additional authenticated data
    U8 m_vcId;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
