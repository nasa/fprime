// ======================================================================
// \title  AESEncryptorTester.cpp
// \author vivi and claradavisb
// \brief  cpp file for AESEncryptor component test harness implementation class
// ======================================================================

#include "AESEncryptorTester.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/Ccsds/Utils/SdlsAuthMask.hpp"

#include <openssl/evp.h>
#include <cstring>

namespace Svc {

namespace Ccsds {

const FwSizeType AESEncryptorTester::MAX_HISTORY_SIZE;
const FwEnumStoreType AESEncryptorTester::TEST_INSTANCE_ID;
const FwSizeType AESEncryptorTester::AES_256_KEY_LEN;
const FwSizeType AESEncryptorTester::GCM_IV_LEN;
const FwSizeType AESEncryptorTester::GCM_TAG_LEN;
const U8 AESEncryptorTester::TEST_VC_ID;
const U16 AESEncryptorTester::TEST_SPI;
const FwSizeType AESEncryptorTester::TEST_BUFFER_SIZE;

namespace {

const U8 TEST_KEY[AESEncryptorTester::AES_256_KEY_LEN] = {
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B,
    0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F};

// ----------------------------------------------------------------------
// Independent reimplementations of what the component does
// ----------------------------------------------------------------------

//! Length of the AAD an SDLS-protected TM transfer frame authenticates: the 6-byte primary
//! header, the 2-byte SPI, and the 12-byte IV field
constexpr FwSizeType TM_AAD_LEN = 6 + 2 + AESEncryptorTester::GCM_IV_LEN;

//! Build the TM additional authenticated data.
//! The primary header masked to 0x0E at byte 1 (the 3-bit VCID in bits
//! 3..1), the SPI, and a zeroed IV field.
void buildTmAad(U8 (&aad)[TM_AAD_LEN], U8 vcId, U16 spi) {
    (void)::memset(aad, 0, TM_AAD_LEN);
    aad[1] = static_cast<U8>((vcId << 1) & 0x0E);
    aad[6] = static_cast<U8>(spi >> 8);
    aad[7] = static_cast<U8>(spi & 0xFF);
}

//! AES-256-GCM encrypt, used to reproduce what the component should have emitted
void gcmEncrypt(const U8* key,
                const U8* iv,
                const U8* aad,
                FwSizeType aadLen,
                const U8* plaintext,
                FwSizeType plainLen,
                U8* ciphertext,
                U8* tag) {
    EVP_CIPHER_CTX* const ctx = EVP_CIPHER_CTX_new();
    ASSERT_NE(ctx, nullptr);
    int len = 0;
    ASSERT_EQ(EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr), 1);
    ASSERT_EQ(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(AESEncryptorTester::GCM_IV_LEN),
                                  nullptr),
              1);
    ASSERT_EQ(EVP_EncryptInit_ex(ctx, nullptr, nullptr, key, iv), 1);
    ASSERT_EQ(EVP_EncryptUpdate(ctx, nullptr, &len, aad, static_cast<int>(aadLen)), 1);
    if (plainLen > 0) {
        ASSERT_EQ(EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, static_cast<int>(plainLen)), 1);
    }
    int finalLen = 0;
    ASSERT_EQ(EVP_EncryptFinal_ex(ctx, ciphertext + plainLen, &finalLen), 1);
    ASSERT_EQ(finalLen, 0) << "GCM is a stream cipher mode; it emits no trailing block";
    ASSERT_EQ(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, static_cast<int>(AESEncryptorTester::GCM_TAG_LEN), tag),
              1);
    EVP_CIPHER_CTX_free(ctx);
}

