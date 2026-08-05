// ======================================================================
// \title  ClearTextDecryptorTestMain.cpp
// \author lestarch-autobot
// \brief  cpp file for ClearTextDecryptor component test main function
// ======================================================================

#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/ClearTextDecryptor/test/ut/ClearTextDecryptorTester.hpp"

using Svc::Ccsds::ClearTextDecryptorTester;

TEST(ClearTextDecryptor, DecryptPassThrough) {
    COMMENT("Pass a decryption request through unmodified, returning SUCCESS.");
    REQUIREMENT("SVC-CCSDS-CLEARTEXT-DECRYPTOR-001");
    REQUIREMENT("SVC-CCSDS-CLEARTEXT-DECRYPTOR-002");
    REQUIREMENT("SVC-CCSDS-CLEARTEXT-DECRYPTOR-003");
    ClearTextDecryptorTester tester;
    tester.testDecryptPassThrough();
}

TEST(ClearTextDecryptor, ReturnPassThrough) {
    COMMENT("Pass a returned buffer through to bufferReturnOut for deallocation.");
    REQUIREMENT("SVC-CCSDS-CLEARTEXT-DECRYPTOR-004");
    ClearTextDecryptorTester tester;
    tester.testReturnPassThrough();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
