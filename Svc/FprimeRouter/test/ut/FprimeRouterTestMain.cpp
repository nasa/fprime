// ======================================================================
// \title  FprimeRouterTestMain.cpp
// \author thomas-bc
// \brief  cpp file for FprimeRouter component test main function
// ======================================================================

#include "FprimeRouterTester.hpp"

#include <Fw/Test/UnitTest.hpp>

TEST(FprimeRouter, TestComInterface) {
    COMMENT("Route a com packet");
    Svc::FprimeRouterTester tester;
    tester.testRouteComInterface();
}
TEST(FprimeRouter, TestFileInterface) {
    COMMENT("Route a file packet");
    Svc::FprimeRouterTester tester;
    tester.testRouteFileInterface();
}
TEST(FprimeRouter, TestUnknownInterface) {
    COMMENT("Route a packet of unknown type");
    Svc::FprimeRouterTester tester;
    tester.testRouteUnknownPacket();
}
TEST(FprimeRouter, TestRouteUnknownPacketUnconnected) {
    COMMENT("Attempt to route a packet of unknown type with no port connected");
    Svc::FprimeRouterTester tester(true);
    tester.testRouteUnknownPacketUnconnected();
}
TEST(FprimeRouter, TestBufferReturn) {
    COMMENT("Deallocate a returning buffer");
    Svc::FprimeRouterTester tester;
    tester.testBufferReturn();
}
TEST(FprimeRouter, TestCommandResponse) {
    COMMENT("Handle a command response (no-op)");
    Svc::FprimeRouterTester tester;
    tester.testCommandResponse();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
