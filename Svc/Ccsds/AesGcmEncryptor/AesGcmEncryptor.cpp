// ======================================================================
// \title  AesGcmEncryptor.cpp
// \author cadena and claradavisb
// \brief  cpp file for AesGcmEncryptor component implementation class
// ======================================================================

#include "Svc/Ccsds/AesGcmEncryptor/AesGcmEncryptor.hpp"

#include <openssl/rand.h>

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AesGcmEncryptor ::AesGcmEncryptor(const char* const compName)
    : AesGcmEncryptorComponentBase(compName),
      m_outBuf(),
      m_bufferState(BufferOwnershipState::OWNED),
      m_cipher(Utils::AesGcmCipher::Direction::ENCRYPT),
      m_aad() {}

AesGcmEncryptor ::~AesGcmEncryptor() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AesGcmEncryptor ::encryptIn_handler(FwIndexType portNum,
                                         U16 securityAssociationIndex,
                                         Fw::Buffer& data,
                                         const ComCfg::FrameContext& context) {
    // The previous frame is still downstream and m_outBuf holds ciphertext that has not been
    // sent. Overwriting it would substitute this frame's contents into the one already in
    // flight, and the MAC would cover the substitution, so the ground would accept it. Drop
    // this frame instead
    if (this->m_bufferState != BufferOwnershipState::OWNED) {
        this->log_WARNING_HI_OutputBufferBusy();
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
        return;
    }

    // m_outBuf layout: IV (12) | ciphertext (N) | MAC (16)
    const FwSizeType plainLen = static_cast<FwSizeType>(data.getSize());
    const FwSizeType requiredSize = plainLen + Utils::AesGcmCipher::IV_LEN + Utils::AesGcmCipher::TAG_LEN;
    if (requiredSize > MAX_OUTPUT_SIZE) {
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
        return;
    }

    Svc::Ccsds::SdlsKeyBuffer key;
    const Svc::Ccsds::SdlsStatus keyStatus = this->keyGet_out(0, securityAssociationIndex, key);
    if ((keyStatus != Svc::Ccsds::SdlsStatus::SUCCESS) || (key.getSize() != Utils::AesGcmCipher::KEY_LEN)) {
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::KEY_ERROR);
        return;
    }

    // The IV, ciphertext, and MAC are written straight into their places in m_outBuf
    const Fw::ByteArray iv(this->m_outBuf, Utils::AesGcmCipher::IV_LEN);
    const Fw::ByteArray ciphertext(this->m_outBuf + Utils::AesGcmCipher::IV_LEN, plainLen);
    const Fw::ByteArray tag(this->m_outBuf + Utils::AesGcmCipher::IV_LEN + plainLen, Utils::AesGcmCipher::TAG_LEN);

    // A repeated IV under one key breaks GCM, so this generates a new one per frame
    if (RAND_bytes(iv.bytes, static_cast<int>(iv.size)) != 1) {
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
        return;
    }

    const Utils::SdlsTmAad& aad = this->m_aad.get(context.get_vcId(), securityAssociationIndex);
    const bool encryptSucceeded = this->m_cipher.encrypt(key, Fw::ConstByteArray(iv.bytes, iv.size), aad.asByteArray(),
                                                         Fw::ConstByteArray(data.getData(), plainLen), ciphertext, tag);
    if (!encryptSucceeded) {
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
        return;
    }

    Fw::Buffer cipherBuf(this->m_outBuf, static_cast<Fw::Buffer::SizeType>(requiredSize));

    // Marked in flight before it is emitted,
    // so that a subsequent frame arriving before the return of this one is dropped
    this->m_bufferState = BufferOwnershipState::NOT_OWNED;

    // The plaintext has been copied; hand it back before passing the ciphertext downstream
    this->bufferReturnOut_out(0, data, context);
    this->encryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, cipherBuf, context);
}

void AesGcmEncryptor ::encryptReturnIn_handler(FwIndexType portNum,
                                               Fw::Buffer& data,
                                               const ComCfg::FrameContext& context) {
    // A failed frame reported its status with an empty buffer
    if (!data.isValid()) {
        return;
    }
    // Only m_outBuf is ever emitted on encryptOut, so anything else is a wiring error
    FW_ASSERT(data.getData() == this->m_outBuf);
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::NOT_OWNED,
              static_cast<FwAssertArgType>(this->m_bufferState));
    this->m_bufferState = BufferOwnershipState::OWNED;
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

void AesGcmEncryptor ::failFrame(Fw::Buffer& data, const ComCfg::FrameContext& context, Svc::Ccsds::SdlsStatus status) {
    // Return the plaintext to its sender
    this->bufferReturnOut_out(0, data, context);
    // Svc::Ccsds::CcsdsSdlsFramer still needs a status on encryptIn to release the com status
    // So a failed frame reports one carrying no data rather than nothing.
    Fw::Buffer empty;
    this->encryptOut_out(0, status, empty, context);
}

}  // namespace Ccsds

}  // namespace Svc
