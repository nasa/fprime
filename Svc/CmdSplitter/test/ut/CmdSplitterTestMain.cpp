// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "CmdSplitterTester.hpp"

TEST(Nominal, Local) {
    Svc::CmdSplitterTester tester;
    tester.test_local_routing();
}

TEST(Nominal, Remote) {
    Svc::CmdSplitterTester tester;
    tester.test_remote_routing();
}

TEST(Nominal, Forwarding) {
    Svc::CmdSplitterTester tester;
    tester.test_response_forwarding();
}

TEST(Error, BadCommands) {
    Svc::CmdSplitterTester tester;
    tester.test_error_routing();
}




int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
