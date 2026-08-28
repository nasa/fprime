// ======================================================================
// \title  AESDecryptorTester.cpp
// \author vivi and claradavisb
// \brief  cpp file for AESDecryptor component test harness implementation class
// ======================================================================

#include "AESDecryptorTester.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/Ccsds/Utils/SdlsAuthMask.hpp"

#include <openssl/evp.h>
#include <cstring>

namespace Svc {

namespace Ccsds {

const FwSizeType AESDecryptorTester::MAX_HISTORY_SIZE;
const FwEnumStoreType AESDecryptorTester::TEST_INSTANCE_ID;
const FwSizeType AESDecryptorTester::AES_256_KEY_LEN;
const FwSizeType AESDecryptorTester::GCM_IV_LEN;
const FwSizeType AESDecryptorTester::GCM_TAG_LEN;
const U8 AESDecryptorTester::TEST_VC_ID;
const U16 AESDecryptorTester::TEST_SPI;
const FwSizeType AESDecryptorTester::TEST_BUFFER_SIZE;

namespace {

// ----------------------------------------------------------------------
// Known-answer vector
// ----------------------------------------------------------------------

const U8 KAT_KEY[AESDecryptorTester::AES_256_KEY_LEN] = {
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B,
    0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F};

const U8 KAT_IV[AESDecryptorTester::GCM_IV_LEN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
                                                   0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};

const U8 KAT_PLAINTEXT[28] = {0x53, 0x44, 0x4C, 0x53, 0x20, 0x41, 0x45, 0x53, 0x2D, 0x32,
                              0x35, 0x36, 0x2D, 0x47, 0x43, 0x4D, 0x20, 0x4B, 0x41, 0x54,
                              0x20, 0x70, 0x61, 0x79, 0x6C, 0x6F, 0x61, 0x64};

//! Ciphertext for KAT_PLAINTEXT under KAT_KEY and KAT_IV
const U8 KAT_CIPHERTEXT[28] = {0x69, 0x41, 0x20, 0x2B, 0x57, 0xCD, 0x01, 0xF3, 0xEF, 0x20,
                               0xB5, 0xF4, 0x95, 0xAA, 0x8F, 0xF6, 0xA3, 0x8F, 0x6C, 0x5D,
                               0x4C, 0x3C, 0xCC, 0x2A, 0x82, 0x4F, 0x89, 0xF2};

//! MAC over the above with the TC AAD for VC 5, SPI 0x1234
const U8 KAT_MAC[AESDecryptorTester::GCM_TAG_LEN] = {0xC4, 0xB0, 0x91, 0x03, 0x7F, 0xA7, 0xA4, 0xAB,
                                                     0xD7, 0x25, 0xCB, 0xA2, 0xE8, 0x24, 0x08, 0xA6};

// ----------------------------------------------------------------------
// Independent reimplementations of what the component does
// ----------------------------------------------------------------------

//! Length of the AAD an SDLS-protected TC transfer frame authenticates: the 5-byte primary
//! header, the 2-byte SPI, and the 12-byte IV field
constexpr FwSizeType TC_AAD_LEN = 5 + 2 + AESDecryptorTester::GCM_IV_LEN;

//! Build the TC additional authenticated data. 
//! The primary header masked to 0xFC at byte 2 (the 6-bit VCID in bits
//! 7..2), the SPI, and a zeroed IV field.
void buildTcAad(U8 (&aad)[TC_AAD_LEN], U8 vcId, U16 spi) {
    (void)::memset(aad, 0, TC_AAD_LEN);
    aad[2] = static_cast<U8>((vcId << 2) & 0xFC);
    aad[5] = static_cast<U8>(spi >> 8);
    aad[6] = static_cast<U8>(spi & 0xFF);
}

//! AES-256-GCM encrypt, used to manufacture frames for the component to decrypt
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
    ASSERT_EQ(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(AESDecryptorTester::GCM_IV_LEN),
                                  nullptr),
              1);
    ASSERT_EQ(EVP_EncryptInit_ex(ctx, nullptr, nullptr, key, iv), 1);
    ASSERT_EQ(EVP_EncryptUpdate(ctx, nullptr, &len, aad, static_cast<int>(aadLen)), 1);
    if (plainLen > 0) {
        ASSERT_EQ(EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, static_cast<int>(plainLen)), 1);
    }
    int finalLen = 0;
    ASSERT_EQ(EVP_EncryptFinal_ex(ctx, ciphertext + plainLen, &finalLen), 1);
    ASSERT_EQ(finalLen, 0);
    ASSERT_EQ(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, static_cast<int>(AESDecryptorTester::GCM_TAG_LEN), tag),
              1);
    EVP_CIPHER_CTX_free(ctx);
}

}  // namespace

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

