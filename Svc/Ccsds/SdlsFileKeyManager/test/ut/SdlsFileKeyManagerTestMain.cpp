// ======================================================================
// \title  SdlsFileKeyManagerTestMain.cpp
// \author lestarch-autobot
// \brief  cpp file for SdlsFileKeyManager component test main function
// ======================================================================

#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/Ccsds/SdlsFileKeyManager/test/ut/SdlsFileKeyManagerTester.hpp"

using Svc::Ccsds::SdlsFileKeyManagerTester;

TEST(SdlsFileKeyManager, NominalRead) {
    COMMENT("Read a key of a random configured length from the key file, returning SUCCESS.");
    REQUIREMENT("SVC-CCSDS-SDLS-FILE-KEY-MANAGER-001");
    REQUIREMENT("SVC-CCSDS-SDLS-FILE-KEY-MANAGER-002");
    REQUIREMENT("SVC-CCSDS-SDLS-FILE-KEY-MANAGER-004");
    SdlsFileKeyManagerTester tester;
    tester.testNominalRead();
}

TEST(SdlsFileKeyManager, MissingFile) {
    COMMENT("Return KEY_ERROR and emit KeyReadFailed when the key file cannot be opened.");
    REQUIREMENT("SVC-CCSDS-SDLS-FILE-KEY-MANAGER-003");
    SdlsFileKeyManagerTester tester;
    tester.testMissingFile();
}

TEST(SdlsFileKeyManager, ShortFile) {
    COMMENT("Return KEY_ERROR and emit KeyReadFailed when the key file is shorter than the key length.");
    REQUIREMENT("SVC-CCSDS-SDLS-FILE-KEY-MANAGER-003");
    SdlsFileKeyManagerTester tester;
    tester.testShortFile();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
