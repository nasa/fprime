// ======================================================================
// \title  SpacePacketIdleFillerTestMain.cpp
// \author claradavisb
// \brief  cpp file for SpacePacketIdleFiller component test main function
// ======================================================================

#include "SpacePacketIdleFillerTester.hpp"

TEST(SpacePacketIdleFiller, PadsToTarget) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testPadsToTarget();
}
TEST(SpacePacketIdleFiller, IdlePacketFormat) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testIdlePacketFormat();
}
TEST(SpacePacketIdleFiller, MinimumGap) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testMinimumGap();
}
TEST(SpacePacketIdleFiller, ExactFitNoPadding) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testExactFitNoPadding();
}
TEST(SpacePacketIdleFiller, InputTooLarge) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testInputTooLarge();
}
TEST(SpacePacketIdleFiller, GapTooSmall) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testGapTooSmall();
}
TEST(SpacePacketIdleFiller, ContextForwarded) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testContextForwarded();
}
TEST(SpacePacketIdleFiller, InputReturned) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testInputReturned();
}
TEST(SpacePacketIdleFiller, StorageReusedAfterReturn) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testStorageReusedAfterReturn();
}
TEST(SpacePacketIdleFiller, StatusForwarded) {
    Svc::Ccsds::SpacePacketIdleFillerTester tester;
    tester.testStatusForwarded();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
