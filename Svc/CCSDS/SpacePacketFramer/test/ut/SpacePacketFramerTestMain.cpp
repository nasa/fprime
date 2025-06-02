// ======================================================================
// \title  SpacePacketFramerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketFramer component test main function
// ======================================================================

#include "SpacePacketFramerTester.hpp"

TEST(SpacePacketFramer, testComStatusPassthrough) {
    Svc::CCSDS::SpacePacketFramerTester tester;
    tester.testComStatusPassthrough();
}

TEST(SpacePacketFramer, testDataReturnPassthrough) {
    Svc::CCSDS::SpacePacketFramerTester tester;
    tester.testDataReturnPassthrough();
}

TEST(SpacePacketFramer, testNominalFraming) {
    Svc::CCSDS::SpacePacketFramerTester tester;
    tester.testNominalFraming();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