AESDecryptorTester ::AESDecryptorTester()
    : AESDecryptorGTestBase("AESDecryptorTester", AESDecryptorTester::MAX_HISTORY_SIZE),
      component("AESDecryptor"),
      m_key(),
      m_keyLen(AES_256_KEY_LEN),
      m_keyStatus(Svc::Ccsds::SdlsStatus::SUCCESS),
      m_storage(),
      m_oversize() {
    this->initComponents();
    this->connectPorts();
    this->component.configure(TEST_VC_ID);
    this->setKey(KAT_KEY, AES_256_KEY_LEN, Svc::Ccsds::SdlsStatus::SUCCESS);
}

AESDecryptorTester ::~AESDecryptorTester() {}

// ----------------------------------------------------------------------
// Handler overrides
// ----------------------------------------------------------------------

Svc::Ccsds::SdlsStatus AESDecryptorTester ::from_keyGet_handler(FwIndexType portNum,
                                                                Svc::Ccsds::SdlsKeyBuffer& key) {
    this->pushFromPortEntry_keyGet(key);
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

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void AESDecryptorTester ::testKnownAnswer() {
    // The vector was generated for VC 5 / SPI 0x1234, which the fixture is configured for
    (void)::memcpy(this->m_storage, KAT_IV, GCM_IV_LEN);
    (void)::memcpy(this->m_storage + GCM_IV_LEN, KAT_CIPHERTEXT, sizeof KAT_CIPHERTEXT);
    (void)::memcpy(this->m_storage + GCM_IV_LEN + sizeof KAT_CIPHERTEXT, KAT_MAC, GCM_TAG_LEN);
    Fw::Buffer frame(this->m_storage, GCM_IV_LEN + sizeof KAT_CIPHERTEXT + GCM_TAG_LEN);

    this->sendDecrypt(frame, TEST_SPI);

    this->assertPlaintext(KAT_PLAINTEXT, sizeof KAT_PLAINTEXT);
}

void AESDecryptorTester ::testAuthMaskLayout() {
    // Check the ends of the VCID field as well as an ordinary value: a shift or mask
    // error shows up at the boundaries first
    const U8 vcIds[] = {0, 1, TEST_VC_ID, 0x3F};
    for (FwSizeType i = 0; i < FW_NUM_ARRAY_ELEMENTS(vcIds); i++) {
        const U16 spi = static_cast<U16>(STest::Pick::lowerUpper(0, 0xFFFF));
        U8 expected[TC_AAD_LEN];
        buildTcAad(expected, vcIds[i], spi);

        const Svc::Ccsds::Utils::SdlsTcAuthMask actual(vcIds[i], spi);

        ASSERT_EQ(static_cast<FwSizeType>(sizeof actual.bytes), TC_AAD_LEN);
        ASSERT_EQ(::memcmp(actual.bytes, expected, TC_AAD_LEN), 0)
            << "TC auth mask does not match the ground segment's layout for VC " << static_cast<U32>(vcIds[i]);
    }
}

void AESDecryptorTester ::testDecryptNominal() {
    U8 plaintext[64];
    for (FwSizeType i = 0; i < sizeof plaintext; i++) {
        plaintext[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF));
    }
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);

    this->sendDecrypt(frame, TEST_SPI);

    this->assertPlaintext(plaintext, sizeof plaintext);
    // Decryption is in place: the plaintext starts where the ciphertext was, one IV in
    const Fw::Buffer out = this->fromPortHistory_decryptOut->at(0).data;
    ASSERT_EQ(out.getData(), this->m_storage + GCM_IV_LEN);
}

