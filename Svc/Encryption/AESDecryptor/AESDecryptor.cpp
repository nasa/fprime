// ======================================================================
// \title  AESDecryptor.cpp
// \author cadena and claradavisb
// \brief  cpp file for AESDecryptor component implementation class
// ======================================================================

#include "Svc/Encryption/AESDecryptor/AESDecryptor.hpp"
#include <openssl/evp.h>
#include "Svc/Ccsds/Utils/SdlsAuthMask.hpp"
#include "SdlsKeyConfig/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

//! Default virtual channel, matching ComCfg::FrameContext
static constexpr U8 DEFAULT_VC_ID = 1;
//! Length of the AES-GCM initialization vector, in bytes
static constexpr U32 GCM_IV_LEN = 12;
//! Length of the AES-GCM authentication tag (the SDLS MAC), in bytes
static constexpr U32 GCM_TAG_LEN = 16;
//! Length of an AES-256 key, in bytes
static constexpr FwSizeType AES_256_KEY_LEN = 32;

AESDecryptor ::AESDecryptor(const char* const compName)
    : AESDecryptorComponentBase(compName), m_cipher(nullptr), m_ctx(nullptr), m_vcId(DEFAULT_VC_ID) {}

AESDecryptor ::~AESDecryptor() {
    EVP_CIPHER_CTX_free(this->m_ctx);
    EVP_CIPHER_free(this->m_cipher);
}

void AESDecryptor ::configure(U8 vcId) {
    this->m_vcId = vcId;

    // Build the cipher state once so that decrypting a frame allocates nothing. The
    // algorithm and the IV length never change; only the key and the IV do, and those are
    // supplied per frame by a single EVP_DecryptInit_ex.
    if (this->m_ctx != nullptr) {
        return;
    }
    this->m_cipher = EVP_CIPHER_fetch(nullptr, "AES-256-GCM", nullptr);
    FW_ASSERT(this->m_cipher != nullptr);
    this->m_ctx = EVP_CIPHER_CTX_new();
    FW_ASSERT(this->m_ctx != nullptr);
    int status = EVP_DecryptInit_ex(this->m_ctx, this->m_cipher, nullptr, nullptr, nullptr);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
    status = EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(GCM_IV_LEN), nullptr);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AESDecryptor ::decryptIn_handler(FwIndexType portNum,
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
    if (data.getSize() > SdlsCfg::AesMaxInputSize) {
        // Larger than the deployment's TC frame
        this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE, data, context);
        return;
    }

    Svc::Ccsds::SdlsKeyBuffer key;
    const Svc::Ccsds::SdlsStatus keyStatus = this->keyGet_out(0, key);
    if ((keyStatus != Svc::Ccsds::SdlsStatus::SUCCESS) || (key.getSize() != AES_256_KEY_LEN)) {
        // A wrong-sized key would decrypt under the wrong material rather than failing
        this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::KEY_ERROR, data, context);
        return;
    }

    U8* const iv = data.getData();
    U8* const ciphertext = data.getData() + GCM_IV_LEN;
    const U32 cipherLen = static_cast<U32>(data.getSize()) - GCM_IV_LEN - GCM_TAG_LEN;
    U8* const tag = ciphertext + cipherLen;

    // Authenticated but not encrypted; the VC comes from configure() because the TC primary
    // header that carried it was stripped upstream
    const Svc::Ccsds::Utils::SdlsTcAuthMask aad(this->m_vcId, securityAssociationIndex);

    int len = 0;
    int plainLen = 0;

    const bool rekeyed = EVP_DecryptInit_ex(this->m_ctx, nullptr, nullptr, key.getBuffAddr(), iv) == 1;
    const bool aadAbsorbed =
        rekeyed && (EVP_DecryptUpdate(this->m_ctx, nullptr, &len, aad.bytes, static_cast<int>(sizeof(aad.bytes))) ==
                    1);
    const bool decrypted =
        aadAbsorbed &&
        (EVP_DecryptUpdate(this->m_ctx, ciphertext, &len, ciphertext, static_cast<int>(cipherLen)) == 1);
    const bool tagSet =
        decrypted &&
        (EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_SET_TAG, static_cast<int>(GCM_TAG_LEN), tag) == 1);
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
    plainLen += len;

    // Move to the plaintext
    data.advance(static_cast<FwSignedSizeType>(GCM_IV_LEN));
    data.setSize(static_cast<Fw::Buffer::SizeType>(plainLen));
    this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, data, context);
}

void AESDecryptor ::decryptReturnIn_handler(FwIndexType portNum,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    this->bufferReturnOut_out(0, data, context);
}

}  // namespace Ccsds

}  // namespace Svc
