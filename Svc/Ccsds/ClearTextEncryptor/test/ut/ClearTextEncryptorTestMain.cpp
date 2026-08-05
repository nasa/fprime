// ======================================================================
// \title  ClearTextEncryptorTestMain.cpp
// \author lestarch-autobot
// \brief  cpp file for ClearTextEncryptor component test main function
// ======================================================================

#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/ClearTextEncryptor/test/ut/ClearTextEncryptorTester.hpp"

using Svc::Ccsds::ClearTextEncryptorTester;

TEST(ClearTextEncryptor, EncryptPassThrough) {
    COMMENT("Pass an encryption request through unmodified, returning SUCCESS.");
    REQUIREMENT("SVC-CCSDS-CLEARTEXT-ENCRYPTOR-001");
    REQUIREMENT("SVC-CCSDS-CLEARTEXT-ENCRYPTOR-002");
    REQUIREMENT("SVC-CCSDS-CLEARTEXT-ENCRYPTOR-003");
    ClearTextEncryptorTester tester;
    tester.testEncryptPassThrough();
}

TEST(ClearTextEncryptor, ReturnPassThrough) {
    COMMENT("Pass a returned buffer through to bufferReturnOut for deallocation.");
    REQUIREMENT("SVC-CCSDS-CLEARTEXT-ENCRYPTOR-004");
    ClearTextEncryptorTester tester;
    tester.testReturnPassThrough();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