void AESDecryptorTester ::testEmptyCiphertext() {
    // A frame of exactly IV + MAC carries no ciphertext, but is still authenticated
    Fw::Buffer frame = this->buildFrame(nullptr, 0, TEST_VC_ID, TEST_SPI);
    ASSERT_EQ(frame.getSize(), GCM_IV_LEN + GCM_TAG_LEN);

    this->sendDecrypt(frame, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    ASSERT_EQ(this->fromPortHistory_decryptOut->at(0).data.getSize(), static_cast<FwSizeType>(0));
}

void AESDecryptorTester ::testAllocationContextPreserved() {
    // A real frame arrives from a Svc.BufferManager, which deallocates by context and
    // asserts the data pointer lies inside the slot it handed out
    const U32 context = static_cast<U32>(STest::Pick::lowerUpper(0, 0xFFFE));
    U8 plaintext[32];
    (void)::memset(plaintext, 0xA5, sizeof plaintext);
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);
    Fw::Buffer withContext(frame.getData(), frame.getSize(), context);

    this->sendDecrypt(withContext, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    const Fw::Buffer out = this->fromPortHistory_decryptOut->at(0).data;
    ASSERT_EQ(out.getContext(), context);
    ASSERT_EQ(out.getOriginalData(), this->m_storage);
}

void AESDecryptorTester ::testTamperedCiphertext() {
    U8 plaintext[48];
    (void)::memset(plaintext, 0x5A, sizeof plaintext);
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);
    // Flip one bit somewhere in the ciphertext
    const U32 offset = STest::Pick::lowerUpper(0, static_cast<U32>(sizeof plaintext) - 1);
    const FwSizeType index = GCM_IV_LEN + static_cast<FwSizeType>(offset);
    this->m_storage[index] ^= 0x01;

    this->sendDecrypt(frame, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::MAC_VERIFICATION_FAILURE);
}

void AESDecryptorTester ::testTamperedMac() {
    U8 plaintext[48];
    (void)::memset(plaintext, 0x5A, sizeof plaintext);
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);
    const FwSizeType macStart = GCM_IV_LEN + sizeof plaintext;
    const U32 offset = STest::Pick::lowerUpper(0, static_cast<U32>(GCM_TAG_LEN) - 1);
    const FwSizeType index = macStart + static_cast<FwSizeType>(offset);
    this->m_storage[index] ^= 0x01;

    this->sendDecrypt(frame, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::MAC_VERIFICATION_FAILURE);
}

void AESDecryptorTester ::testTamperedIv() {
    U8 plaintext[48];
    (void)::memset(plaintext, 0x5A, sizeof plaintext);
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);
    const FwSizeType index = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, static_cast<U32>(GCM_IV_LEN) - 1));
    this->m_storage[index] ^= 0x01;

    this->sendDecrypt(frame, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::MAC_VERIFICATION_FAILURE);
}

void AESDecryptorTester ::testWrongVcId() {
    U8 plaintext[32];
    (void)::memset(plaintext, 0x11, sizeof plaintext);
    // Authenticated for a channel other than the one the component is configured for
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID + 1, TEST_SPI);

    this->sendDecrypt(frame, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::MAC_VERIFICATION_FAILURE);
}

void AESDecryptorTester ::testWrongSecurityAssociation() {
    U8 plaintext[32];
    (void)::memset(plaintext, 0x22, sizeof plaintext);
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);

    // Same frame, presented under a different SA
    this->sendDecrypt(frame, static_cast<U16>(TEST_SPI + 1));

    this->assertStatus(Svc::Ccsds::SdlsStatus::MAC_VERIFICATION_FAILURE);
}

void AESDecryptorTester ::testReconfigureChangesVc() {
    const U8 otherVcId = TEST_VC_ID + 1;
    this->component.configure(otherVcId);

    U8 plaintext[32];
    (void)::memset(plaintext, 0x66, sizeof plaintext);
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, otherVcId, TEST_SPI);
    this->sendDecrypt(frame, TEST_SPI);
    this->assertPlaintext(plaintext, sizeof plaintext);

    // The channel it was configured for before no longer authenticates
    Fw::Buffer stale = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);
    this->sendDecrypt(stale, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::MAC_VERIFICATION_FAILURE);
}

void AESDecryptorTester ::testRecoversAfterMacFailure() {
    U8 plaintext[40];
    (void)::memset(plaintext, 0x77, sizeof plaintext);

    // Reject one frame on its MAC
    Fw::Buffer bad = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);
    this->m_storage[GCM_IV_LEN] ^= 0x01;
    this->sendDecrypt(bad, TEST_SPI);
    this->assertStatus(Svc::Ccsds::SdlsStatus::MAC_VERIFICATION_FAILURE);

    // The next good frame must still decrypt
    Fw::Buffer good = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);
    this->sendDecrypt(good, TEST_SPI);
    this->assertPlaintext(plaintext, sizeof plaintext);
}

void AESDecryptorTester ::testShortBuffer() {
    // One byte below the smallest well-formed frame
    Fw::Buffer frame(this->m_storage, GCM_IV_LEN + GCM_TAG_LEN - 1);

    this->sendDecrypt(frame, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE);
    // Rejected on shape alone, before the key is ever fetched
    ASSERT_from_keyGet_SIZE(0);
}

