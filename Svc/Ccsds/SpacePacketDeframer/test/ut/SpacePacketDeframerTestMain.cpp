// ======================================================================
// \title  SpacePacketDeframerTestMain.cpp
// \author thomas-bc
// \brief  cpp file for SpacePacketDeframer component test main function
// ======================================================================

#include "SpacePacketDeframerTester.hpp"

TEST(SpacePacketDeframer, testDataReturnPassthrough) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testDataReturnPassthrough();
}

TEST(SpacePacketDeframer, testNominalDeframing) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testNominalDeframing();
}

TEST(SpacePacketDeframer, testDeframingIncorrectLength) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testDeframingIncorrectLength();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
