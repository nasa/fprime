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

TEST(SpacePacketDeframer, testPacketDataLengthMaxU16Overflow) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testPacketDataLengthMaxU16Overflow();
}

// ----------------------------------------------------------------------
// Tests for graceful handling of undersized buffers
// ----------------------------------------------------------------------

TEST(SpacePacketDeframer, testBufferExactlyHeaderSize) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testBufferExactlyHeaderSize();
}

TEST(SpacePacketDeframer, testBufferSmallerThanHeaderSize) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testBufferSmallerThanHeaderSize();
}

TEST(SpacePacketDeframer, testBufferSingleByte) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testBufferSingleByte();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
