// ======================================================================
// \title  AesGcmDecryptor.cpp
// \author cadena and claradavisb
// \brief  cpp file for AesGcmDecryptor component implementation class
// ======================================================================

#include "Svc/Ccsds/AesGcmDecryptor/AesGcmDecryptor.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AesGcmDecryptor ::AesGcmDecryptor(const char* const compName)
    : AesGcmDecryptorComponentBase(compName), m_cipher(Utils::AesGcmCipher::Direction::DECRYPT), m_aad() {}

AesGcmDecryptor ::~AesGcmDecryptor() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AesGcmDecryptor ::decryptIn_handler(FwIndexType portNum,
                                         U16 securityAssociationIndex,
                                         Fw::Buffer& data,
                                         const ComCfg::FrameContext& context) {
    // Layout after CcsdsSdlsDeframer strips the SPI: IV (12) | ciphertext (N) | MAC (16).

    if (data.getSize() < Utils::AesGcmCipher::IV_LEN + Utils::AesGcmCipher::TAG_LEN) {
        // Too short to hold an IV and a MAC
        this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE, data, context);
        return;
    }

    Svc::Ccsds::SdlsKeyBuffer key;
    const Svc::Ccsds::SdlsStatus keyStatus = this->keyGet_out(0, securityAssociationIndex, key);
    if ((keyStatus != Svc::Ccsds::SdlsStatus::SUCCESS) || (key.getSize() != Utils::AesGcmCipher::KEY_LEN)) {
        // A wrong-sized key would decrypt under the wrong material rather than failing
        this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::KEY_ERROR, data, context);
        return;
    }

    const FwSizeType cipherLen =
        static_cast<FwSizeType>(data.getSize()) - Utils::AesGcmCipher::IV_LEN - Utils::AesGcmCipher::TAG_LEN;
    const Fw::ConstByteArray iv(data.getData(), Utils::AesGcmCipher::IV_LEN);
    const Fw::ByteArray ciphertext(data.getData() + Utils::AesGcmCipher::IV_LEN, cipherLen);
    const Fw::ConstByteArray tag(ciphertext.bytes + cipherLen, Utils::AesGcmCipher::TAG_LEN);

    // Authenticated but not encrypted; the VC travels in the context
    const Utils::SdlsTcAad& aad = this->m_aad.get(context.get_vcId(), securityAssociationIndex);
    const Svc::Ccsds::SdlsStatus status = this->m_cipher.decrypt(key, iv, aad.asByteArray(), ciphertext, tag);
    if (status != Svc::Ccsds::SdlsStatus::SUCCESS) {
        this->decryptOut_out(0, status, data, context);
        return;
    }

    // Move to the plaintext, decrypted in place over the ciphertext
    data.advance(static_cast<FwSignedSizeType>(Utils::AesGcmCipher::IV_LEN));
    data.setSize(static_cast<Fw::Buffer::SizeType>(cipherLen));
    this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, data, context);
}

void AesGcmDecryptor ::decryptReturnIn_handler(FwIndexType portNum,
                                               Fw::Buffer& data,
                                               const ComCfg::FrameContext& context) {
    this->bufferReturnOut_out(0, data, context);
}

}  // namespace Ccsds

}  // namespace Svc
