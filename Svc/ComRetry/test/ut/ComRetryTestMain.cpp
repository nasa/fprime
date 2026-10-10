// ======================================================================
// \title  ComRetryTestMain.cpp
// \author valdaarhun
// \brief  cpp file for ComRetry test main function
// ======================================================================

#include "ComRetryTester.hpp"

// Requirement: SVC-COMRETRY-005
TEST(Nominal, NullBuffer) {
    Svc::ComRetryTester tester;
    tester.testNullBuffer();
}

// Requirement: SVC-COMRETRY-001, SVC-COMRETRY-002, SVC-COMRETRY-007, SVC-COMRETRY-008
TEST(Nominal, Send) {
    Svc::ComRetryTester tester;
    tester.testBufferSend();
}

// Requirement: SVC-COMRETRY-003, SVC-COMRETRY-004
TEST(Nominal, Retry) {
    Svc::ComRetryTester tester;
    tester.testBufferRetry();
}

// Requirement: SVC-COMRETRY-004, SVC-COMRETRY-007, SVC-COMRETRY-008
TEST(Nominal, RetryTillFailure) {
    Svc::ComRetryTester tester;
    tester.testBufferRetryTillFailure();
}

// Requirement: SVC-COMRETRY-009, SVC-COMRETRY-010
TEST(Active, InputsQueued) {
    Svc::ComRetryTester tester;
    tester.testInputsQueued();
}

// Requirement: SVC-COMRETRY-004, SVC-COMRETRY-010, SVC-COMRETRY-011, SVC-COMRETRY-012
TEST(Active, SynchronousAdapterRetry) {
    Svc::ComRetryTester tester;
    tester.testSynchronousAdapterRetry();
}

// Requirement: SVC-COMRETRY-007, SVC-COMRETRY-008, SVC-COMRETRY-011
TEST(Active, SynchronousAdapterExhaustion) {
    Svc::ComRetryTester tester;
    tester.testSynchronousAdapterExhaustion();
}

// Requirement: SVC-COMRETRY-013
TEST(Active, Ping) {
    Svc::ComRetryTester tester;
    tester.testPing();
}

// Requirement: SVC-COMRETRY-012
TEST(OffNominal, QueueFullAsserts) {
    Svc::ComRetryTester tester;
    tester.testQueueFullAsserts();
}

// Requirement: SVC-COMRETRY-013
TEST(OffNominal, PingDroppedWhenFull) {
    Svc::ComRetryTester tester;
    tester.testPingDroppedWhenFull();
}

// Requirement: SVC-COMRETRY-006, SVC-COMRETRY-007, SVC-COMRETRY-008
TEST(Nominal, NoRetries) {
    Svc::ComRetryTester tester;
    tester.testNoRetries();
}

// Requirement: SVC-COMRETRY-014
TEST(OffNominal, QueueDepthCheck) {
    Svc::ComRetryTester tester;
    tester.testQueueDepthCheck();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
