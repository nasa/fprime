// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, BasicMessaging) {
    Drv::Tester tester;
    tester.test_basic_messaging();
}

TEST(Nominal, BasicReceiveThread) {
    Drv::Tester tester;
    tester.test_receive_thread();
}

TEST(Reconnect, MultiMessaging) {
    Drv::Tester tester;
    tester.test_multiple_messaging();
}

TEST(Reconnect, ReceiveThreadReconnect) {
    Drv::Tester tester;
    tester.test_advanced_reconnect();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
