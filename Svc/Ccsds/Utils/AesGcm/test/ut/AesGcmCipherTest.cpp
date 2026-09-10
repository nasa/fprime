// ======================================================================
// \title  AesGcmCipherTest.cpp
// \brief  Unit tests for the shared AES-256-GCM cipher
// ======================================================================

#include <gtest/gtest.h>
#include <cstring>

#include "Svc/Ccsds/Utils/AesGcm/AesGcmCipher.hpp"
#include "Svc/Ccsds/Utils/SdlsAad.hpp"

namespace {

using Svc::Ccsds::SdlsKeyBuffer;
using Svc::Ccsds::SdlsStatus;
using Svc::Ccsds::Utils::AesGcmCipher;
using Svc::Ccsds::Utils::SdlsTcAad;
using Svc::Ccsds::Utils::SdlsTmAad;

// ----------------------------------------------------------------------
// Known-answer vector, shared with the AesGcmDecryptor unit test
// ----------------------------------------------------------------------

const U8 KAT_KEY[AesGcmCipher::KEY_LEN] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A,
                                           0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55,
                                           0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F};

const U8 KAT_IV[AesGcmCipher::IV_LEN] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};

const U8 KAT_PLAINTEXT[28] = {0x53, 0x44, 0x4C, 0x53, 0x20, 0x41, 0x45, 0x53, 0x2D, 0x32, 0x35, 0x36, 0x2D, 0x47,
                              0x43, 0x4D, 0x20, 0x4B, 0x41, 0x54, 0x20, 0x70, 0x61, 0x79, 0x6C, 0x6F, 0x61, 0x64};

//! Ciphertext for KAT_PLAINTEXT under KAT_KEY and KAT_IV
const U8 KAT_CIPHERTEXT[28] = {0x69, 0x41, 0x20, 0x2B, 0x57, 0xCD, 0x01, 0xF3, 0xEF, 0x20, 0xB5, 0xF4, 0x95, 0xAA,
                               0x8F, 0xF6, 0xA3, 0x8F, 0x6C, 0x5D, 0x4C, 0x3C, 0xCC, 0x2A, 0x82, 0x4F, 0x89, 0xF2};

//! MAC over the above with the TC AAD for VC 5, SPI 0x1234
const U8 KAT_MAC[AesGcmCipher::TAG_LEN] = {0xC4, 0xB0, 0x91, 0x03, 0x7F, 0xA7, 0xA4, 0xAB,
                                           0xD7, 0x25, 0xCB, 0xA2, 0xE8, 0x24, 0x08, 0xA6};

const U8 KAT_VC_ID = 5;
const U16 KAT_SPI = 0x1234;

//! The key, as the components receive it from the key manager
SdlsKeyBuffer loadKey(const U8* bytes, FwSizeType len) {
    SdlsKeyBuffer key;
    (void)::memcpy(key.getBuffAddr(), bytes, static_cast<size_t>(len));
    const Fw::SerializeStatus status = key.setBuffLen(len);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    return key;
}

//! The key buffer holds only zeros
bool keyIsWiped(const SdlsKeyBuffer& key) {
    const U8* const bytes = key.getBuffAddr();
    for (FwSizeType i = 0; i < key.getCapacity(); i++) {
        if (bytes[i] != 0) {
            return false;
        }
    }
    return true;
}

}  // namespace

// ----------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------

TEST(AesGcmCipher, ConstantsMatchAes256Gcm) {
    // Copies, since gtest binds its arguments by reference (C++14 ODR-use of static constexpr)
    const FwSizeType keyLen = AesGcmCipher::KEY_LEN;
    const FwSizeType ivLen = AesGcmCipher::IV_LEN;
    const FwSizeType tagLen = AesGcmCipher::TAG_LEN;
    const FwSizeType tcIvSize = SdlsTcAad::IV_SIZE;
    const FwSizeType tmIvSize = SdlsTmAad::IV_SIZE;
    // AES-256 keys are 256 bits; GCM uses a 96-bit IV and a 128-bit tag
    EXPECT_EQ(keyLen, 32u);
    EXPECT_EQ(ivLen, 12u);
    EXPECT_EQ(tagLen, 16u);
    // The AAD utilities reserve exactly one IV field
    EXPECT_EQ(tcIvSize, ivLen);
    EXPECT_EQ(tmIvSize, ivLen);
}

