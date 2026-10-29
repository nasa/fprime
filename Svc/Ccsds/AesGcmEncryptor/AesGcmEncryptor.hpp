// ======================================================================
// \title  AesGcmEncryptor.hpp
// \author cadena and claradavisb
// \brief  hpp file for AesGcmEncryptor component implementation class
// ======================================================================

#ifndef Svc_Ccsds_AesGcmEncryptor_HPP
#define Svc_Ccsds_AesGcmEncryptor_HPP

#include "Svc/Ccsds/AesGcmEncryptor/AesGcmEncryptorComponentAc.hpp"
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/TMHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Utils/AesGcm/AesGcmCipher.hpp"
#include "Svc/Ccsds/Utils/SdlsAad.hpp"

namespace Svc {

namespace Ccsds {

class AesGcmEncryptor final : public AesGcmEncryptorComponentBase {
    //! If  m_outBuf is available, or still in flight downstream
    enum class BufferOwnershipState {
        NOT_OWNED,  //!< m_outBuf has been emitted and not yet returned
        OWNED,      //!< m_outBuf is free for the next frame
    };

  public:
    //! Largest output frame the downstream chain carries: the TM data field less the 2-byte SA index
    static constexpr FwSizeType MAX_OUTPUT_SIZE =
        ComCfg::TmFrameFixedSize - TMHeader::SERIALIZED_SIZE - TMTrailer::SERIALIZED_SIZE - sizeof(U16);

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
    //! ComCfg.TmFrameFixedSize
    U8 m_outBuf[ComCfg::TmFrameFixedSize];

    //! If  m_outBuf is free for the next frame
    BufferOwnershipState m_bufferState;

    //! AES-256-GCM cipher, built once at construction and re-keyed per frame
    Svc::Ccsds::Utils::AesGcmCipher m_cipher;

    //! AAD for the most recent VC and SA
    Svc::Ccsds::Utils::SdlsAadCache<Svc::Ccsds::Utils::SdlsTmAad> m_aad;
};

}  // namespace Ccsds

}  // namespace Svc

#endif
