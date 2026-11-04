// ======================================================================
// \title  AsmFramerTestMain.cpp
// \author devin
// \brief  cpp file for AsmFramer component test main function
// ======================================================================

#include "Svc/Ccsds/AsmFramer/test/ut/AsmFramerTester.hpp"

TEST(AsmFramer, testComStatusPassthrough) {
    Svc::Ccsds::AsmFramerTester tester;
    tester.testComStatusPassthrough();
}

TEST(AsmFramer, testNominalFraming) {
    Svc::Ccsds::AsmFramerTester tester;
    tester.testNominalFraming();
}

TEST(AsmFramer, testConfiguredAsm) {
    Svc::Ccsds::AsmFramerTester tester;
    tester.testConfiguredAsm();
}

TEST(AsmFramer, testDisabledAsm) {
    Svc::Ccsds::AsmFramerTester tester;
    tester.testDisabledAsm();
}

TEST(AsmFramer, testConfigureInvalid) {
    Svc::Ccsds::AsmFramerTester tester;
    tester.testConfigureInvalid();
}

TEST(AsmFramer, testFrameSizeBounds) {
    Svc::Ccsds::AsmFramerTester tester;
    tester.testFrameSizeBounds();
}

TEST(AsmFramer, testDataReturn) {
    Svc::Ccsds::AsmFramerTester tester;
    tester.testDataReturn();
}

TEST(AsmFramer, testBufferOwnershipState) {
    Svc::Ccsds::AsmFramerTester tester;
    tester.testBufferOwnershipState();
}

TEST(AsmFramer, testReentrantReturn) {
    Svc::Ccsds::AsmFramerTester tester;
    tester.testReentrantReturn();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
