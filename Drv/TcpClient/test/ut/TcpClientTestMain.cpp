// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "TcpClientTester.hpp"

TEST(Nominal, BasicMessaging) {
    Drv::TcpClientTester tester;
    tester.test_basic_messaging();
}

TEST(Nominal, BasicReceiveThread) {
    Drv::TcpClientTester tester;
    tester.test_receive_thread();
}

TEST(Reconnect, MultiMessaging) {
    Drv::TcpClientTester tester;
    tester.test_multiple_messaging();
}

TEST(Reconnect, ReceiveThreadReconnect) {
    Drv::TcpClientTester tester;
    tester.test_advanced_reconnect();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
