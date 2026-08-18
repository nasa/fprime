// ======================================================================
// \title  PassThroughRouterTestMain.cpp
// \author kessler
// \brief  cpp file for PassThroughRouter component test main function
// ======================================================================

#include "PassThroughRouterTester.hpp"

#include <Fw/Test/UnitTest.hpp>

TEST(PassThroughRouter, TestRouteAPacket) {
    COMMENT("Route a packet");
    Svc::PassThroughRouterTester tester;
    tester.testRouteAPacket();
}

TEST(PassThroughRouter, TestAllPacketsReturn) {
    COMMENT("Test returning a packet");
    Svc::PassThroughRouterTester tester;
    tester.testAllPacketsReturn();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
