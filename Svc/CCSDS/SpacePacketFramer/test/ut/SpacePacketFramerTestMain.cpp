// ======================================================================
// \title  SpacePacketFramerTestMain.cpp
// \author chammard
// \brief  cpp file for SpacePacketFramer component test main function
// ======================================================================

#include "SpacePacketFramerTester.hpp"

TEST(Nominal, testComStatusPassthrough) {
    Svc::CCSDS::SpacePacketFramerTester tester;
    tester.testComStatusPassthrough();
}

TEST(Nominal, testDataReturnPassthrough) {
    Svc::CCSDS::SpacePacketFramerTester tester;
    tester.testDataReturnPassthrough();
}

TEST(Nominal, testNominalFraming) {
    Svc::CCSDS::SpacePacketFramerTester tester;
    tester.testNominalFraming();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
