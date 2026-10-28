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

TEST(TestNominal, ChannelInMaxPacketsTest) {
    TEST_CASE(100.1.79, "Channel present in MAX_PACKETIZER_PACKETS packets fills every packet and reads back");
    Svc::TlmPacketizerTester tester;
    tester.channelInMaxPacketsTest();
}

TEST(TestNominal, FullComBufferPacketTest) {
    TEST_CASE(100.1.80, "Packet filling the com buffer exactly round-trips its last-offset channel");
    Svc::TlmPacketizerTester tester;
    tester.fullComBufferPacketTest();
}

TEST(TestOffNominal, OversizedPacketTest) {
    TEST_CASE(100.2.7, "Packet one byte larger than the com buffer asserts at setPacketList");
    Svc::TlmPacketizerTester tester;
    tester.oversizedPacketTest();
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
    TEST_CASE(100.1.15, "Per-packet OVERRIDE_PACKET_SEND(SEND_NEVER) override silences a single packet/section");
    Svc::TlmPacketizerTester tester;
    tester.perPacketOverrideTest();
}

TEST(TestNominal, PerPacketCommandsTest) {
    TEST_CASE(100.1.16, "Per-packet commands update overrides + mirror out packetOverrideOut");
    Svc::TlmPacketizerTester tester;
    tester.perPacketCommandsTest();
}

TEST(TestNominal, ConfigInReloadTest) {
    TEST_CASE(100.1.17, "packetOverrideIn reload applies overrides to the volatile table without echo");
    Svc::TlmPacketizerTester tester;
    tester.configInReloadTest();
}

TEST(TestNominal, GetPacketConfigTest) {
    TEST_CASE(100.1.18, "GET_PACKET_CONFIG reports effective config; unknown id warns");
    Svc::TlmPacketizerTester tester;
    tester.getPacketConfigTest();
}

TEST(TestNominal, GroupRatesFollowThroughSendOverrideTest) {
    TEST_CASE(100.1.19, "Send-state override leaves rates following the group (no seeding)");
    Svc::TlmPacketizerTester tester;
    tester.groupRatesFollowThroughSendOverrideTest();
}

TEST(TestNominal, ClearPacketOverrideTest) {
    TEST_CASE(100.1.20, "CLEAR_PACKET_OVERRIDE reverts to group behavior and mirrors the clear");
    Svc::TlmPacketizerTester tester;
    tester.clearPacketOverrideTest();
}

TEST(TestNominal, MirrorAllocFailTest) {
    TEST_CASE(100.1.21, "Mirror allocation failure answers EXECUTION_ERROR and leaves the override untouched");
    Svc::TlmPacketizerTester tester;
    tester.mirrorAllocFailTest();
}

TEST(TestNominal, MirrorReturnTest) {
    TEST_CASE(100.2.6, "A packetOverrideOut mirror buffer returned on packetOverrideReturnIn is deallocated");
    Svc::TlmPacketizerTester tester;
    tester.mirrorReturnTest();
}

TEST(TestNominal, ConfigInClearReloadTest) {
    TEST_CASE(100.1.22, "packetOverrideIn GROUP_DEFAULT reload reverts a packet to group behavior");
    Svc::TlmPacketizerTester tester;
    tester.configInClearReloadTest();
}

TEST(TestNominal, ConfigInMalformedReloadTest) {
    TEST_CASE(
        100.1.23,
        "Malformed packetOverrideIn reload emits ConfigReloadFailed; buffer is returned on packetOverrideReturnOut");
    Svc::TlmPacketizerTester tester;
    tester.configInMalformedReloadTest();
}

TEST(TestNominal, SendStateResolutionTest) {
    TEST_CASE(100.1.24, "SEND_NEVER packet override stays silent in a SEND_ALWAYS group");
    Svc::TlmPacketizerTester tester;
    tester.sendStateResolutionTest();
}

TEST(TestNominal, StaleParameterShapeTest) {
    TEST_CASE(100.1.25, "SECTION_CONFIGS value of a different serialized shape is rejected at load");
    Svc::TlmPacketizerTester tester;
    tester.staleParameterShapeTest();
}

TEST(TestNominal, GroupSendCommandTest) {
    TEST_CASE(100.1.26, "CONFIGURE_GROUP_SEND responds, events and telemeters; out-of-range group is rejected");
    Svc::TlmPacketizerTester tester;
    tester.groupSendCommandTest();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
