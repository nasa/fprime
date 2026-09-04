// ======================================================================
// \title  AesGcmDecryptorTestMain.cpp
// \author vivi and claradavisb
// \brief  cpp file for AesGcmDecryptor component test main function
// ======================================================================

#include "AesGcmDecryptorTester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, KnownAnswer) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-001");
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("A frame built outside this repository decrypts to the expected plaintext");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testKnownAnswer();
}

TEST(Nominal, AuthMaskLayout) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("The TC auth mask matches the ground segment's layout byte for byte");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testAuthMaskLayout();
}

TEST(Nominal, DecryptNominal) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-001");
    COMMENT("A well-formed frame is decrypted in place");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testDecryptNominal();
}

TEST(Nominal, EmptyCiphertext) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-001");
    COMMENT("A frame of exactly IV and MAC is authenticated and accepted");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testEmptyCiphertext();
}

TEST(Nominal, AllocationContextPreserved) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-006");
    COMMENT("The emitted buffer remains deallocatable by its BufferManager");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testAllocationContextPreserved();
}

TEST(OffNominal, TamperedCiphertext) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-003");
    COMMENT("A flipped ciphertext bit fails the MAC check");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testTamperedCiphertext();
}

TEST(OffNominal, TamperedMac) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-003");
    COMMENT("A flipped MAC bit fails the MAC check");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testTamperedMac();
}

TEST(OffNominal, TamperedIv) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-003");
    COMMENT("A flipped IV bit fails the MAC check");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testTamperedIv();
}

TEST(OffNominal, WrongVcId) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("A frame authenticated for another virtual channel is rejected");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testWrongVcId();
}

TEST(OffNominal, WrongSecurityAssociation) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("A frame authenticated under another security association is rejected");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testWrongSecurityAssociation();
}

TEST(Nominal, VcFromContext) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("The virtual channel authenticated in the AAD comes from the frame context");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testVcFromContext();
}

TEST(Nominal, RecoversAfterMacFailure) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-003");
    COMMENT("A good frame still decrypts after one was rejected on its MAC");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testRecoversAfterMacFailure();
}

TEST(OffNominal, ShortBuffer) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-004");
    COMMENT("A buffer too short to hold an IV and a MAC is rejected");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testShortBuffer();
}

TEST(OffNominal, OversizeBuffer) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-004");
    COMMENT("A buffer larger than the frame can carry is rejected before a key is requested");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testOversizeBuffer();
}

TEST(OffNominal, KeyUnavailable) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-005");
    COMMENT("A key the key manager could not supply yields KEY_ERROR");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testKeyUnavailable();
}

TEST(OffNominal, WrongKeySize) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-005");
    COMMENT("A key of the wrong length yields KEY_ERROR rather than being used");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testWrongKeySize();
}

TEST(Nominal, BufferReturn) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-007");
    COMMENT("A buffer returned on decryptReturnIn goes back to its sender");
    Svc::Ccsds::AesGcmDecryptorTester tester;
    tester.testBufferReturn();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
