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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
