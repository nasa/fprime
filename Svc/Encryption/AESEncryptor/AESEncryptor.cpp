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
static constexpr U32 GCM_IV_LEN = 12;
static constexpr U32 GCM_TAG_LEN = 16;

void AESEncryptor ::encryptIn_handler(FwIndexType portNum,
                                      U16 securityAssociationIndex,
                                      Fw::Buffer& data,
                                      const ComCfg::FrameContext& context) {
    Svc::Ccsds::SdlsKeyBuffer key;
    Svc::Ccsds::SdlsStatus status = this->keyGet_out(0, key);

    if (status != Svc::Ccsds::SdlsStatus::SUCCESS) {
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }    
    static U8 outbuf[1024];

    if(data.getSize() + 28 > sizeof(outbuf)){
        this->encryptOut_out(portNum, Svc::Ccsds::SdlsStatus::KEY_ERROR, data, context);
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }

    U8 *iv = outbuf;
    U8 *ciphertext = outbuf + GCM_IV_LEN;
    U8 tag[GCM_TAG_LEN];
    int len = 0, cipherLen = 0, finalLen = 0;

    if(RAND_bytes(iv, GCM_IV_LEN) != 1){
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if(ctx == nullptr){
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, GCM_IV_LEN, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }

    if (!EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.getBuffAddr(), iv)) {
        EVP_CIPHER_CTX_free(ctx);
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }

    if (data.getSize() + GCM_IV_LEN + GCM_TAG_LEN > sizeof(outbuf)) {
        EVP_CIPHER_CTX_free(ctx);
        this->encryptOut_out(portNum, Svc::Ccsds::SdlsStatus::KEY_ERROR, data, context);
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }

    if (!EVP_EncryptUpdate(ctx, ciphertext, &len, data.getData(), static_cast<int>(data.getSize()))) {
        EVP_CIPHER_CTX_free(ctx);
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }
    cipherLen = len;

    if (!EVP_EncryptFinal_ex(ctx, ciphertext + cipherLen, &finalLen)) {
        EVP_CIPHER_CTX_free(ctx);
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }
    cipherLen += finalLen;

    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, GCM_TAG_LEN, tag)) {
        EVP_CIPHER_CTX_free(ctx);
        this->bufferReturnOut_out(portNum, data, context);
        return;
    }
    EVP_CIPHER_CTX_free(ctx);

    memcpy(ciphertext + cipherLen, tag, GCM_TAG_LEN);
    U32 outLen = GCM_IV_LEN + cipherLen + GCM_TAG_LEN;
    Fw::Buffer cipherBuf(outbuf, outLen);
    this->encryptOut_out(0, Svc::Ccsds::SdlsStatus::SUCCESS, cipherBuf, context);
    
}

void AESEncryptor ::encryptReturnIn_handler(FwIndexType portNum,
                                            Fw::Buffer& data,
                                            const ComCfg::FrameContext& context) {
    this->bufferReturnOut_out(portNum, data, context);
}

}  // namespace Ccsds

}  // namespace Svc
