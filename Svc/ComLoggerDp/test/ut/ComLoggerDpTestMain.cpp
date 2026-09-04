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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