// ----------------------------------------------------------------------
// Encrypt
// ----------------------------------------------------------------------

TEST(AesGcmCipher, EncryptKnownAnswer) {
    AesGcmCipher cipher(AesGcmCipher::Direction::ENCRYPT);
    SdlsKeyBuffer key = loadKey(KAT_KEY, AesGcmCipher::KEY_LEN);
    const SdlsTcAad aad(KAT_VC_ID, KAT_SPI);
    U8 ciphertext[sizeof(KAT_PLAINTEXT)] = {};
    U8 tag[AesGcmCipher::TAG_LEN] = {};

    ASSERT_TRUE(cipher.encrypt(key, Fw::ConstByteArray(KAT_IV, AesGcmCipher::IV_LEN), aad.asByteArray(),
                               Fw::ConstByteArray(KAT_PLAINTEXT, sizeof(KAT_PLAINTEXT)),
                               Fw::ByteArray(ciphertext, sizeof(ciphertext)), Fw::ByteArray(tag, sizeof(tag))));
    EXPECT_EQ(::memcmp(ciphertext, KAT_CIPHERTEXT, sizeof(KAT_CIPHERTEXT)), 0);
    EXPECT_EQ(::memcmp(tag, KAT_MAC, sizeof(KAT_MAC)), 0);
    // The key schedule lives in the cipher; the caller's copy is wiped
    EXPECT_TRUE(keyIsWiped(key));
}

TEST(AesGcmCipher, EncryptEmptyPlaintext) {
    AesGcmCipher cipher(AesGcmCipher::Direction::ENCRYPT);
    SdlsKeyBuffer key = loadKey(KAT_KEY, AesGcmCipher::KEY_LEN);
    const SdlsTmAad aad(KAT_VC_ID, KAT_SPI);
    U8 tag[AesGcmCipher::TAG_LEN] = {};
    U8 tagAgain[AesGcmCipher::TAG_LEN] = {};

    // GCM over nothing is a pure MAC of the AAD; the tag is still produced
    ASSERT_TRUE(cipher.encrypt(key, Fw::ConstByteArray(KAT_IV, AesGcmCipher::IV_LEN), aad.asByteArray(),
                               Fw::ConstByteArray(nullptr, 0), Fw::ByteArray(nullptr, 0),
                               Fw::ByteArray(tag, sizeof(tag))));
    // Deterministic under the same key, IV, and AAD
    key = loadKey(KAT_KEY, AesGcmCipher::KEY_LEN);
    ASSERT_TRUE(cipher.encrypt(key, Fw::ConstByteArray(KAT_IV, AesGcmCipher::IV_LEN), aad.asByteArray(),
                               Fw::ConstByteArray(nullptr, 0), Fw::ByteArray(nullptr, 0),
                               Fw::ByteArray(tagAgain, sizeof(tagAgain))));
    EXPECT_EQ(::memcmp(tag, tagAgain, sizeof(tag)), 0);
}

TEST(AesGcmCipher, EncryptRejectsWrongKeyLength) {
    AesGcmCipher cipher(AesGcmCipher::Direction::ENCRYPT);
    SdlsKeyBuffer key = loadKey(KAT_KEY, 16);
    const SdlsTcAad aad(KAT_VC_ID, KAT_SPI);
    U8 ciphertext[sizeof(KAT_PLAINTEXT)] = {};
    U8 tag[AesGcmCipher::TAG_LEN] = {};

    EXPECT_FALSE(cipher.encrypt(key, Fw::ConstByteArray(KAT_IV, AesGcmCipher::IV_LEN), aad.asByteArray(),
                                Fw::ConstByteArray(KAT_PLAINTEXT, sizeof(KAT_PLAINTEXT)),
                                Fw::ByteArray(ciphertext, sizeof(ciphertext)), Fw::ByteArray(tag, sizeof(tag))));
    // Wiped even on the failure path
    EXPECT_TRUE(keyIsWiped(key));
}

