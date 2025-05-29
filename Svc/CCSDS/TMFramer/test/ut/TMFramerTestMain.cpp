// ======================================================================
// \title  TMFramerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for TMFramer component test main function
// ======================================================================

#include "TMFramerTester.hpp"

TEST(TMFramer, testComStatusPassthrough) {
    Svc::CCSDS::TMFramerTester tester;
    tester.testComStatusPassthrough();
}

TEST(TMFramer, testDataReturn) {
    Svc::CCSDS::TMFramerTester tester;
    tester.testDataReturn();
}

TEST(TMFramer, testNominalFraming) {
    Svc::CCSDS::TMFramerTester tester;
    tester.testNominalFraming();
}

TEST(TMFramer, testSeqCountWrapAround) {
    Svc::CCSDS::TMFramerTester tester;
    tester.testSeqCountWrapAround();
}

TEST(TMFramer, testInputBufferTooLarge) {
    Svc::CCSDS::TMFramerTester tester;
    tester.testInputBufferTooLarge();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
