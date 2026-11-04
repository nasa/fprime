// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "GenericHubTester.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, TestIo) {
    Svc::GenericHubTester tester;
    tester.test_in_out();
}

TEST(Nominal, TestBufferIo) {
    Svc::GenericHubTester tester;
    tester.test_buffer_io();
}

TEST(Nominal, TestRandomIo) {
    Svc::GenericHubTester tester;
    tester.test_random_io();
}

TEST(Nominal, TestEvents) {
    Svc::GenericHubTester tester;
    tester.test_events();
}

TEST(Nominal, TestTelemetry) {
    Svc::GenericHubTester tester;
    tester.test_telemetry();
}

TEST(Nominal, TestEventsBoundarySizes) {
    Svc::GenericHubTester tester;
    tester.test_events_size(0);
    tester.test_events_size(FW_LOG_BUFFER_MAX_SIZE);
}

TEST(Nominal, TestTelemetryBoundarySizes) {
    Svc::GenericHubTester tester;
    tester.test_telemetry_size(0);
    tester.test_telemetry_size(FW_TLM_BUFFER_MAX_SIZE);
}

TEST(Nominal, TestCommands) {
    Svc::GenericHubTester tester;
    tester.test_commands();
}

TEST(Nominal, TestCommandsNonZeroPort) {
    Svc::GenericHubTester tester;
    tester.test_commands_nonzero_port();
}

TEST(Invalid, TestDeserializationGuards) {
    Svc::GenericHubTester tester;
    tester.test_invalid_deserialization_paths();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