void AESDecryptorTester ::testOversizeBuffer() {
    // One byte past what the deployment's TC frame can carry
    Fw::Buffer frame(this->m_oversize, static_cast<Fw::Buffer::SizeType>(OVERSIZE_STORAGE));

    this->sendDecrypt(frame, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE);
    // Rejected on shape alone, before the key is ever fetched
    ASSERT_from_keyGet_SIZE(0);
}

void AESDecryptorTester ::testKeyUnavailable() {
    U8 plaintext[32];
    (void)::memset(plaintext, 0x33, sizeof plaintext);
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);
    this->setKey(nullptr, 0, Svc::Ccsds::SdlsStatus::KEY_ERROR);

    this->sendDecrypt(frame, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::KEY_ERROR);
}

void AESDecryptorTester ::testWrongKeySize() {
    U8 plaintext[32];
    (void)::memset(plaintext, 0x44, sizeof plaintext);
    Fw::Buffer frame = this->buildFrame(plaintext, sizeof plaintext, TEST_VC_ID, TEST_SPI);
    // An AES-128 key: long enough to use, and silently wrong if it were used
    this->setKey(KAT_KEY, 16, Svc::Ccsds::SdlsStatus::SUCCESS);

    this->sendDecrypt(frame, TEST_SPI);

    this->assertStatus(Svc::Ccsds::SdlsStatus::KEY_ERROR);
}

void AESDecryptorTester ::testBufferReturn() {
    this->clearHistory();
    const U32 size = STest::Pick::lowerUpper(1, static_cast<U32>(TEST_BUFFER_SIZE));
    Fw::Buffer buffer(this->m_storage, static_cast<FwSizeType>(size));
    ComCfg::FrameContext context;
    context.set_vcId(static_cast<U8>(STest::Pick::lowerUpper(0, 7)));

    this->invoke_to_decryptReturnIn(0, buffer, context);

    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_from_bufferReturnOut(0, buffer, context);
    ASSERT_from_decryptOut_SIZE(0);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void AESDecryptorTester ::setKey(const U8* key, FwSizeType keyLen, Svc::Ccsds::SdlsStatus status) {
    FW_ASSERT(keyLen <= AES_256_KEY_LEN, static_cast<FwAssertArgType>(keyLen));
    (void)::memset(this->m_key, 0, sizeof this->m_key);
    if (key != nullptr) {
        (void)::memcpy(this->m_key, key, static_cast<size_t>(keyLen));
    }
    this->m_keyLen = keyLen;
    this->m_keyStatus = status;
}

Fw::Buffer AESDecryptorTester ::buildFrame(const U8* plaintext, FwSizeType plainLen, U8 vcId, U16 spi) {
    const FwSizeType frameLen = GCM_IV_LEN + plainLen + GCM_TAG_LEN;
    FW_ASSERT(frameLen <= TEST_BUFFER_SIZE, static_cast<FwAssertArgType>(frameLen));

    for (FwSizeType i = 0; i < GCM_IV_LEN; i++) {
        this->m_storage[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF));
    }
    U8 aad[TC_AAD_LEN];
    buildTcAad(aad, vcId, spi);
    gcmEncrypt(this->m_key, this->m_storage, aad, TC_AAD_LEN, plaintext, plainLen, this->m_storage + GCM_IV_LEN,
               this->m_storage + GCM_IV_LEN + plainLen);
    return Fw::Buffer(this->m_storage, frameLen);
}

void AESDecryptorTester ::sendDecrypt(Fw::Buffer& data, U16 spi) {
    this->clearHistory();
    ComCfg::FrameContext context;
    this->invoke_to_decryptIn(0, spi, data, context);
}

void AESDecryptorTester ::assertStatus(Svc::Ccsds::SdlsStatus status) {
    // Every path through decryptIn ends in exactly one decryptOut, whatever the outcome
    ASSERT_from_decryptOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_decryptOut->at(0).status, status);
}

void AESDecryptorTester ::assertPlaintext(const U8* expected, FwSizeType expectedLen) {
    this->assertStatus(Svc::Ccsds::SdlsStatus::SUCCESS);
    const Fw::Buffer out = this->fromPortHistory_decryptOut->at(0).data;
    ASSERT_EQ(out.getSize(), expectedLen);
    ASSERT_EQ(::memcmp(out.getData(), expected, static_cast<size_t>(expectedLen)), 0);
}

}  // namespace Ccsds

}  // namespace Svc