// ----------------------------------------------------------------------
// Decrypt
// ----------------------------------------------------------------------

TEST(AesGcmCipher, DecryptKnownAnswer) {
    AesGcmCipher cipher(AesGcmCipher::Direction::DECRYPT);
    SdlsKeyBuffer key = loadKey(KAT_KEY, AesGcmCipher::KEY_LEN);
    const SdlsTcAad aad(KAT_VC_ID, KAT_SPI);
    U8 data[sizeof(KAT_CIPHERTEXT)];
    (void)::memcpy(data, KAT_CIPHERTEXT, sizeof(data));

    ASSERT_EQ(cipher.decrypt(key, Fw::ConstByteArray(KAT_IV, AesGcmCipher::IV_LEN), aad.asByteArray(),
                             Fw::ByteArray(data, sizeof(data)), Fw::ConstByteArray(KAT_MAC, AesGcmCipher::TAG_LEN)),
              SdlsStatus::SUCCESS);
    // Decrypted in place
    EXPECT_EQ(::memcmp(data, KAT_PLAINTEXT, sizeof(KAT_PLAINTEXT)), 0);
    EXPECT_TRUE(keyIsWiped(key));
}

TEST(AesGcmCipher, DecryptRejectsTamperedMac) {
    AesGcmCipher cipher(AesGcmCipher::Direction::DECRYPT);
    SdlsKeyBuffer key = loadKey(KAT_KEY, AesGcmCipher::KEY_LEN);
    const SdlsTcAad aad(KAT_VC_ID, KAT_SPI);
    U8 data[sizeof(KAT_CIPHERTEXT)];
    (void)::memcpy(data, KAT_CIPHERTEXT, sizeof(data));
    U8 mac[AesGcmCipher::TAG_LEN];
    (void)::memcpy(mac, KAT_MAC, sizeof(mac));
    mac[0] ^= 0x01;

    EXPECT_EQ(cipher.decrypt(key, Fw::ConstByteArray(KAT_IV, AesGcmCipher::IV_LEN), aad.asByteArray(),
                             Fw::ByteArray(data, sizeof(data)), Fw::ConstByteArray(mac, sizeof(mac))),
              SdlsStatus::MAC_VERIFICATION_FAILURE);
}

TEST(AesGcmCipher, DecryptRejectsWrongAad) {
    AesGcmCipher cipher(AesGcmCipher::Direction::DECRYPT);
    SdlsKeyBuffer key = loadKey(KAT_KEY, AesGcmCipher::KEY_LEN);
    // Same frame presented on a different virtual channel
    const SdlsTcAad aad(static_cast<U8>(KAT_VC_ID + 1), KAT_SPI);
    U8 data[sizeof(KAT_CIPHERTEXT)];
    (void)::memcpy(data, KAT_CIPHERTEXT, sizeof(data));

    EXPECT_EQ(cipher.decrypt(key, Fw::ConstByteArray(KAT_IV, AesGcmCipher::IV_LEN), aad.asByteArray(),
                             Fw::ByteArray(data, sizeof(data)), Fw::ConstByteArray(KAT_MAC, AesGcmCipher::TAG_LEN)),
              SdlsStatus::MAC_VERIFICATION_FAILURE);
}

TEST(AesGcmCipher, DecryptRejectsWrongKeyLength) {
    AesGcmCipher cipher(AesGcmCipher::Direction::DECRYPT);
    SdlsKeyBuffer key = loadKey(KAT_KEY, 16);
    const SdlsTcAad aad(KAT_VC_ID, KAT_SPI);
    U8 data[sizeof(KAT_CIPHERTEXT)];
    (void)::memcpy(data, KAT_CIPHERTEXT, sizeof(data));

    EXPECT_EQ(cipher.decrypt(key, Fw::ConstByteArray(KAT_IV, AesGcmCipher::IV_LEN), aad.asByteArray(),
                             Fw::ByteArray(data, sizeof(data)), Fw::ConstByteArray(KAT_MAC, AesGcmCipher::TAG_LEN)),
              SdlsStatus::DECRYPTION_FAILURE);
    EXPECT_TRUE(keyIsWiped(key));
}

