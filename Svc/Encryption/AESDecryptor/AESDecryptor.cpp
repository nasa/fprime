// ======================================================================
// \title  AESDecryptor.cpp
// \author cadena
// \brief  cpp file for AESDecryptor component implementation class
// ======================================================================

#include "Svc/Encryption/AESDecryptor/AESDecryptor.hpp"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <cstring>


namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AESDecryptor ::AESDecryptor(const char* const compName) : AESDecryptorComponentBase(compName) {}

AESDecryptor ::~AESDecryptor() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AESDecryptor ::decryptIn_handler(FwIndexType portNum,
                                      U16 securityAssociationIndex,
                                      Fw::Buffer& data,
                                      const ComCfg::FrameContext& context) {
    Svc::Ccsds::SdlsKeyBuffer key;
    Svc::Ccsds::SdlsStatus status = this->keyGet_out(0, key);

    if (status == Svc::Ccsds::SdlsStatus::KEY_ERROR) {
        return;
    }

    if (data.getSize() < 12 + 16) {
        return;
    }

    U8 *iv = data.getData();
    U8 *ciphertext = data.getData() + 12;
    U32 cipherLen = static_cast<U32>(data.getSize()) - 12 - 16;
    U8 *tag = data.getData() + 12 + cipherLen;

    U8 outbuf[1024];
    int len = 0, plainLen = 0, finalLen = 0;

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    if (!EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.getBuffAddr(), iv)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    if (!EVP_DecryptUpdate(ctx, outbuf, &len, ciphertext, static_cast<int>(cipherLen))) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    plainLen = len;

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    if (!EVP_DecryptFinal_ex(ctx, outbuf + plainLen, &finalLen)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    plainLen += finalLen;

    EVP_CIPHER_CTX_free(ctx);

    Fw::Buffer plainBuf(outbuf, plainLen);
    this->decryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, plainBuf, context);
}

void AESDecryptor ::decryptReturnIn_handler(FwIndexType portNum,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    // TODO
}

}  // namespace Ccsds

}  // namespace Svc
