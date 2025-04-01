// ======================================================================
// \title  FprimeFramerTestMain.cpp
// \author chammard
// \brief  cpp file for FprimeFramer component test main function
// ======================================================================

#include "FprimeFramerTester.hpp"

TEST(Nominal, testComStatusPassThrough) {
    Svc::FprimeFramerTester tester;
    tester.testComStatusPassThrough();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
