// ======================================================================
// \title  AESEncryptor.cpp
// \author cadena and claradavisb
// \brief  cpp file for AESEncryptor component implementation class
// ======================================================================

#include "Svc/Encryption/AESEncryptor/AESEncryptor.hpp"
#include "Svc/Ccsds/Utils/SdlsAuthMask.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>

namespace Svc {

namespace Ccsds {

//! Virtual channel authenticated in the AAD when configure() is not called
static constexpr U8 DEFAULT_VC_ID = 1;
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

AESEncryptor ::AESEncryptor(const char* const compName)
    : AESEncryptorComponentBase(compName),
      m_outBuf(),
      m_bufferState(BufferOwnershipState::OWNED),
      m_cipher(nullptr),
      m_ctx(nullptr),
      m_vcId(DEFAULT_VC_ID) {}

AESEncryptor ::~AESEncryptor() {
    EVP_CIPHER_CTX_free(this->m_ctx);
    EVP_CIPHER_free(this->m_cipher);
}

void AESEncryptor ::configure(U8 vcId) {
    this->m_vcId = vcId;

    // Build the cipher state once so that encrypting a frame allocates nothing. The
    // algorithm and the IV length never change; only the key and the IV do, and those are
    // supplied per frame by a single EVP_EncryptInit_ex.
    if (this->m_ctx != nullptr) {
        return;  // already built; this call is only changing the virtual channel
    }
    this->m_cipher = EVP_CIPHER_fetch(nullptr, "AES-256-GCM", nullptr);
    FW_ASSERT(this->m_cipher != nullptr);
    this->m_ctx = EVP_CIPHER_CTX_new();
    FW_ASSERT(this->m_ctx != nullptr);

    int status = EVP_EncryptInit_ex(this->m_ctx, this->m_cipher, nullptr, nullptr, nullptr);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
    status = EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(GCM_IV_LEN), nullptr);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AESEncryptor ::encryptIn_handler(FwIndexType portNum,
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
    const Svc::Ccsds::SdlsStatus keyStatus = this->keyGet_out(0, key);
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
    U8 tag[GCM_TAG_LEN] = {};
    // Authenticated but not encrypted. A null output pointer makes EVP_EncryptUpdate consume
    // the input as AAD; it must precede any plaintext.
    const Svc::Ccsds::Utils::SdlsTmAuthMask aad(this->m_vcId, securityAssociationIndex);

    // Re-keying the context configure() built. Passing a null cipher here reuses the
    // algorithm and IV length already set, which is what keeps this path allocation-free.
    bool encryptSucceeded =
        (EVP_EncryptInit_ex(this->m_ctx, nullptr, nullptr, key.getBuffAddr(), iv) == 1) &&
        (EVP_EncryptUpdate(this->m_ctx, nullptr, &len, aad.bytes, static_cast<int>(sizeof(aad.bytes))) == 1) &&
        (EVP_EncryptUpdate(this->m_ctx, ciphertext, &len, data.getData(), static_cast<int>(data.getSize())) == 1);
    if (encryptSucceeded) {
        cipherLen = len;
        encryptSucceeded = (EVP_EncryptFinal_ex(this->m_ctx, ciphertext + cipherLen, &len) == 1);
    }
    if (encryptSucceeded) {
        cipherLen += len;
        encryptSucceeded =
            (EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_GET_TAG, static_cast<int>(GCM_TAG_LEN), tag) == 1);
    }

    if (!encryptSucceeded) {
        this->failFrame(data, context, Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
        return;
    }

    (void)::memcpy(ciphertext + cipherLen, tag, GCM_TAG_LEN);
    const U32 outLen = GCM_IV_LEN + static_cast<U32>(cipherLen) + GCM_TAG_LEN;
    Fw::Buffer cipherBuf(this->m_outBuf, outLen);

    // Marked in flight before it is emitted, 
    // so that a subsequent frame arriving before the return of this one is dropped
    this->m_bufferState = BufferOwnershipState::NOT_OWNED;

    // The plaintext has been copied; hand it back before passing the ciphertext downstream
    this->bufferReturnOut_out(0, data, context);
    this->encryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, cipherBuf, context);
}

void AESEncryptor ::encryptReturnIn_handler(FwIndexType portNum,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    // A failed frame reported its status with an empty buffer
    if (!data.isValid()) {
        return;
    }
    // Only m_outBuf is ever emitted on encryptOut, so anything else is a wiring error
    FW_ASSERT(data.getData() == this->m_outBuf);
    this->m_bufferState = BufferOwnershipState::OWNED;
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

void AESEncryptor ::failFrame(Fw::Buffer& data,
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
