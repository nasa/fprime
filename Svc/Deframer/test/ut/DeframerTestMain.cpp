// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "DeframerTester.hpp"
#include <Fw/Test/UnitTest.hpp>
#include <Svc/FramingProtocol/DeframingProtocol.hpp>

TEST(Deframer, TestMoreNeeded) {
    COMMENT("Send a frame buffer to the mock protocol, expecting more needed");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::DeframerTester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_MORE_NEEDED);
}
TEST(Deframer, TestSuccess) {
    COMMENT("Send a frame buffer to the mock protocol, expecting success");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::DeframerTester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_STATUS_SUCCESS);
}
TEST(Deframer, TestBadChecksum) {
    COMMENT("Send a frame buffer to the mock protocol, expecting bad checksum");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::DeframerTester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_INVALID_CHECKSUM);
}
TEST(Deframer, TestBadSize) {
    COMMENT("Send a frame buffer to the mock protocol, expecting bad size");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::DeframerTester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_INVALID_SIZE);
}
TEST(Deframer, TestBadFormat) {
    COMMENT("Send a frame buffer to the mock protocol, expecting bad format");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::DeframerTester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_INVALID_FORMAT);
}
TEST(Deframer, TestComInterface) {
    COMMENT("Route a com packet");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
    Svc::DeframerTester tester;
    tester.test_com_interface();
}
TEST(Deframer, TestFileInterface) {
    COMMENT("Route a file packet");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
    Svc::DeframerTester tester;
    tester.test_file_interface();
}
TEST(Deframer, TestUnknownInterface) {
    COMMENT("Attempt to route a packet of unknown type");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    Svc::DeframerTester tester;
    tester.test_unknown_interface();
}
TEST(Deframer, TestCommandResponse) {
    COMMENT("Handle a command response (no-op)");
    Svc::DeframerTester tester;
    tester.testCommandResponse();
}
TEST(Deframer, TestCommandPacketTooLarge) {
    COMMENT("Attempt to route a command packet that is too large");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    Svc::DeframerTester tester;
    tester.testCommandPacketTooLarge();
}
TEST(Deframer, TestPacketBufferTooSmall) {
    COMMENT("Attempt to route a packet that is too small");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    Svc::DeframerTester tester;
    tester.testPacketBufferTooSmall();
}

TEST(Deframer, TestBufferOutUnconnected) {
    COMMENT("Test routing a file packet when bufferOut is unconnected");
    REQUIREMENT("SVC-DEFRAMER-011");
    Svc::DeframerTester tester(Svc::DeframerTester::ConnectStatus::UNCONNECTED);
    tester.testBufferOutUnconnected();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
