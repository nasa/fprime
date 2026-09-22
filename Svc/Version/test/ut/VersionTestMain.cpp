// ======================================================================
// \title  VersionTestMain.cpp
// \author sreddy
// \brief  cpp file for Version component test main function
// ======================================================================

#include "VersionTester.hpp"

TEST(Nominal, test_startup) {
    Svc::VersionTester tester;
    tester.test_startup();
}

TEST(Nominal, test_cmds) {
    Svc::VersionTester tester;
    tester.test_commands();
}

TEST(Nominal, test_custom_events_disabled) {
    Svc::VersionTester tester;
    tester.test_customEventsDisabled();
}

TEST(Nominal, test_set_version_rewrite) {
    Svc::VersionTester tester;
    tester.test_setVerRewrite();
}

TEST(Nominal, test_ports) {
    Svc::VersionTester tester;
    tester.test_ports();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
