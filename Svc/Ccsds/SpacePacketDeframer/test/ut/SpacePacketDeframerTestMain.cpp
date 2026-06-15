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

TEST(SpacePacketDeframer, testInvalidPacketIdentificationControlFields) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testInvalidPacketIdentificationControlFields();
}

TEST(SpacePacketDeframer, testInvalidPacketType) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testInvalidPacketType();
}

TEST(SpacePacketDeframer, testInvalidSecondaryHeaderFlag) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testInvalidSecondaryHeaderFlag();
}

TEST(SpacePacketDeframer, testInvalidSequenceFlags) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testInvalidSequenceFlags();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
