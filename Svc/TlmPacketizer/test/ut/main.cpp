// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include <gtest/gtest.h>
#include <Fw/Test/UnitTest.hpp>
#include "TlmPacketizerTester.hpp"

TEST(TestNominal, Initialization) {
    TEST_CASE(100.1.1, "Initialization");
    Svc::TlmPacketizerTester tester;
    tester.initTest();
}

TEST(TestNominal, PushTlm) {
    TEST_CASE(100.1.2, "Push Telemetry");
    Svc::TlmPacketizerTester tester;
    tester.pushTlmTest();
}

TEST(TestNominal, SendPackets) {
    TEST_CASE(100.1.2, "Send Packets");
    Svc::TlmPacketizerTester tester;
    tester.sendPacketsTest();
}

// TEST(TestNominal,SendPacketLevels) {
//
//     TEST_CASE(100.1.3,"Send Packets with levels");
//     Svc::TlmPacketizerTester tester;
//     tester.sendPacketLevelsTest();
// }

TEST(TestNominal, UpdatePacketsTest) {
    TEST_CASE(100.1.4, "Update Packets");
    Svc::TlmPacketizerTester tester;
    tester.updatePacketsTest();
}

TEST(TestNominal, PingTest) {
    TEST_CASE(100.1.5, "Ping");
    Svc::TlmPacketizerTester tester;
    tester.pingTest();
}

TEST(TestNominal, IgnoredChannelTest) {
    TEST_CASE(100.1.6, "Ignored Channels");
    Svc::TlmPacketizerTester tester;
    tester.ignoreTest();
}

TEST(TestNominal, SendPacketTest) {
    TEST_CASE(100.1.7, "Manually sent packets");
    Svc::TlmPacketizerTester tester;
    tester.sendManualPacketTest();
}
#if 0
TEST(TestNominal,SetPacketLevelTest) {

    TEST_CASE(100.1.78,"Set packet level");
    Svc::TlmPacketizerTester tester;
    tester.setPacketLevelTest();
}
#endif
TEST(TestOffNominal, NonPacketizedChannelTest) {
    TEST_CASE(100.2.1, "Non-packetized Channels");
    Svc::TlmPacketizerTester tester;
    tester.nonPacketizedChannelTest();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
