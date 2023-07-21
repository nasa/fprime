// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include <Fw/Test/UnitTest.hpp>
#include <Svc/FramingProtocol/DeframingProtocol.hpp>

TEST(Deframer, TestMoreNeeded) {
    COMMENT("Send a frame buffer to the mock protocol, expecting more needed");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::Tester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_MORE_NEEDED);
}
TEST(Deframer, TestSuccess) {
    COMMENT("Send a frame buffer to the mock protocol, expecting success");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::Tester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_STATUS_SUCCESS);
}
TEST(Deframer, TestBadChecksum) {
    COMMENT("Send a frame buffer to the mock protocol, expecting bad checksum");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::Tester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_INVALID_CHECKSUM);
}
TEST(Deframer, TestBadSize) {
    COMMENT("Send a frame buffer to the mock protocol, expecting bad size");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::Tester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_INVALID_SIZE);
}
TEST(Deframer, TestBadFormat) {
    COMMENT("Send a frame buffer to the mock protocol, expecting bad format");
    REQUIREMENT("SVC-DEFRAMER-001");
    REQUIREMENT("SVC-DEFRAMER-002");
    REQUIREMENT("SVC-DEFRAMER-003");
    REQUIREMENT("SVC-DEFRAMER-007");
    Svc::Tester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_INVALID_FORMAT);
}
TEST(Deframer, TestComInterface) {
    COMMENT("Route a com packet");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
    Svc::Tester tester;
    tester.test_com_interface();
}
TEST(Deframer, TestFileInterface) {
    COMMENT("Route a file packet");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    REQUIREMENT("SVC-DEFRAMER-010");
    Svc::Tester tester;
    tester.test_file_interface();
}
TEST(Deframer, TestUnknownInterface) {
    COMMENT("Attempt to route a packet of unknown type");
    REQUIREMENT("SVC-DEFRAMER-008");
    REQUIREMENT("SVC-DEFRAMER-009");
    Svc::Tester tester;
    tester.test_unknown_interface();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
