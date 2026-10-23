// ======================================================================
// \title  AesGcmDecryptor.hpp
// \author cadena and claradavisb
// \brief  hpp file for AesGcmDecryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AesGcmDecryptor_HPP
#define Svc_Ccsds_AesGcmDecryptor_HPP

#include "Svc/Ccsds/AesGcmDecryptor/AesGcmDecryptorComponentAc.hpp"
#include "Svc/Ccsds/Utils/AesGcm/AesGcmCipher.hpp"
#include "Svc/Ccsds/Utils/SdlsAad.hpp"

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

    // Owns cipher state, so it must not be copied or moved
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

    //! AES-256-GCM cipher, built once at construction and re-keyed per frame
    Svc::Ccsds::Utils::AesGcmCipher m_cipher;

    //! AAD for the most recent VC and SA
    Svc::Ccsds::Utils::SdlsAadCache<Svc::Ccsds::Utils::SdlsTcAad> m_aad;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
