// ======================================================================
// \title  SpacePacketDeframerTestMain.cpp
// \author chammard
// \brief  cpp file for SpacePacketDeframer component test main function
// ======================================================================

#include "SpacePacketDeframerTester.hpp"

TEST(Nominal, testDataReturnPassthrough) {
    Svc::CCSDS::SpacePacketDeframerTester tester;
    tester.testDataReturnPassthrough();
}

TEST(Nominal, testNominalDeframing) {
    Svc::CCSDS::SpacePacketDeframerTester tester;
    tester.testNominalDeframing();
}

TEST(Nominal, testDeframingUntrackedApid) {
    Svc::CCSDS::SpacePacketDeframerTester tester;
    tester.testDeframingUntrackedApid();
}

TEST(Nominal, testDeframingIncorrectSeqCount) {
    Svc::CCSDS::SpacePacketDeframerTester tester;
    tester.testDeframingIncorrectSeqCount();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
