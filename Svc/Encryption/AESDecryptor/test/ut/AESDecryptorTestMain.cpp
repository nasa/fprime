// ======================================================================
// \title  AESDecryptorTestMain.cpp
// \author vivi and claradavisb
// \brief  cpp file for AESDecryptor component test main function
// ======================================================================

#include "AESDecryptorTester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, KnownAnswer) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-001");
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("A frame built outside this repository decrypts to the expected plaintext");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testKnownAnswer();
}

TEST(Nominal, AuthMaskLayout) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("The TC auth mask matches the ground segment's layout byte for byte");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testAuthMaskLayout();
}

TEST(Nominal, DecryptNominal) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-001");
    COMMENT("A well-formed frame is decrypted in place");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testDecryptNominal();
}

TEST(Nominal, EmptyCiphertext) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-001");
    COMMENT("A frame of exactly IV and MAC is authenticated and accepted");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testEmptyCiphertext();
}

TEST(Nominal, AllocationContextPreserved) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-006");
    COMMENT("The emitted buffer remains deallocatable by its BufferManager");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testAllocationContextPreserved();
}

TEST(OffNominal, TamperedCiphertext) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-003");
    COMMENT("A flipped ciphertext bit fails the MAC check");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testTamperedCiphertext();
}

TEST(OffNominal, TamperedMac) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-003");
    COMMENT("A flipped MAC bit fails the MAC check");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testTamperedMac();
}

TEST(OffNominal, TamperedIv) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-003");
    COMMENT("A flipped IV bit fails the MAC check");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testTamperedIv();
}

TEST(OffNominal, WrongVcId) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("A frame authenticated for another virtual channel is rejected");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testWrongVcId();
}

TEST(OffNominal, WrongSecurityAssociation) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("A frame authenticated under another security association is rejected");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testWrongSecurityAssociation();
}

TEST(Nominal, ReconfigureChangesVc) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-002");
    COMMENT("configure() may be called again to change the virtual channel");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testReconfigureChangesVc();
}

TEST(Nominal, RecoversAfterMacFailure) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-003");
    COMMENT("A good frame still decrypts after one was rejected on its MAC");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testRecoversAfterMacFailure();
}

TEST(OffNominal, ShortBuffer) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-004");
    COMMENT("A buffer too short to hold an IV and a MAC is rejected");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testShortBuffer();
}

TEST(OffNominal, OversizeBuffer) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-004");
    COMMENT("A buffer larger than the frame can carry is rejected before a key is requested");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testOversizeBuffer();
}

TEST(OffNominal, KeyUnavailable) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-005");
    COMMENT("A key the key manager could not supply yields KEY_ERROR");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testKeyUnavailable();
}

TEST(OffNominal, WrongKeySize) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-005");
    COMMENT("A key of the wrong length yields KEY_ERROR rather than being used");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testWrongKeySize();
}

TEST(Nominal, BufferReturn) {
    REQUIREMENT("SVC-CCSDS-AES-DECRYPTOR-007");
    COMMENT("A buffer returned on decryptReturnIn goes back to its sender");
    Svc::Ccsds::AESDecryptorTester tester;
    tester.testBufferReturn();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
