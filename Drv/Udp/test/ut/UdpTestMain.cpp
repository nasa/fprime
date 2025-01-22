// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "UdpTester.hpp"

TEST(Nominal, UdpBasicMessaging) {
    Drv::UdpTester tester;
    tester.test_basic_messaging();
}

TEST(Nominal, UdpBasicReceiveThread) {
    Drv::UdpTester tester;
    tester.test_receive_thread();
}

TEST(Reconnect, UdpMultiMessaging) {
    Drv::UdpTester tester;
    tester.test_multiple_messaging();
}

TEST(Reconnect, UdpReceiveThreadReconnect) {
    Drv::UdpTester tester;
    tester.test_advanced_reconnect();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
