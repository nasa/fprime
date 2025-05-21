// ======================================================================
// \title  TMFramerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for TMFramer component test main function
// ======================================================================

#include "TMFramerTester.hpp"

TEST(Nominal, testComStatusPassthrough) {
    Svc::CCSDS::TMFramerTester tester;
    tester.testComStatusPassthrough();
}

TEST(Nominal, testNominalFraming) {
    Svc::CCSDS::TMFramerTester tester;
    tester.testNominalFraming();
}

TEST(Nominal, testSeqCountWrapAround) {
    Svc::CCSDS::TMFramerTester tester;
    tester.testSeqCountWrapAround();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
