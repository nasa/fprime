#include <limits>

#include "Fw/Test/UnitTest.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Random/Random.hpp"
#include "Svc/FramingProtocol/test/ut/DeframingTester.hpp"
#include "Svc/FramingProtocol/test/ut/FramingTester.hpp"
#include "gtest/gtest.h"

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

TEST(Deframing, IncompleteHeader) {
  COMMENT("Apply deframing to a frame with an incomplete header");
  REQUIREMENT("Svc-FramingProtocol-002");
  REQUIREMENT("Svc-FramingProtocol-003");
  Svc::DeframingTester tester;
  // Start word
  tester.serializeTokenType(Svc::FpFrameHeader::START_WORD);
  U32 needed = 0;
  const Svc::DeframingProtocol::DeframingStatus status =
    tester.deframe(needed);
  ASSERT_EQ(status, Svc::DeframingProtocol::DEFRAMING_MORE_NEEDED);
}

TEST(Deframing, InvalidStartWord) {
  COMMENT("Apply deframing to a frame with an invalid start word");
  REQUIREMENT("Svc-FramingProtocol-002");
  REQUIREMENT("Svc-FramingProtocol-003");
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
  COMMENT("Apply deframing to a frame with an invalid packet size due to integer overflow");
  REQUIREMENT("Svc-FramingProtocol-002");
  REQUIREMENT("Svc-FramingProtocol-003");
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
  COMMENT("Apply deframing to a frame with an invalid packet size due to buffer overflow");
  REQUIREMENT("Svc-FramingProtocol-002");
  REQUIREMENT("Svc-FramingProtocol-003");
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
  COMMENT("Apply deframing to an incomplete frame");
  REQUIREMENT("Svc-FramingProtocol-002");
  REQUIREMENT("Svc-FramingProtocol-003");
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

TEST(Deframing, ZeroPacketSize) {
  COMMENT("Apply deframing to a valid frame with packet size zero");
  REQUIREMENT("Svc-FramingProtocol-002");
  REQUIREMENT("Svc-FramingProtocol-003");
  Svc::DeframingTester tester;
  const U32 packetSize = 0;
  tester.testNominalDeframing(packetSize);
}

TEST(Deframing, RandomPacketSize) {
  COMMENT("Apply deframing to a valid frame with a random packet size");
  REQUIREMENT("Svc-FramingProtocol-002");
  REQUIREMENT("Svc-FramingProtocol-003");
  Svc::DeframingTester tester;
  const U32 packetSize = STest::Pick::lowerUpper(
      0,
      Svc::DeframingTester::MAX_PACKET_SIZE
  );
  tester.testNominalDeframing(packetSize);
}

TEST(Deframing, MaxPacketSize) {
  COMMENT("Apply deframing to a valid frame with maximum packet size for the test buffer");
  REQUIREMENT("Svc-FramingProtocol-002");
  REQUIREMENT("Svc-FramingProtocol-003");
  Svc::DeframingTester tester;
  const U32 packetSize = Svc::DeframingTester::MAX_PACKET_SIZE;
  tester.testNominalDeframing(packetSize);
}

TEST(Deframing, BadChecksum) {
  COMMENT("Apply deframing to a frame with a bad checksum");
  REQUIREMENT("Svc-FramingProtocol-002");
  REQUIREMENT("Svc-FramingProtocol-003");
  Svc::DeframingTester tester;
  const U32 packetSize = STest::Pick::lowerUpper(
      0,
      Svc::DeframingTester::MAX_PACKET_SIZE
  );
  tester.testBadChecksum(packetSize);
}

TEST(Framing, CommandPacket) {
  COMMENT("Apply framing to a command packet");
  REQUIREMENT("Svc-FramingProtocol-001");
  REQUIREMENT("Svc-FramingProtocol-003");
  Svc::FramingTester tester(Fw::ComPacket::FW_PACKET_COMMAND);
  tester.check();
}

TEST(Framing, FilePacket) {
  COMMENT("Apply framing to a file packet");
  REQUIREMENT("Svc-FramingProtocol-001");
  REQUIREMENT("Svc-FramingProtocol-003");
  Svc::FramingTester tester(Fw::ComPacket::FW_PACKET_FILE);
  tester.check();
}

TEST(Framing, UnknownPacket) {
  COMMENT("Apply framing to a packet of unknown type");
  REQUIREMENT("Svc-FramingProtocol-001");
  REQUIREMENT("Svc-FramingProtocol-003");
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
