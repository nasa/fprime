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

// ----------------------------------------------------------------------
// Finding 1 — U16 overflow in pkt_length calculation
// packetDataLength=0xFFFF wraps to 0 without the U32-widening fix,
// causing the length guard to pass and an empty buffer to reach dataOut.
// ----------------------------------------------------------------------

TEST(SpacePacketDeframer, testPacketDataLengthMaxU16Overflow) {
    Svc::Ccsds::SpacePacketDeframerTester tester;
    tester.testPacketDataLengthMaxU16Overflow();
}

// ----------------------------------------------------------------------
// Finding 3 — graceful handling of undersized buffers
// The original FW_ASSERT aborts the FSW on deserialisation failure.
// The fix replaces it with an InvalidPacket EVR and a graceful drop.
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
