// ======================================================================
// \title  AesGcmCipher.cpp
// \brief  AES-256-GCM one-shot encrypt / decrypt over a preallocated OpenSSL context
// ======================================================================

#include "Svc/Ccsds/Utils/AesGcm/AesGcmCipher.hpp"

#include <openssl/crypto.h>

#include "Fw/Types/Assert.hpp"

namespace Svc {
namespace Ccsds {
namespace Utils {

// Out-of-class definitions so the constants can be ODR-used under C++14
constexpr FwSizeType AesGcmCipher::KEY_LEN;
constexpr FwSizeType AesGcmCipher::IV_LEN;
constexpr FwSizeType AesGcmCipher::TAG_LEN;

// Build the cipher state once so that a frame operation allocates nothing. Only the key and
// the IV change, and those are supplied per operation by a single EVP_CipherInit_ex.
AesGcmCipher ::AesGcmCipher(Direction direction) : m_direction(direction), m_cipher(nullptr), m_ctx(nullptr) {
    this->m_cipher = EVP_CIPHER_fetch(nullptr, "AES-256-GCM", nullptr);
    FW_ASSERT(this->m_cipher != nullptr);
    this->m_ctx = EVP_CIPHER_CTX_new();
    FW_ASSERT(this->m_ctx != nullptr);

    const int encrypting = (direction == Direction::ENCRYPT) ? 1 : 0;
    int status = EVP_CipherInit_ex(this->m_ctx, this->m_cipher, nullptr, nullptr, nullptr, encrypting);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
    status = EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(IV_LEN), nullptr);
    FW_ASSERT(status == 1, static_cast<FwAssertArgType>(status));
}

AesGcmCipher ::~AesGcmCipher() {
    EVP_CIPHER_CTX_free(this->m_ctx);
    EVP_CIPHER_free(this->m_cipher);
}

bool AesGcmCipher ::encrypt(Svc::Ccsds::SdlsKeyBuffer& key,
                            Fw::ConstByteArray iv,
                            Fw::ConstByteArray aad,
                            Fw::ConstByteArray plaintext,
                            Fw::ByteArray ciphertext,
                            Fw::ByteArray tag) {
    FW_ASSERT(this->m_direction == Direction::ENCRYPT);
    FW_ASSERT(ciphertext.size >= plaintext.size, static_cast<FwAssertArgType>(ciphertext.size),
              static_cast<FwAssertArgType>(plaintext.size));
    FW_ASSERT(tag.size == TAG_LEN, static_cast<FwAssertArgType>(tag.size));

    int len = 0;
    int cipherLen = 0;
    bool ok = this->init(key, iv, aad);
    // A null output would switch EVP_CipherUpdate into AAD mode, so an empty payload skips it
    if (ok && (plaintext.size > 0)) {
        ok = (EVP_CipherUpdate(this->m_ctx, ciphertext.bytes, &len, plaintext.bytes,
                               static_cast<int>(plaintext.size)) == 1);
        cipherLen = len;
    }
    if (ok) {
        ok = (EVP_CipherFinal_ex(this->m_ctx, ciphertext.bytes + cipherLen, &len) == 1);
    }
    if (ok) {
        cipherLen += len;
        // GCM is a stream mode, so the ciphertext is exactly as long as the plaintext
        FW_ASSERT(static_cast<FwSizeType>(cipherLen) == plaintext.size, static_cast<FwAssertArgType>(cipherLen));
        ok = (EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_GET_TAG, static_cast<int>(TAG_LEN), tag.bytes) == 1);
    }
    return ok;
}

Svc::Ccsds::SdlsStatus AesGcmCipher ::decrypt(Svc::Ccsds::SdlsKeyBuffer& key,
                                              Fw::ConstByteArray iv,
                                              Fw::ConstByteArray aad,
                                              Fw::ByteArray data,
                                              Fw::ConstByteArray tag) {
    FW_ASSERT(this->m_direction == Direction::DECRYPT);
    FW_ASSERT(tag.size == TAG_LEN, static_cast<FwAssertArgType>(tag.size));

    int len = 0;
    int plainLen = 0;
    bool ok = this->init(key, iv, aad);
    // A null output would switch EVP_CipherUpdate into AAD mode, so an empty payload skips it
    if (ok && (data.size > 0)) {
        ok = (EVP_CipherUpdate(this->m_ctx, data.bytes, &len, data.bytes, static_cast<int>(data.size)) == 1);
        plainLen = len;
    }
    // The tag is only read; the const_cast satisfies the void* control interface
    ok = ok && (EVP_CIPHER_CTX_ctrl(this->m_ctx, EVP_CTRL_GCM_SET_TAG, static_cast<int>(TAG_LEN),
                                    const_cast<U8*>(tag.bytes)) == 1);
    if (!ok) {
        return Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE;
    }

    // Verify the MAC
    if (EVP_CipherFinal_ex(this->m_ctx, data.bytes + plainLen, &len) != 1) {
        return Svc::Ccsds::SdlsStatus::MAC_VERIFICATION_FAILURE;
    }
    FW_ASSERT(len == 0, static_cast<FwAssertArgType>(len));
    FW_ASSERT(static_cast<FwSizeType>(plainLen) == data.size, static_cast<FwAssertArgType>(plainLen));
    return Svc::Ccsds::SdlsStatus::SUCCESS;
}

bool AesGcmCipher ::init(Svc::Ccsds::SdlsKeyBuffer& key, Fw::ConstByteArray iv, Fw::ConstByteArray aad) {
    FW_ASSERT(iv.size == IV_LEN, static_cast<FwAssertArgType>(iv.size));

    // A wrong-sized key would run the cipher under the wrong material rather than failing
    bool ok = (key.getSize() == KEY_LEN);
    // Passing a null cipher re-keys the context the constructor built, reusing the algorithm,
    // direction, and IV length already set, which is what keeps this path allocation-free
    ok = ok && (EVP_CipherInit_ex(this->m_ctx, nullptr, nullptr, key.getBuffAddr(), iv.bytes, -1) == 1);
    // The context holds the key schedule now, so the caller's copy is dead; wipe it so the key
    // cannot be recovered from a memory dump
    OPENSSL_cleanse(key.getBuffAddr(), key.getCapacity());

    // A null output makes EVP_CipherUpdate absorb its input as AAD; it must precede the payload
    int len = 0;
    if (ok && (aad.size > 0)) {
        ok = (EVP_CipherUpdate(this->m_ctx, nullptr, &len, aad.bytes, static_cast<int>(aad.size)) == 1);
    }
    return ok;
}

}  // namespace Utils
}  // namespace Ccsds
}  // namespace Svc
