// ======================================================================
// \title  AesGcmDecryptor.cpp
// \author cadena and claradavisb
// \brief  cpp file for AesGcmDecryptor component implementation class
// ======================================================================

#include "Svc/Ccsds/AesGcmDecryptor/AesGcmDecryptor.hpp"
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include "Svc/Ccsds/Utils/SdlsAuthMask.hpp"
#include "Svc/Ccsds/Utils/SdlsIvSequence.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

//! Length of the AES-GCM initialization vector, in bytes
static constexpr U32 GCM_IV_LEN = 12;
//! Length of the AES-GCM authentication tag (the SDLS MAC), in bytes
static constexpr U32 GCM_TAG_LEN = 16;
//! Length of an AES-256 key, in bytes
static constexpr FwSizeType AES_256_KEY_LEN = 32;

static_assert(GCM_IV_LEN == Svc::Ccsds::Utils::SdlsIvSequence::SIZE, "SdlsIv must be the width of a GCM IV");

// Build the cipher state once so that decrypting a frame allocates nothing.
// Only the key and the IV change and those are supplied per
// frame by a single EVP_DecryptInit_ex.
AesGcmDecryptor ::AesGcmDecryptor(const char* const compName)
    : AesGcmDecryptorComponentBase(compName),
      m_cipher(nullptr),
      m_ctx(nullptr),
      m_aad(0, 0),
      m_aadVcId(0),
      m_aadSaIndex(0),
      m_antiReplayEnabled(false),
      m_antiReplayWindow(DEFAULT_ANTI_REPLAY_WINDOW),
      m_lastAcceptedIv(static_cast<U8>(0xFF)) {
    this->m_cipher = EVP_CIPHER_fetch(nullptr, "AES-256-GCM", nullptr);
    FW_ASSERT(this->m_cipher != nullptr);
    this->m_ctx = EVP_CIPHER_CTX_new();
    FW_ASSERT(this->m_ctx != nullptr);
    int status = EVP_DecryptInit_ex(this->m_ctx, this->m_cipher, nullptr, nullptr, nullptr);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
    status = EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(GCM_IV_LEN), nullptr);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
}

AesGcmDecryptor ::~AesGcmDecryptor() {
    EVP_CIPHER_CTX_free(this->m_ctx);
    EVP_CIPHER_free(this->m_cipher);
}

// ----------------------------------------------------------------------
// Public methods
// ----------------------------------------------------------------------

void AesGcmDecryptor ::configureAntiReplay(bool enabled, U32 window) {
    FW_ASSERT(window > 0);
    this->m_antiReplayEnabled = enabled;
    this->m_antiReplayWindow = window;
}

void AesGcmDecryptor ::setLastAcceptedIv(const SdlsIv& iv) {
    this->m_lastAcceptedIv = iv;
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AesGcmDecryptor ::decryptIn_handler(FwIndexType portNum,
                                         U16 securityAssociationIndex,
                                         Fw::Buffer& data,
                                         const ComCfg::FrameContext& context) {
    FW_ASSERT(this->m_ctx != nullptr);

    // Layout after CcsdsSdlsDeframer strips the SPI: IV (12) | ciphertext (N) | MAC (16).

    if (data.getSize() < GCM_IV_LEN + GCM_TAG_LEN) {
        // Too short to hold an IV and a MAC
        this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE, data, context);
        return;
    }

    Svc::Ccsds::SdlsKeyBuffer key;
    const Svc::Ccsds::SdlsStatus keyStatus = this->keyGet_out(0, securityAssociationIndex, key);
    if ((keyStatus != Svc::Ccsds::SdlsStatus::SUCCESS) || (key.getSize() != AES_256_KEY_LEN)) {
        // A wrong-sized key would decrypt under the wrong material rather than failing
        this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::KEY_ERROR, data, context);
        return;
    }

    U8* const iv = data.getData();
    U8* const ciphertext = data.getData() + GCM_IV_LEN;
    const U32 cipherLen = static_cast<U32>(data.getSize()) - GCM_IV_LEN - GCM_TAG_LEN;
    U8* const tag = ciphertext + cipherLen;

    // Authenticated but not encrypted; the VC travels in the context
    // The mask depends only on the VC and the SA, so it is rebuilt when either changes
    // rather than per frame
    const U8 vcId = context.get_vcId();
    if ((vcId != this->m_aadVcId) || (securityAssociationIndex != this->m_aadSaIndex)) {
        this->m_aad = Svc::Ccsds::Utils::SdlsTcAuthMask(vcId, securityAssociationIndex);
        this->m_aadVcId = vcId;
        this->m_aadSaIndex = securityAssociationIndex;
    }

    int len = 0;
    int plainLen = 0;

    const bool rekeyed = EVP_DecryptInit_ex(this->m_ctx, nullptr, nullptr, key.getBuffAddr(), iv) == 1;
    // The cipher context holds the key schedule now, so the stack copy is dead.
    // OPENSSL_cleanse wipes it so the key cannot be recovered from a memory dump.
    OPENSSL_cleanse(key.getBuffAddr(), key.getCapacity());
    const bool aadAbsorbed = rekeyed && (EVP_DecryptUpdate(this->m_ctx, nullptr, &len, this->m_aad.bytes,
                                                           static_cast<int>(sizeof(this->m_aad.bytes))) == 1);
    const bool decrypted =
        aadAbsorbed && (EVP_DecryptUpdate(this->m_ctx, ciphertext, &len, ciphertext, static_cast<int>(cipherLen)) == 1);
    const bool tagSet =
        decrypted && (EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_SET_TAG, static_cast<int>(GCM_TAG_LEN), tag) == 1);
    if (!tagSet) {
        this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE, data, context);
        return;
    }
    plainLen = len;

    // Verify the MAC
    if (EVP_DecryptFinal_ex(this->m_ctx, ciphertext + plainLen, &len) != 1) {
        this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::MAC_VERIFICATION_FAILURE, data, context);
        return;
    }
    FW_ASSERT(len == 0, static_cast<FwAssertArgType>(len));

    // Only an authenticated IV may move the window, so a forged frame cannot desynchronize it
    SdlsIv receivedIv;
    Svc::Ccsds::Utils::SdlsIvSequence::fromBytes(receivedIv, iv);
    if (this->m_antiReplayEnabled &&
        !Svc::Ccsds::Utils::SdlsIvSequence::isInWindow(this->m_lastAcceptedIv, receivedIv, this->m_antiReplayWindow)) {
        this->log_WARNING_HI_IvReplayed(securityAssociationIndex, receivedIv, this->m_lastAcceptedIv);
        this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::ANTI_REPLAY_FAILURE, data, context);
        return;
    }
    this->m_lastAcceptedIv = receivedIv;

    // Move to the plaintext
    data.advance(static_cast<FwSignedSizeType>(GCM_IV_LEN));
    data.setSize(static_cast<Fw::Buffer::SizeType>(plainLen));
    this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, data, context);
}

void AesGcmDecryptor ::decryptReturnIn_handler(FwIndexType portNum,
                                               Fw::Buffer& data,
                                               const ComCfg::FrameContext& context) {
    this->bufferReturnOut_out(0, data, context);
}

}  // namespace Ccsds

}  // namespace Svc
