// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "TcpServerTester.hpp"

TEST(Nominal, TcpServerBasicMessaging) {
    Drv::TcpServerTester tester;
    tester.test_basic_messaging();
}

TEST(Nominal, TcpServerBasicReceiveThread) {
    Drv::TcpServerTester tester;
    tester.test_receive_thread();
}

TEST(Reconnect, TcpServerMultiMessaging) {
    Drv::TcpServerTester tester;
    tester.test_multiple_messaging();
}

TEST(Reconnect, TcpServerReceiveThreadReconnect) {
    Drv::TcpServerTester tester;
    tester.test_advanced_reconnect();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
