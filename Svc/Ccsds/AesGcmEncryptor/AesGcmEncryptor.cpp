// ======================================================================
// \title  AesGcmEncryptor.cpp
// \author cadena and claradavisb
// \brief  cpp file for AesGcmEncryptor component implementation class
// ======================================================================

#include "Svc/Ccsds/AesGcmEncryptor/AesGcmEncryptor.hpp"
#include "Svc/Ccsds/Utils/SdlsAuthMask.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>

namespace Svc {

namespace Ccsds {

//! Length of the AES-GCM initialization vector, in bytes
static constexpr U32 GCM_IV_LEN = 12;
//! Length of the AES-GCM authentication tag (the SDLS MAC), in bytes
static constexpr U32 GCM_TAG_LEN = 16;
//! Length of an AES-256 key, in bytes
static constexpr FwSizeType AES_256_KEY_LEN = 32;

// The configuration constant a deployment sizes against must match what this component
// actually adds, or a topology could size its fill target to overrun the output store
static_assert(GCM_IV_LEN + GCM_TAG_LEN == SdlsCfg::AesFrameOverhead,
              "SdlsCfg.AesFrameOverhead disagrees with this component's IV and MAC lengths");

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

// Build the cipher state once so that encrypting a frame allocates nothing.
// Only the key and the IV change, and those are supplied per
// frame by a single EVP_EncryptInit_ex.
AesGcmEncryptor ::AesGcmEncryptor(const char* const compName)
    : AesGcmEncryptorComponentBase(compName),
      m_outBuf(),
      m_bufferState(BufferOwnershipState::OWNED),
      m_cipher(nullptr),
      m_ctx(nullptr),
      m_aad(0, 0),
      m_aadVcId(0),
      m_aadSaIndex(0) {
    this->m_cipher = EVP_CIPHER_fetch(nullptr, "AES-256-GCM", nullptr);
    FW_ASSERT(this->m_cipher != nullptr);
    this->m_ctx = EVP_CIPHER_CTX_new();
    FW_ASSERT(this->m_ctx != nullptr);

    int status = EVP_EncryptInit_ex(this->m_ctx, this->m_cipher, nullptr, nullptr, nullptr);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
    status = EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(GCM_IV_LEN), nullptr);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
}

AesGcmEncryptor ::~AesGcmEncryptor() {
    EVP_CIPHER_CTX_free(this->m_ctx);
    EVP_CIPHER_free(this->m_cipher);
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AesGcmEncryptor ::encryptIn_handler(FwIndexType portNum,
                                      U16 securityAssociationIndex,
                                      Fw::Buffer& data,
                                      const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_ctx != nullptr);

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
    const FwSizeType requiredSize = static_cast<FwSizeType>(data.getSize()) + GCM_IV_LEN + GCM_TAG_LEN;
    if (requiredSize > SdlsCfg::AesMaxOutputSize) {
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
        return;
    }

    Svc::Ccsds::SdlsKeyBuffer key;
    const Svc::Ccsds::SdlsStatus keyStatus = this->keyGet_out(0, securityAssociationIndex, key);
    if ((keyStatus != Svc::Ccsds::SdlsStatus::SUCCESS) || (key.getSize() != AES_256_KEY_LEN)) {
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::KEY_ERROR);
        return;
    }

    U8* const iv = this->m_outBuf;
    U8* const ciphertext = this->m_outBuf + GCM_IV_LEN;

    // A repeated IV under one key breaks GCM, so this generates a new one per frame
    if (RAND_bytes(iv, static_cast<int>(GCM_IV_LEN)) != 1) {
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
        return;
    }

    int len = 0;
    int cipherLen = 0;
    // Authenticated but not encrypted. A null output pointer makes EVP_EncryptUpdate consume
    // the input as AAD; it must precede any plaintext.
    // The mask depends only on the VC and the SA, so it is rebuilt when either changes
    // rather than per frame
    const U8 vcId = context.get_vcId();
    if ((vcId != this->m_aadVcId) || (securityAssociationIndex != this->m_aadSaIndex)) {
        this->m_aad = Svc::Ccsds::Utils::SdlsTmAuthMask(vcId, securityAssociationIndex);
        this->m_aadVcId = vcId;
        this->m_aadSaIndex = securityAssociationIndex;
    }

    // Re-keying the context the constructor built. Passing a null cipher here reuses the
    // algorithm and IV length already set, which is what keeps this path allocation-free.
    bool encryptSucceeded =
        (EVP_EncryptInit_ex(this->m_ctx, nullptr, nullptr, key.getBuffAddr(), iv) == 1) &&
        (EVP_EncryptUpdate(this->m_ctx, nullptr, &len, this->m_aad.bytes,
                           static_cast<int>(sizeof(this->m_aad.bytes))) == 1) &&
        (EVP_EncryptUpdate(this->m_ctx, ciphertext, &len, data.getData(), static_cast<int>(data.getSize())) == 1);
    if (encryptSucceeded) {
        cipherLen = len;
        encryptSucceeded = (EVP_EncryptFinal_ex(this->m_ctx, ciphertext + cipherLen, &len) == 1);
    }
    if (encryptSucceeded) {
        cipherLen += len;
        // The MAC is written straight into its place in m_outBuf, after the ciphertext
        encryptSucceeded = (EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_GET_TAG, static_cast<int>(GCM_TAG_LEN),
                                                ciphertext + cipherLen) == 1);
    }

    if (!encryptSucceeded) {
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
        return;
    }

    const U32 outLen = GCM_IV_LEN + static_cast<U32>(cipherLen) + GCM_TAG_LEN;
    Fw::Buffer cipherBuf(this->m_outBuf, outLen);

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

void AesGcmEncryptor ::failFrame(Fw::Buffer& data,
                              const ComCfg::FrameContext& context,
                              Svc::Ccsds::SdlsStatus status) {
    // Return the plaintext to its sender
    this->bufferReturnOut_out(0, data, context);
    // Svc::Ccsds::CcsdsSdlsFramer still needs a status on encryptIn to release the com status
    // So a failed frame reports one carrying no data rather than nothing.
    Fw::Buffer empty;
    this->encryptOut_out(0, status, empty, context);
}

}  // namespace Ccsds

}  // namespace Svc
