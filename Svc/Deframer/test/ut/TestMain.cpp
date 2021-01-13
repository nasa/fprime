// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Deframer, test_incoming_frame_with_valid_size) {
    Svc::Tester tester;
    tester.test_incoming_frame(4, 4);
}

TEST(Deframer, test_incoming_frame_with_invalid_size) {
    Svc::Tester tester;
    tester.test_incoming_frame(1024, 1023);
}

TEST(Deframer, test_route_packet_command) {
    Svc::Tester tester;
    tester.test_route(Fw::ComPacket::FW_PACKET_COMMAND);
}

TEST(Deframer, test_route_packet_file) {
    Svc::Tester tester;
    tester.test_route(Fw::ComPacket::FW_PACKET_FILE);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
