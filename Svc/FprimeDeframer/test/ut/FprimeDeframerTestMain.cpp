// ======================================================================
// \title  FprimeDeframerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for FprimeDeframer component test main function
// ======================================================================

#include "FprimeDeframerTester.hpp"
#include "STest/Random/Random.hpp"

TEST(FprimeDeframer, NominalFrame) {
    Svc::FprimeDeframerTester tester;
    tester.testNominalFrame();
}

TEST(FprimeDeframer, NominalFrameApid) {
    Svc::FprimeDeframerTester tester;
    tester.testNominalFrameApid();
}

TEST(FprimeDeframer, TruncatedFrame) {
    Svc::FprimeDeframerTester tester;
    tester.testTruncatedFrame();
}

TEST(FprimeDeframer, ZeroSizeFrame) {
    Svc::FprimeDeframerTester tester;
    tester.testZeroSizeFrame();
}

TEST(FprimeDeframer, testIncorrectLengthToken) {
    Svc::FprimeDeframerTester tester;
    tester.testIncorrectLengthToken();
}

TEST(FprimeDeframer, testIncorrectStartWord) {
    Svc::FprimeDeframerTester tester;
    tester.testIncorrectStartWord();
}

TEST(FprimeDeframer, testIncorrectCrc) {
    Svc::FprimeDeframerTester tester;
    tester.testIncorrectCrc();
}

TEST(FprimeDeframer, testDataReturn) {
    Svc::FprimeDeframerTester tester;
    tester.testDataReturn();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
