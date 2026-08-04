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
    COMMENT("Return a buffer via fileBufferReturnIn");
    Svc::FprimeRouterTester tester;
    tester.testBufferReturn();
}
TEST(FprimeRouter, TestCommandResponse) {
    COMMENT("Handle a command response (no-op)");
    Svc::FprimeRouterTester tester;
    tester.testCommandResponse();
}
TEST(FprimeRouter, TestFileContextRoundTrip) {
    COMMENT("A file buffer's context is restored on fileBufferReturnIn");
    Svc::FprimeRouterTester tester;
    tester.testFileContextRoundTrip();
}
TEST(FprimeRouter, TestMultiBufferContextRoundTrip) {
    COMMENT("Multiple outstanding buffers each restore their own context, out of order");
    Svc::FprimeRouterTester tester;
    tester.testMultiBufferContextRoundTrip();
}
TEST(FprimeRouter, TestFileOutContextTableFull) {
    COMMENT("Overflowing the context table on the fileOut path emits an event and degrades gracefully");
    Svc::FprimeRouterTester tester;
    tester.testContextTableFull(Fw::ComPacketType::FW_PACKET_FILE);
}
TEST(FprimeRouter, TestUnknownDataOutContextTableFull) {
    COMMENT("Overflowing the context table on the unknownDataOut path emits an event and degrades gracefully");
    Svc::FprimeRouterTester tester;
    tester.testContextTableFull(Fw::ComPacketType::FW_PACKET_UNKNOWN);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
