#include <limits>

#include "gtest/gtest.h"
#include "Svc/FramingProtocol/test/ut/DeframingTester.hpp"
#include "Svc/FramingProtocol/test/ut/FramingTester.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Random/Random.hpp"

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

TEST(Deframing, IncompleteHeader) {
  Svc::DeframingTester tester;
  // Start word
  tester.serializeTokenType(Svc::FpFrameHeader::START_WORD);
  U32 needed = 0;
  const Svc::DeframingProtocol::DeframingStatus status =
    tester.deframe(needed);
  ASSERT_EQ(status, Svc::DeframingProtocol::DEFRAMING_MORE_NEEDED);
}

TEST(Deframing, InvalidStartWord) {
  Svc::DeframingTester tester;
  // Start word
  tester.serializeTokenType(Svc::FpFrameHeader::START_WORD + 1);
  // Packet size
  tester.serializeTokenType(0);
  U32 needed = 0;
  const Svc::DeframingProtocol::DeframingStatus status =
    tester.deframe(needed);
  ASSERT_EQ(status, Svc::DeframingProtocol::DEFRAMING_INVALID_FORMAT);
}

TEST(Deframing, InvalidSizeIntegerOverflow) {
  Svc::DeframingTester tester;
  // Start word
  tester.serializeTokenType(Svc::FpFrameHeader::START_WORD);
  // Packet size
  const U32 maxU32 = std::numeric_limits<U32>::max();
  const U32 maxSize = maxU32 - (Svc::FpFrameHeader::SIZE + HASH_DIGEST_LENGTH);
  // Make size too big
  tester.serializeTokenType(maxSize + 1);
  U32 needed = 0;
  const Svc::DeframingProtocol::DeframingStatus status =
    tester.deframe(needed);
  ASSERT_EQ(status, Svc::DeframingProtocol::DEFRAMING_INVALID_SIZE);
}

TEST(Deframing, InvalidSizeBufferOverflow) {
  const Svc::FpFrameHeader::TokenType packetSize = 10;
  const U32 frameSize =
    Svc::FpFrameHeader::SIZE + packetSize + HASH_DIGEST_LENGTH;
  // Make the circular buffer too small to hold the frame
  Svc::DeframingTester tester(frameSize - 1);
  // Start word
  tester.serializeTokenType(Svc::FpFrameHeader::START_WORD);
  // Packet size
  tester.serializeTokenType(packetSize);
  U32 needed = 0;
  const Svc::DeframingProtocol::DeframingStatus status =
    tester.deframe(needed);
  ASSERT_EQ(status, Svc::DeframingProtocol::DEFRAMING_INVALID_SIZE);
  ASSERT_EQ(needed, frameSize);
}

TEST(Deframing, IncompleteFrame) {
  const Svc::FpFrameHeader::TokenType packetSize = 1;
  Svc::DeframingTester tester;
  // Start word
  tester.serializeTokenType(Svc::FpFrameHeader::START_WORD);
  // Packet size
  tester.serializeTokenType(packetSize);
  U32 needed = 0;
  // Deframe
  const Svc::DeframingProtocol::DeframingStatus status =
    tester.deframe(needed);
  // Check results
  ASSERT_EQ(status, Svc::DeframingProtocol::DEFRAMING_MORE_NEEDED);
  const U32 expectedFrameSize =
    Svc::FpFrameHeader::SIZE + packetSize + HASH_DIGEST_LENGTH;
  ASSERT_EQ(needed, expectedFrameSize);
}

TEST(Deframing, RandomPacketSize) {
  Svc::DeframingTester tester;
  const U32 packetSize = STest::Pick::lowerUpper(
      0,
      Svc::DeframingTester::MAX_PACKET_SIZE
  );
  tester.testNominalDeframing(packetSize);
}

TEST(Framing, CommandPacket) {
  Svc::FramingTester tester(Fw::ComPacket::FW_PACKET_COMMAND);
  tester.check();
}

TEST(Framing, FilePacket) {
  Svc::FramingTester tester(Fw::ComPacket::FW_PACKET_FILE);
  tester.check();
}

TEST(Framing, UnknownPacket) {
  Svc::FramingTester tester(Fw::ComPacket::FW_PACKET_UNKNOWN);
  tester.check();
}

// ----------------------------------------------------------------------
// Main function
// ----------------------------------------------------------------------

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
