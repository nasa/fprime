// ======================================================================
// \title  AESEncryptorTestMain.cpp
// \author vivi and claradavisb
// \brief  cpp file for AESEncryptor component test main function
// ======================================================================

#include "AESEncryptorTester.hpp"
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, EncryptNominal) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-001");
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-002");
    COMMENT("The emitted frame is IV, ciphertext and MAC, and decrypts back to the plaintext");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testEncryptNominal();
}

TEST(Nominal, CiphertextAndMacMatch) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-001");
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-002");
    COMMENT("Ciphertext and MAC match an independently computed AES-256-GCM result byte for byte");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testCiphertextAndMacMatch();
}

TEST(Nominal, AuthMaskLayout) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-002");
    COMMENT("The TM auth mask matches the ground segment's layout byte for byte");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testAuthMaskLayout();
}

TEST(Nominal, ConfiguredVcIdIsAuthenticated) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-008");
    COMMENT("configure() changes the virtual channel bound into the AAD");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testConfiguredVcIdIsAuthenticated();
}

TEST(Nominal, IvIsFreshPerFrame) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-003");
    COMMENT("Identical plaintext gets a different IV and different ciphertext each time");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testIvIsFreshPerFrame();
}

TEST(Nominal, EmptyPlaintext) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-001");
    COMMENT("A zero-length plaintext still produces a well-formed IV and MAC");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testEmptyPlaintext();
}

TEST(OffNominal, OutputCapacityBoundary) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-005");
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-010");
    COMMENT("The largest plaintext that fits is accepted; one byte more is refused");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testOutputCapacityBoundary();
}

TEST(OffNominal, KeyUnavailable) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-004");
    COMMENT("A key the key manager could not supply yields KEY_ERROR");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testKeyUnavailable();
}

TEST(OffNominal, WrongKeySize) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-004");
    COMMENT("A key of the wrong length yields KEY_ERROR rather than being used");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testWrongKeySize();
}

TEST(Nominal, PlaintextReturnedUnmodified) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-006");
    COMMENT("The plaintext buffer is handed back to its sender, unmodified");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testPlaintextReturnedUnmodified();
}

TEST(Nominal, OutputBufferReclaimed) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-007");
    COMMENT("The component reclaims its own output buffer and passes any other back upstream");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testOutputBufferReclaimed();
}

TEST(OffNominal, OutputBufferBusy) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-009");
    COMMENT("A frame arriving while the previous is in flight is dropped, not overwritten");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testOutputBufferBusy();
}

TEST(Nominal, SynchronousReturn) {
    REQUIREMENT("SVC-CCSDS-AES-ENCRYPTOR-009");
    COMMENT("Back-to-back frames succeed when the buffer returns inside the emit call");
    Svc::Ccsds::AESEncryptorTester tester;
    tester.testSynchronousReturn();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