//! AES-256-GCM decrypt, returning false on a MAC verification failure
bool gcmDecrypt(const U8* key,
                const U8* iv,
                const U8* aad,
                FwSizeType aadLen,
                const U8* ciphertext,
                FwSizeType cipherLen,
                const U8* tag,
                U8* plaintext) {
    EVP_CIPHER_CTX* const ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        return false;
    }
    int len = 0;
    bool decryptedAndVerified =
        (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) == 1) &&
        (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(AESEncryptorTester::GCM_IV_LEN),
                             nullptr) == 1) &&
        (EVP_DecryptInit_ex(ctx, nullptr, nullptr, key, iv) == 1) &&
        (EVP_DecryptUpdate(ctx, nullptr, &len, aad, static_cast<int>(aadLen)) == 1);
    if (decryptedAndVerified && (cipherLen > 0)) {
        decryptedAndVerified = (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, static_cast<int>(cipherLen)) == 1);
    }
    if (decryptedAndVerified) {
        // EVP_CTRL_GCM_SET_TAG takes a non-const pointer it only reads
        decryptedAndVerified =
            (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, static_cast<int>(AESEncryptorTester::GCM_TAG_LEN),
                                 const_cast<U8*>(tag)) == 1) &&
            (EVP_DecryptFinal_ex(ctx, plaintext + cipherLen, &len) == 1);
    }
    EVP_CIPHER_CTX_free(ctx);
    return decryptedAndVerified;
}

}  // namespace

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

AESEncryptorTester ::AESEncryptorTester()
    : AESEncryptorGTestBase("AESEncryptorTester", AESEncryptorTester::MAX_HISTORY_SIZE),
      component("AESEncryptor"),
      m_key(),
      m_keyLen(AES_256_KEY_LEN),
      m_keyStatus(Svc::Ccsds::SdlsStatus::SUCCESS),
      m_storage(),
      m_returnSynchronously(false) {
    this->initComponents();
    this->connectPorts();
    this->setKey(TEST_KEY, AES_256_KEY_LEN, Svc::Ccsds::SdlsStatus::SUCCESS);
}

AESEncryptorTester ::~AESEncryptorTester() {}

// ----------------------------------------------------------------------
// Handler overrides
// ----------------------------------------------------------------------

Svc::Ccsds::SdlsStatus AESEncryptorTester ::from_keyGet_handler(FwIndexType portNum,
                                                                U16 securityAssociationIndex,
                                                                Svc::Ccsds::SdlsKeyBuffer& key) {
    this->pushFromPortEntry_keyGet(securityAssociationIndex, key);
    if (this->m_keyStatus != Svc::Ccsds::SdlsStatus::SUCCESS) {
        const Fw::SerializeStatus status = key.setBuffLen(0);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
        return this->m_keyStatus;
    }
    (void)::memcpy(key.getBuffAddr(), this->m_key, static_cast<size_t>(this->m_keyLen));
    const Fw::SerializeStatus status = key.setBuffLen(this->m_keyLen);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    return Svc::Ccsds::SdlsStatus::SUCCESS;
}

