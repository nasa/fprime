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

TEST(FprimeDeframer, TruncatedFrame) {
    Svc::FprimeDeframerTester tester;
    tester.testTruncatedFrame();
}

TEST(FprimeDeframer, ZeroSizeFrame) {
    Svc::FprimeDeframerTester tester;
    tester.testZeroSizeFrame();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
