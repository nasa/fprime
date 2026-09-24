// ======================================================================
// \title  ComLoggerDpTestMain.cpp
// \author tcanham
// \brief  cpp file for ComLoggerDp component test main function
// ======================================================================

#include "ComLoggerDpTester.hpp"

// Test SVC-COMLOGGERDP-001: Log Com buffers
TEST(Nominal, ComLogging) {
    Svc::ComLoggerDpTester tester;
    tester.testComLogging();
}

// Test SVC-COMLOGGER-002: StartComDp command
TEST(Nominal, StartComDp) {
    Svc::ComLoggerDpTester tester;
    tester.testStartComDp();
}

// Test SVC-COMLOGGER-003: StopComDp command
TEST(Nominal, StopComDp) {
    Svc::ComLoggerDpTester tester;
    tester.testStopComDp();
}

// Test SVC-COMLOGGER-004: UpdatePriority command
TEST(Nominal, UpdatePriority) {
    Svc::ComLoggerDpTester tester;
    tester.testUpdatePriority();
}

// Test ping functionality
TEST(Nominal, Ping) {
    Svc::ComLoggerDpTester tester;
    tester.testPing();
}

// Test container fill behavior
TEST(Nominal, ContainerFill) {
    Svc::ComLoggerDpTester tester;
    tester.testContainerFill();
}

// Test SVC-COMLOGGER-005: Buffer allocation failure handling
TEST(OffNominal, AllocationFailure) {
    Svc::ComLoggerDpTester tester;
    tester.testAllocationFailure();
}

// Test validation failure via port
TEST(OffNominal, PortValidationFailure) {
    Svc::ComLoggerDpTester tester;
    tester.testPortValidationFailure();
}

// Test telemetry functionality
TEST(Nominal, Telemetry) {
    Svc::ComLoggerDpTester tester;
    tester.testTelemetry();
}

// Test priority is preserved when starting from disabled state
TEST(Nominal, PriorityPreserved) {
    Svc::ComLoggerDpTester tester;
    tester.testPriorityPreserved();
}

// Test starting recording via port
TEST(Nominal, StartRecordingPort) {
    Svc::ComLoggerDpTester tester;
    tester.testStartRecordingPort();
}

// Test stopping recording via port
TEST(Nominal, StopRecordingPort) {
    Svc::ComLoggerDpTester tester;
    tester.testStopRecordingPort();
}

// Test clearing counters
TEST(Nominal, ClearCounters) {
    Svc::ComLoggerDpTester tester;
    tester.testClearCounters();
}

// Test buffer overflow handling
TEST(Nominal, BufferOverflow) {
    Svc::ComLoggerDpTester tester;
    tester.testBufferOverflow();
}

// Test DpBufferError event throttling
TEST(OffNominal, DpBufferErrorThrottling) {
    Svc::ComLoggerDpTester tester;
    tester.testDpBufferErrorThrottling();
}

// Test UpdatePriority when not recording
TEST(EdgeCases, UpdatePriorityNotRecording) {
    Svc::ComLoggerDpTester tester;
    tester.testUpdatePriorityNotRecording();
}

// Test UpdatePriority when no container allocated
TEST(EdgeCases, UpdatePriorityNoContainer) {
    Svc::ComLoggerDpTester tester;
    tester.testUpdatePriorityNoContainer();
}

// Test data product format validation (sentry + ComBuffer structure)
TEST(Nominal, DataProductFormat) {
    Svc::ComLoggerDpTester tester;
    tester.testDataProductFormat();
}

// Test configure() with enabled=true
TEST(Nominal, ConfigureEnabled) {
    Svc::ComLoggerDpTester tester;
    tester.testConfigureEnabled();
}

// Test reconfiguring while already recording with partial container
TEST(Nominal, ReconfigureWithPartialContainer) {
    Svc::ComLoggerDpTester tester;
    tester.testReconfigureWithPartialContainer();
}

// Test packet too large to fit in container
TEST(OffNominal, PacketTooLarge) {
    Svc::ComLoggerDpTester tester;
    tester.testPacketTooLarge();
}

// Test container overflow with partial send and retry
TEST(Nominal, ContainerOverflowRetry) {
    Svc::ComLoggerDpTester tester;
    tester.testContainerOverflowRetry();
}

// Test CLEAR_COUNTERS command functionality
TEST(Nominal, SerializationFailureCounter) {
    Svc::ComLoggerDpTester tester;
    tester.testSerializationFailureCounter();
}

// Test auto-flush after timeout with no new packets
TEST(Nominal, AutoFlush) {
    Svc::ComLoggerDpTester tester;
    tester.testAutoFlush();
}

// Test auto-flush counter resets on packet arrival
TEST(Nominal, AutoFlushResetOnPacket) {
    Svc::ComLoggerDpTester tester;
    tester.testAutoFlushResetOnPacket();
}

// Test auto-flush does not occur when disabled
TEST(Nominal, AutoFlushDisabled) {
    Svc::ComLoggerDpTester tester;
    tester.testAutoFlushDisabled();
}

// Test packetsPerContainer validation rejects excessive values
TEST(OffNominal, PacketsPerContainerTooLarge) {
    Svc::ComLoggerDpTester tester;
    tester.testPacketsPerContainerTooLarge();
}

// Test stopping when already stopped is idempotent
TEST(EdgeCases, StopWhenAlreadyStopped) {
    Svc::ComLoggerDpTester tester;
    tester.testStopWhenAlreadyStopped();
}

// Test sending ComBuffer when logging is disabled
TEST(EdgeCases, ComBufferWhenDisabled) {
    Svc::ComLoggerDpTester tester;
    tester.testComBufferWhenDisabled();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
