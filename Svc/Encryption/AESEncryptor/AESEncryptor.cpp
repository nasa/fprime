// ======================================================================
// \title  AESEncryptor.cpp
// \author cadena
// \brief  cpp file for AESEncryptor component implementation class
// ======================================================================

#include "Svc/Encryption/AESEncryptor/AESEncryptor.hpp"

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <cstring>

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AESEncryptor ::AESEncryptor(const char* const compName) : AESEncryptorComponentBase(compName) {}

AESEncryptor ::~AESEncryptor() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AESEncryptor ::encryptIn_handler(FwIndexType portNum,
                                      U16 securityAssociationIndex,
                                      Fw::Buffer& data,
                                      const ComCfg::FrameContext& context) {
    Svc::Ccsds::SdlsKeyBuffer key;
    Svc::Ccsds::SdlsStatus status = this->keyGet_out(0, key);

    if (status == Svc::Ccsds::SdlsStatus::KEY_ERROR) {
        return;
    }    
    U8 outbuf[1024];
    U8 *iv = outbuf;
    U8 *ciphertext = outbuf + 12;
    U8 tag[16];
    int len = 0, cipherLen = 0, finalLen = 0;

    RAND_bytes(iv, 12);

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    if (!EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.getBuffAddr(), iv)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }

    if (!EVP_EncryptUpdate(ctx, ciphertext, &len, data.getData(), static_cast<int>(data.getSize()))) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    cipherLen = len;

    if (!EVP_EncryptFinal_ex(ctx, ciphertext + cipherLen, &finalLen)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    cipherLen += finalLen;

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag)) {
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    EVP_CIPHER_CTX_free(ctx);

    memcpy(ciphertext + cipherLen, tag, 16);
    U32 outLen = 12 + cipherLen + 16;
    Fw::Buffer cipherBuf(outbuf, outLen);
    this->encryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, cipherBuf, context);
    
}

void AESEncryptor ::encryptReturnIn_handler(FwIndexType portNum,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    // TODO
}

}  // namespace Ccsds

}  // namespace Svc