void AESEncryptorTester ::from_encryptOut_handler(FwIndexType portNum,
                                                  const Svc::Ccsds::SdlsStatus& status,
                                                  Fw::Buffer& data,
                                                  const ComCfg::FrameContext& context) {
    this->pushFromPortEntry_encryptOut(status, data, context);
    if (this->m_returnSynchronously && data.isValid()) {
        // Svc::Ccsds::CcsdsSdlsFramer copies the ciphertext and returns the buffer before
        // encryptOut_out returns. Modelling that here is what exercises the ownership
        // flag's ordering: set after the emit, the return's release would be undone.
        Fw::Buffer returned = data;
        this->invoke_to_encryptReturnIn(0, returned, context);
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void AESEncryptorTester ::testEncryptNominal() {
    const FwSizeType plainLen = 64;
    const Fw::Buffer sent = this->sendEncrypt(plainLen, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    const Fw::Buffer out = this->fromPortHistory_encryptOut->at(0).data;
    ASSERT_EQ(out.getSize(), GCM_IV_LEN + plainLen + GCM_TAG_LEN);
    // The frame is a copy, not the caller's storage
    ASSERT_NE(out.getData(), sent.getData());
    this->assertFrameDecryptsTo(sent.getData(), plainLen, TEST_VC_ID, TEST_SPI);
}

void AESEncryptorTester ::testCiphertextAndMacMatch() {
    const FwSizeType plainLen = 48;
    const Fw::Buffer sent = this->sendEncrypt(plainLen, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);

    const U8* const frame = this->fromPortHistory_encryptOut->at(0).data.getData();
    U8 aad[TM_AAD_LEN];
    buildTmAad(aad, TEST_VC_ID, TEST_SPI);
    U8 expectedCiphertext[TEST_BUFFER_SIZE];
    U8 expectedMac[GCM_TAG_LEN];
    // Same key, the component's own IV, and the AAD the contract calls for
    gcmEncrypt(TEST_KEY, frame, aad, TM_AAD_LEN, sent.getData(), plainLen, expectedCiphertext, expectedMac);

    ASSERT_EQ(::memcmp(frame + GCM_IV_LEN, expectedCiphertext, static_cast<size_t>(plainLen)), 0)
        << "Ciphertext differs from the reference implementation";
    ASSERT_EQ(::memcmp(frame + GCM_IV_LEN + plainLen, expectedMac, static_cast<size_t>(GCM_TAG_LEN)), 0)
        << "MAC differs from the reference implementation; the AAD is the usual cause";
}

void AESEncryptorTester ::testAuthMaskLayout() {
    // Spot-check the ends of the VCID field as well as an ordinary value: a shift or mask
    // error shows up at the boundaries first
    const U8 vcIds[] = {0, 1, TEST_VC_ID, 0x07};
    for (FwSizeType i = 0; i < FW_NUM_ARRAY_ELEMENTS(vcIds); i++) {
        const U16 spi = static_cast<U16>(STest::Pick::lowerUpper(0, 0xFFFF));
        U8 expected[TM_AAD_LEN];
        buildTmAad(expected, vcIds[i], spi);

        const Svc::Ccsds::Utils::SdlsTmAuthMask actual(vcIds[i], spi);

        ASSERT_EQ(static_cast<FwSizeType>(sizeof actual.bytes), TM_AAD_LEN);
        ASSERT_EQ(::memcmp(actual.bytes, expected, TM_AAD_LEN), 0)
            << "TM auth mask does not match the ground segment's layout for VC " << static_cast<U32>(vcIds[i]);
    }
}

void AESEncryptorTester ::testContextVcIdIsAuthenticated() {
    const U8 otherVcId = TEST_VC_ID + 1;
    const FwSizeType plainLen = 32;
    const Fw::Buffer sent = this->sendEncrypt(plainLen, TEST_SPI, otherVcId);
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);

    // Authenticated under the new channel
    this->assertFrameDecryptsTo(sent.getData(), plainLen, otherVcId, TEST_SPI);

    // Not authenticated under the old one
    const U8* const frame = this->fromPortHistory_encryptOut->at(0).data.getData();
    U8 staleAad[TM_AAD_LEN];
    buildTmAad(staleAad, TEST_VC_ID, TEST_SPI);
    U8 recovered[TEST_BUFFER_SIZE];
    ASSERT_FALSE(gcmDecrypt(TEST_KEY, frame, staleAad, TM_AAD_LEN, frame + GCM_IV_LEN, plainLen,
                            frame + GCM_IV_LEN + plainLen, recovered))
        << "The frame authenticated under a virtual channel other than the one on its context";
}

void AESEncryptorTester ::testIvIsFreshPerFrame() {
    const FwSizeType plainLen = 32;
    (void)this->sendEncrypt(plainLen, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    U8 firstFrame[GCM_IV_LEN + 32 + GCM_TAG_LEN];
    const Fw::Buffer first = this->fromPortHistory_encryptOut->at(0).data;
    (void)::memcpy(firstFrame, first.getData(), static_cast<size_t>(first.getSize()));

    // Hand the buffer back so the component may reuse its storage, then send the same
    // plaintext again
    ComCfg::FrameContext context;
    Fw::Buffer returned = first;
    this->invoke_to_encryptReturnIn(0, returned, context);
    (void)this->sendEncrypt(plainLen, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    const Fw::Buffer second = this->fromPortHistory_encryptOut->at(0).data;

    ASSERT_NE(::memcmp(firstFrame, second.getData(), static_cast<size_t>(GCM_IV_LEN)), 0)
        << "The IV repeated across frames";
    ASSERT_NE(::memcmp(firstFrame + GCM_IV_LEN, second.getData() + GCM_IV_LEN, static_cast<size_t>(plainLen)), 0)
        << "Identical plaintext produced identical ciphertext";
}

void AESEncryptorTester ::testEmptyPlaintext() {
    const Fw::Buffer sent = this->sendEncrypt(0, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    ASSERT_EQ(this->fromPortHistory_encryptOut->at(0).data.getSize(), GCM_IV_LEN + GCM_TAG_LEN);
    this->assertFrameDecryptsTo(sent.getData(), 0, TEST_VC_ID, TEST_SPI);
}

void AESEncryptorTester ::testOutputCapacityBoundary() {
    const FwSizeType maxOutput = SdlsCfg::AesMaxOutputSize;
    const FwSizeType largest = maxOutput - GCM_IV_LEN - GCM_TAG_LEN;
    ASSERT_LE(largest, TEST_BUFFER_SIZE) << "Test storage is too small to reach the boundary";

    (void)this->sendEncrypt(largest, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    ASSERT_EQ(this->fromPortHistory_encryptOut->at(0).data.getSize(), maxOutput);

    // Release the store, so the next send is refused on its size and not a busy store
    ComCfg::FrameContext returnContext;
    Fw::Buffer accepted = this->fromPortHistory_encryptOut->at(0).data;
    this->invoke_to_encryptReturnIn(0, accepted, returnContext);

    // One byte more has nowhere to go
    const Fw::Buffer sent = this->sendEncrypt(largest + 1, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
    // The rejected plaintext goes home on bufferReturnOut; the status carries no data
    ASSERT_FALSE(this->fromPortHistory_encryptOut->at(0).data.isValid());
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferReturnOut->at(0).data.getData(), sent.getData());
}

void AESEncryptorTester ::testKeyUnavailable() {
    this->setKey(nullptr, 0, Svc::Ccsds::SdlsStatus::KEY_ERROR);

    (void)this->sendEncrypt(32, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::KEY_ERROR);
}

void AESEncryptorTester ::testWrongKeySize() {
    // An AES-128 key is long enough to use, and silently wrong if it were used
    this->setKey(TEST_KEY, 16, Svc::Ccsds::SdlsStatus::SUCCESS);

    (void)this->sendEncrypt(32, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::KEY_ERROR);
}

void AESEncryptorTester ::testPlaintextReturnedUnmodified() {
    const FwSizeType plainLen = 40;
    U8 original[plainLen];
    const Fw::Buffer sent = this->sendEncrypt(plainLen, TEST_SPI);
    (void)::memcpy(original, sent.getData(), static_cast<size_t>(plainLen));

    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferReturnOut->at(0).data.getData(), sent.getData());
    ASSERT_EQ(::memcmp(sent.getData(), original, static_cast<size_t>(plainLen)), 0)
        << "The plaintext buffer was modified in place";
}

void AESEncryptorTester ::testOutputBufferReclaimed() {
    (void)this->sendEncrypt(32, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    Fw::Buffer emitted = this->fromPortHistory_encryptOut->at(0).data;

    // Returning it releases the store without sending anything upstream
    this->clearHistory();
    ComCfg::FrameContext context;
    this->invoke_to_encryptReturnIn(0, emitted, context);
    ASSERT_from_bufferReturnOut_SIZE(0);

    // The next frame is accepted, so the store was released
    (void)this->sendEncrypt(32, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);

    // An empty buffer from a failed frame is a no-op
    this->clearHistory();
    Fw::Buffer empty;
    this->invoke_to_encryptReturnIn(0, empty, context);
    ASSERT_from_bufferReturnOut_SIZE(0);
}

void AESEncryptorTester ::testSynchronousReturn() {
    this->m_returnSynchronously = true;
    for (FwSizeType frame = 0; frame < 3; frame++) {
        (void)this->sendEncrypt(32, TEST_SPI);
        this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    }
    ASSERT_EVENTS_OutputBufferBusy_SIZE(0);
}

void AESEncryptorTester ::testOutputBufferBusy() {
    // Emit a frame and do not return it
    const FwSizeType plainLen = 48;
    (void)this->sendEncrypt(plainLen, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    const Fw::Buffer inFlight = this->fromPortHistory_encryptOut->at(0).data;
    U8 firstFrame[GCM_IV_LEN + 48 + GCM_TAG_LEN];
    (void)::memcpy(firstFrame, inFlight.getData(), static_cast<size_t>(inFlight.getSize()));

    // The next frame must be refused rather than written over
    const Fw::Buffer sent = this->sendEncrypt(plainLen, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::ENCRYPTION_FAILURE);
    ASSERT_EVENTS_OutputBufferBusy_SIZE(1);
    ASSERT_FALSE(this->fromPortHistory_encryptOut->at(0).data.isValid());
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_bufferReturnOut->at(0).data.getData(), sent.getData());
    ASSERT_EQ(::memcmp(firstFrame, inFlight.getData(), static_cast<size_t>(inFlight.getSize())), 0)
        << "the in-flight frame was overwritten";

    // Once returned, the component accepts frames again
    ComCfg::FrameContext context;
    Fw::Buffer returned = inFlight;
    this->invoke_to_encryptReturnIn(0, returned, context);
    (void)this->sendEncrypt(plainLen, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void AESEncryptorTester ::setKey(const U8* key, FwSizeType keyLen, Svc::Ccsds::SdlsStatus status) {
    FW_ASSERT(keyLen <= AES_256_KEY_LEN, static_cast<FwAssertArgType>(keyLen));
    (void)::memset(this->m_key, 0, sizeof this->m_key);
    if (key != nullptr) {
        (void)::memcpy(this->m_key, key, static_cast<size_t>(keyLen));
    }
    this->m_keyLen = keyLen;
    this->m_keyStatus = status;
}

Fw::Buffer AESEncryptorTester ::sendEncrypt(FwSizeType plainLen, U16 spi, U8 vcId) {
    FW_ASSERT(plainLen <= TEST_BUFFER_SIZE, static_cast<FwAssertArgType>(plainLen));
    this->clearHistory();
    for (FwSizeType i = 0; i < plainLen; i++) {
        this->m_storage[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF));
    }
    Fw::Buffer plaintext(this->m_storage, plainLen);
    ComCfg::FrameContext context;
    // The VC reaches the component on the context, the same field Svc::Ccsds::TmFramer
    // reads to build the transmitted header
    context.set_vcId(vcId);
    this->invoke_to_encryptIn(0, spi, plaintext, context);
    return plaintext;
}

void AESEncryptorTester ::assertStatus(Svc::Ccsds::SdlsStatus status) {
    // Every path through encryptIn ends in exactly one encryptOut
    ASSERT_from_encryptOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_encryptOut->at(0).status, status);
}

void AESEncryptorTester ::assertFrameDecryptsTo(const U8* expected, FwSizeType expectedLen, U8 vcId, U16 spi) {
    const Fw::Buffer out = this->fromPortHistory_encryptOut->at(0).data;
    ASSERT_EQ(out.getSize(), GCM_IV_LEN + expectedLen + GCM_TAG_LEN);
    const U8* const frame = out.getData();

    U8 aad[TM_AAD_LEN];
    buildTmAad(aad, vcId, spi);
    U8 recovered[TEST_BUFFER_SIZE];
    ASSERT_TRUE(gcmDecrypt(TEST_KEY, frame, aad, TM_AAD_LEN, frame + GCM_IV_LEN, expectedLen,
                           frame + GCM_IV_LEN + expectedLen, recovered))
        << "The emitted frame failed its MAC check";
    ASSERT_EQ(::memcmp(recovered, expected, static_cast<size_t>(expectedLen)), 0)
        << "The emitted frame did not decrypt back to the plaintext";
}

}  // namespace Ccsds

}  // namespace Svc
