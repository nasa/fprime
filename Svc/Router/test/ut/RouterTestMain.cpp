// ======================================================================
// \title  RouterTestMain.cpp
// \author chammard
// \brief  cpp file for Router component test main function
// ======================================================================

#include "RouterTester.hpp"

#include <Fw/Test/UnitTest.hpp>

TEST(Route, TestComInterface) {
    COMMENT("Route a com packet");
    Svc::RouterTester tester;
    tester.testRouteComInterface();
}
TEST(Route, TestFileInterface) {
    COMMENT("Route a file packet");
    Svc::RouterTester tester;
    tester.testRouteFileInterface();
}
TEST(Route, TestUnknownInterface) {
    COMMENT("Attempt to route a packet of unknown type");
    Svc::RouterTester tester;
    tester.testRouteUnknownPacket();
}
TEST(Route, TestCommandResponse) {
    COMMENT("Handle a command response (no-op)");
    Svc::RouterTester tester;
    tester.testCommandResponse();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
