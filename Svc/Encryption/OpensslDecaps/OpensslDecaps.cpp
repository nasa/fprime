// ======================================================================
// \title  OpensslDecaps.cpp
// \author cadena
// \brief  cpp file for OpensslDecaps component implementation class
// ======================================================================

#include "Svc/Encryption/OpensslDecaps/OpensslDecaps.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <oqs/oqs.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/provider.h>
#include <openssl/kdf.h>

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

OpensslDecaps ::OpensslDecaps(const char* const compName) : OpensslDecapsComponentBase(compName) {}

OpensslDecaps ::~OpensslDecaps() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void OpensslDecaps ::kemMsgIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    // decapsulation of random internal shared secret and ciphertext with private key
    const char* algName = "ML-KEM-768";
    Svc::Ccsds::SdlsKeyBuffer privKeyBuf;
    Svc::Ccsds::SdlsStatus status = this->keyGet_out(0, privKeyBuf);
    if (status == Svc::Ccsds::SdlsStatus::KEY_ERROR) {
        return;
    }
    EVP_PKEY* keypair = EVP_PKEY_new_raw_private_key_ex(nullptr, algName, nullptr, privKeyBuf.getBuffAddr(), privKeyBuf.getSize());
    if (!keypair) {
        return;
    }
    EVP_PKEY_CTX *decapsCtx = EVP_PKEY_CTX_new_from_pkey(nullptr, keypair, nullptr);
    if (!decapsCtx) {
        return;
    }

    U8 *ciphertext = fwBuffer.getData();
    size_t ctLen = fwBuffer.getSize();
    size_t slen = 0;

    if (EVP_PKEY_decapsulate_init(decapsCtx, nullptr) <= 0) {
        EVP_PKEY_CTX_free(decapsCtx);
        return;
    }

    if (EVP_PKEY_decapsulate(decapsCtx, nullptr, &slen, ciphertext, ctLen) <= 0) {
        EVP_PKEY_CTX_free(decapsCtx);
        return;
    }

    unsigned char* sharedSecret = new unsigned char[slen];
    if (EVP_PKEY_decapsulate(decapsCtx, sharedSecret, &slen, ciphertext, ctLen) <= 0) {
        EVP_PKEY_CTX_free(decapsCtx);
        delete[] sharedSecret;
        return;
    }
    EVP_PKEY_CTX_free(decapsCtx);
    EVP_PKEY_free(keypair);

    // KDF: shared secret -> AES key
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr);
    if (!pctx) {
        delete[] sharedSecret;
        return;
    }

    if (EVP_PKEY_derive_init(pctx) <= 0 ||
        EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha256()) <= 0 ||
        EVP_PKEY_CTX_set1_hkdf_key(pctx, sharedSecret, static_cast<int>(slen)) <= 0 ||
        EVP_PKEY_CTX_add1_hkdf_info(pctx, reinterpret_cast<const unsigned char*>("aes-session-key"), 15) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        delete[] sharedSecret;
        return;
    }

    U8 aesKey[32];
    size_t aesKeyLen = sizeof(aesKey);
    if (EVP_PKEY_derive(pctx, aesKey, &aesKeyLen) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        delete[] sharedSecret;
        return;
    }

    EVP_PKEY_CTX_free(pctx);
    delete[] sharedSecret;

    Fw::Buffer keyBuffer(aesKey, aesKeyLen);
    this->keySet_out(0, keyBuffer);
}

}  // namespace Ccsds

}  // namespace Svc
