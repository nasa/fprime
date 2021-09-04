// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"
#include <Svc/FramingProtocol/DeframingProtocol.hpp>

TEST(Deframer, TestMoreNeeded) {
    Svc::Tester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_MORE_NEEDED);
}
TEST(Deframer, TestSuccess) {
    Svc::Tester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_STATUS_SUCCESS);
}
TEST(Deframer, TestBadChecksum) {
    Svc::Tester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_INVALID_CHECKSUM);
}
TEST(Deframer, TestBadSize) {
    Svc::Tester tester;
    tester.test_incoming_frame(Svc::DeframingProtocol::DEFRAMING_INVALID_CHECKSUM);
}
TEST(Deframer, TestComInterface) {
    Svc::Tester tester;
    tester.test_com_interface();
}
TEST(Deframer, TestBufferInterface) {
    Svc::Tester tester;
    tester.test_buffer_interface();
}
TEST(Deframer, TestUnknownInterface) {
    Svc::Tester tester;
    tester.test_unnknown_interface();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
