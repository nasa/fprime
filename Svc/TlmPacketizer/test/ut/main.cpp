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

TEST(TestOffNominal, SetLevelInvalidTest) {
    TEST_CASE(100.2.2, "SET_LEVEL with out-of-range level");
    Svc::TlmPacketizerTester tester;
    tester.setLevelInvalidTest();
}

TEST(TestNominal, DuplicateChannelIdMatchingSizeTest) {
    TEST_CASE(100.1.13, "Duplicate channel ID across packets with identical size");
    Svc::TlmPacketizerTester tester;
    tester.duplicateChannelIdMatchingSizeTest();
}

TEST(TestOffNominal, DuplicateChannelIdConflictingSizeTest) {
    TEST_CASE(100.2.3, "Duplicate channel ID across packets with conflicting size");
    Svc::TlmPacketizerTester tester;
    tester.duplicateChannelIdConflictingSizeTest();
}

TEST(TestOffNominal, OversizedChannelTest) {
    TEST_CASE(100.2.4, "Oversized channel value is rejected with a warning event");
    Svc::TlmPacketizerTester tester;
    tester.oversizedChannelTest();
}

TEST(TestNominal, EmptyPacketTest) {
    TEST_CASE(100.1.14, "Packet specification with no channels is accepted");
    Svc::TlmPacketizerTester tester;
    tester.emptyPacketTest();
}

TEST(TestOffNominal, NullChannelListTest) {
    TEST_CASE(100.2.5, "Non-empty packet with nullptr channel list is rejected");
    Svc::TlmPacketizerTester tester;
    tester.nullChannelListTest();
}

TEST(TestNominal, TlmGetTest) {
    TEST_CASE(100.1.8, "Get telemetry channel");
    Svc::TlmPacketizerTester tester;
    tester.getChannelValueTest();
}
TEST(TestNominal, configuredTelemetryGroupsTests) {
    TEST_CASE(100.1.9, "Configure Telem Send Levels and Rates");
    Svc::TlmPacketizerTester tester;
    tester.configuredTelemetryGroupsTests();
}
TEST(TestNominal, advancedControlGroupTests) {
    TEST_CASE(100.1.10, "Control enable sections and groups");
    Svc::TlmPacketizerTester tester;
    tester.advancedControlGroupTests();
}

TEST(TestNominal, sectionEnabledParameterTest) {
    TEST_CASE(100.1.11, "Test Section Enabled Parameter");
    Svc::TlmPacketizerTester tester;
    tester.sectionEnabledParameterTest();
}

TEST(TestNominal, sectionConfigParameterTest) {
    TEST_CASE(100.1.12, "Test Section Config Parameter");
    Svc::TlmPacketizerTester tester;
    tester.sectionConfigParameterTest();
}

TEST(TestNominal, PerPacketOverrideTest) {
    TEST_CASE(100.1.15, "Per-packet ENABLE_PACKET override disables a single packet/section");
    Svc::TlmPacketizerTester tester;
    tester.perPacketOverrideTest();
}

TEST(TestNominal, PerPacketCommandsTest) {
    TEST_CASE(100.1.16, "Per-packet commands update overrides + mirror out configOut");
    Svc::TlmPacketizerTester tester;
    tester.perPacketCommandsTest();
}

TEST(TestNominal, ConfigInReloadTest) {
    TEST_CASE(100.1.17, "configIn reload applies overrides to the volatile table without echo");
    Svc::TlmPacketizerTester tester;
    tester.configInReloadTest();
}

TEST(TestNominal, GetPacketConfigTest) {
    TEST_CASE(100.1.18, "GET_PACKET_CONFIG reports effective config; unknown id warns");
    Svc::TlmPacketizerTester tester;
    tester.getPacketConfigTest();
}

TEST(TestNominal, SeedFromEffectiveConfigTest) {
    TEST_CASE(100.1.19, "First per-packet override seeds from group-derived effective config");
    Svc::TlmPacketizerTester tester;
    tester.seedFromEffectiveConfigTest();
}

TEST(TestNominal, ClearPacketOverrideTest) {
    TEST_CASE(100.1.20, "CLEAR_PACKET_OVERRIDE reverts to group behavior and mirrors the clear");
    Svc::TlmPacketizerTester tester;
    tester.clearPacketOverrideTest();
}

TEST(TestNominal, ConfigInBatchCapTest) {
    TEST_CASE(100.1.21, "configIn batch exceeding the cap drops extras and warns");
    Svc::TlmPacketizerTester tester;
    tester.configInBatchCapTest();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
