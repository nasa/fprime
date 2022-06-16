#include "gtest/gtest.h"
#include "Svc/FramingProtocol/test/ut/FramingTester.hpp"
#include "STest/Random/Random.hpp"

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

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
