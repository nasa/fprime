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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
