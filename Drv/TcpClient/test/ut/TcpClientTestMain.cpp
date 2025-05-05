// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "TcpClientTester.hpp"


TEST(Nominal, TcpClientBasicMessaging) {
    Drv::TcpClientTester tester;
    tester.test_basic_messaging();
}

TEST(Nominal, TcpClientBasicReceiveThread) {
    Drv::TcpClientTester tester;
    tester.test_receive_thread();
}

TEST(Reconnect, TcpClientMultiMessaging) {
    Drv::TcpClientTester tester;
    tester.test_multiple_messaging();
}

TEST(Reconnect, TcpClientReceiveThreadReconnect) {
    Drv::TcpClientTester tester;
    tester.test_advanced_reconnect();
}

TEST(AutoConnect, AutoConnectOnSendOff) {
    Drv::TcpClientTester tester;
    tester.test_no_automatic_send_connection();
}

TEST(AutoConnect, AutoConnectOnRecvOff) {
    Drv::TcpClientTester tester;
    tester.test_no_automatic_recv_connection();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
