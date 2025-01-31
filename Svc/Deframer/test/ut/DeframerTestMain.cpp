// ======================================================================
// \title  DeframerTestMain.cpp
// \author chammard
// \brief  cpp file for Deframer component test main function
// ======================================================================

#include "DeframerTester.hpp"
#include "STest/Random/Random.hpp"

TEST(Deframer, NominalFrame) {
    Svc::DeframerTester tester;
    tester.testNominalFrame();
}

TEST(Deframer, TruncatedFrame) {
    Svc::DeframerTester tester;
    tester.testTruncatedFrame();
}

TEST(Deframer, ZeroSizeFrame) {
    Svc::DeframerTester tester;
    tester.testZeroSizeFrame();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
