// ======================================================================
// \title  FprimeFramerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for FprimeFramer component test main function
// ======================================================================

#include "FprimeFramerTester.hpp"

TEST(Nominal, testComStatusPassThrough) {
    Svc::FprimeFramerTester tester;
    tester.testComStatusPassThrough();
}

TEST(Nominal, testFrameDeallocation) {
    Svc::FprimeFramerTester tester;
    tester.testFrameDeallocation();
}

TEST(Nominal, testNominalFraming) {
    Svc::FprimeFramerTester tester;
    tester.testNominalFraming();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