// ----------------------------------------------------------------------
// Round trip and reuse
// ----------------------------------------------------------------------

TEST(AesGcmCipher, RoundTripAcrossReusedContexts) {
    AesGcmCipher encryptor(AesGcmCipher::Direction::ENCRYPT);
    AesGcmCipher decryptor(AesGcmCipher::Direction::DECRYPT);
    const SdlsTmAad aad(KAT_VC_ID, KAT_SPI);

    // Several frames through the same two contexts, each re-keyed
    for (U8 round = 0; round < 3; round++) {
        U8 iv[AesGcmCipher::IV_LEN];
        (void)::memcpy(iv, KAT_IV, sizeof(iv));
        iv[AesGcmCipher::IV_LEN - 1] = round;
        U8 plaintext[40];
        for (FwSizeType i = 0; i < sizeof(plaintext); i++) {
            plaintext[i] = static_cast<U8>(i + round);
        }
        U8 ciphertext[sizeof(plaintext)] = {};
        U8 tag[AesGcmCipher::TAG_LEN] = {};

        SdlsKeyBuffer key = loadKey(KAT_KEY, AesGcmCipher::KEY_LEN);
        ASSERT_TRUE(encryptor.encrypt(key, Fw::ConstByteArray(iv, sizeof(iv)), aad.asByteArray(),
                                      Fw::ConstByteArray(plaintext, sizeof(plaintext)),
                                      Fw::ByteArray(ciphertext, sizeof(ciphertext)), Fw::ByteArray(tag, sizeof(tag))));
        EXPECT_NE(::memcmp(ciphertext, plaintext, sizeof(plaintext)), 0);

        key = loadKey(KAT_KEY, AesGcmCipher::KEY_LEN);
        ASSERT_EQ(
            decryptor.decrypt(key, Fw::ConstByteArray(iv, sizeof(iv)), aad.asByteArray(),
                              Fw::ByteArray(ciphertext, sizeof(ciphertext)), Fw::ConstByteArray(tag, sizeof(tag))),
            SdlsStatus::SUCCESS);
        EXPECT_EQ(::memcmp(ciphertext, plaintext, sizeof(plaintext)), 0);
    }
}

// ----------------------------------------------------------------------
// AAD cache
// ----------------------------------------------------------------------

TEST(SdlsAadCache, RebuildsOnlyWhenKeyChanges) {
    Svc::Ccsds::Utils::SdlsAadCache<SdlsTcAad> cache;

    const SdlsTcAad& first = cache.get(KAT_VC_ID, KAT_SPI);
    EXPECT_EQ(::memcmp(first.bytes, SdlsTcAad(KAT_VC_ID, KAT_SPI).bytes, SdlsTcAad::SIZE), 0);

    // Same pair returns the same cached object
    const SdlsTcAad& again = cache.get(KAT_VC_ID, KAT_SPI);
    EXPECT_EQ(&first, &again);
    EXPECT_EQ(::memcmp(again.bytes, SdlsTcAad(KAT_VC_ID, KAT_SPI).bytes, SdlsTcAad::SIZE), 0);

    // A new VC or SA rebuilds the contents
    const SdlsTcAad& newVc = cache.get(static_cast<U8>(KAT_VC_ID + 1), KAT_SPI);
    EXPECT_EQ(::memcmp(newVc.bytes, SdlsTcAad(static_cast<U8>(KAT_VC_ID + 1), KAT_SPI).bytes, SdlsTcAad::SIZE), 0);
    const SdlsTcAad& newSa = cache.get(static_cast<U8>(KAT_VC_ID + 1), static_cast<U16>(KAT_SPI + 1));
    EXPECT_EQ(::memcmp(newSa.bytes, SdlsTcAad(static_cast<U8>(KAT_VC_ID + 1), static_cast<U16>(KAT_SPI + 1)).bytes,
                       SdlsTcAad::SIZE),
              0);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
