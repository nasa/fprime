// ======================================================================
// \title  AesGcmDecryptor.hpp
// \author cadena and claradavisb
// \brief  hpp file for AesGcmDecryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AesGcmDecryptor_HPP
#define Svc_Ccsds_AesGcmDecryptor_HPP

#include "Svc/Ccsds/AesGcmDecryptor/AesGcmDecryptorComponentAc.hpp"
#include "Svc/Ccsds/Types/SdlsIvArrayAc.hpp"
#include "Svc/Ccsds/Utils/SdlsAuthMask.hpp"

#include <openssl/evp.h>

namespace Svc {

namespace Ccsds {

class AesGcmDecryptor final : public AesGcmDecryptorComponentBase {
  public:
    //! Anti-replay window used until configureAntiReplay() is called; matches the ground segment's default
    static constexpr U32 DEFAULT_ANTI_REPLAY_WINDOW = 10;

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

    // ----------------------------------------------------------------------
    // Public methods
    // ----------------------------------------------------------------------

    //! Enable or disable anti-replay checking of the IV as a sequence number. When enabled, an
    //! authenticated frame is accepted only if its IV is one of the next `window` values after the
    //! last accepted IV (modulo 2^96). Anti-replay is disabled until this is called.
    void configureAntiReplay(bool enabled,  //!< Whether to check the IV against the window
                             U32 window     //!< Number of IVs past the last accepted one to accept; must be > 0
    );

    //! Set the last accepted IV, from which the anti-replay window is measured. The default is
    //! all ones, so that a peer starting its sequence from zero is accepted.
    void setLastAcceptedIv(const SdlsIv& iv  //!< IV to treat as the last one accepted
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

    //! Whether authenticated frames are also checked against the anti-replay window
    bool m_antiReplayEnabled;

    //! Number of IVs past m_lastAcceptedIv that are accepted
    U32 m_antiReplayWindow;

    //! IV of the last frame accepted, the reference point of the anti-replay window
    SdlsIv m_lastAcceptedIv;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
