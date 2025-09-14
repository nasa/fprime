// ======================================================================
// \title  TmFramerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for TmFramer component test main function
// ======================================================================

#include "TmFramerTester.hpp"

TEST(TmFramer, testComStatusPassthrough) {
    Svc::Ccsds::TmFramerTester tester;
    tester.testComStatusPassthrough();
}

TEST(TmFramer, testDataReturn) {
    Svc::Ccsds::TmFramerTester tester;
    tester.testDataReturn();
}

TEST(TmFramer, testNominalFraming) {
    Svc::Ccsds::TmFramerTester tester;
    tester.testNominalFraming();
}

TEST(TmFramer, testSeqCountWrapAround) {
    Svc::Ccsds::TmFramerTester tester;
    tester.testSeqCountWrapAround();
}

TEST(TmFramer, testInputBufferTooLarge) {
    Svc::Ccsds::TmFramerTester tester;
    tester.testInputBufferTooLarge();
}

TEST(TmFramer, testBufferOwnershipState) {
    Svc::Ccsds::TmFramerTester tester;
    tester.testBufferOwnershipState();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
