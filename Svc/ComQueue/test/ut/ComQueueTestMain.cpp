// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "ComQueueTester.hpp"
#include "Fw/Test/UnitTest.hpp"

TEST(Nominal, Send) {
    Svc::ComQueueTester tester;
    tester.testQueueSend();
}

TEST(Nominal, Flush) {
    Svc::ComQueueTester tester;
    tester.testQueueFlush();
}

TEST(OffNominal, FlushInvalidIndex) {
    Svc::ComQueueTester tester;
    tester.testQueueFlushInvalidIndex();
}

TEST(Nominal, FlushAll) {
    Svc::ComQueueTester tester;
    tester.testQueueFlushAll();
}

TEST(Nominal, Pause) {
    Svc::ComQueueTester tester;
    tester.testQueuePause();
}

TEST(Nominal, Priority) {
    Svc::ComQueueTester tester;
    tester.testPrioritySend();
}

TEST(Nominal, ExternalQueueOverflow) {
    Svc::ComQueueTester tester;
    tester.testExternalQueueOverflow();
}

TEST(Nominal, InternalQueueOverflow) {
    Svc::ComQueueTester tester;
    tester.testInternalQueueOverflow();
}

TEST(OffNominal, ComPacketAsyncIngressDrop) {
    REQUIREMENT("SVC-COMQUEUE-013");
    COMMENT("Count rejected ComBuffer ingress packets and report them after an async run drop.");
    Svc::ComQueueTester tester;
    tester.testComPacketAsyncIngressDrop();
}

TEST(OffNominal, BufferAsyncIngressDrop) {
    REQUIREMENT("SVC-COMQUEUE-013");
    COMMENT("Count rejected Buffer ingress messages and return rejected buffers to the sender.");
    Svc::ComQueueTester tester;
    tester.testBufferAsyncIngressDrop();
}

TEST(OffNominal, ConcurrentComAsyncIngressDrop) {
    REQUIREMENT("SVC-COMQUEUE-013");
    COMMENT("Verify concurrent ComBuffer ingress drop counts from multiple producer tasks.");
    Svc::ComQueueTester tester;
    tester.testConcurrentComAsyncIngressDrop();
}

TEST(OffNominal, FifoDropNewestAccounting) {
    REQUIREMENT("SVC-COMQUEUE-013");
    COMMENT("Count managed queue DROP_NEWEST overflows without inflating ingress counters.");
    Svc::ComQueueTester tester;
    tester.testFifoDropNewestAccounting();
}

TEST(OffNominal, FifoDropOldestAccounting) {
    REQUIREMENT("SVC-COMQUEUE-013");
    COMMENT("Count managed queue DROP_OLDEST replacements and return displaced buffers.");
    Svc::ComQueueTester tester;
    tester.testFifoDropOldestAccounting();
}

TEST(Nominal, DepthZeroQueue) {
    Svc::ComQueueTester tester;
    tester.testDepthZeroQueue();
}

TEST(OffNominal, AllQueuesDisabled) {
    Svc::ComQueueTester tester;
    tester.testAllQueuesDisabled();
}

TEST(Nominal, ReadyFirst) {
    Svc::ComQueueTester tester;
    tester.testReadyFirst();
}

TEST(Nominal, ContextData) {
    Svc::ComQueueTester tester;
    tester.testContextData();
}

TEST(QueueModes, FIFOMode) {
    Svc::ComQueueTester tester;
    tester.testFIFOMode();
}

TEST(QueueModes, LIFOMode) {
    Svc::ComQueueTester tester;
    tester.testLIFOMode();
}

TEST(OverflowModes, DropNewest) {
    Svc::ComQueueTester tester;
    tester.testDropNewestMode();
}

TEST(OverflowModes, DropOldest) {
    Svc::ComQueueTester tester;
    tester.testDropOldestMode();
}

TEST(CombinedModes, LIFOWithDropOldest) {
    Svc::ComQueueTester tester;
    tester.testLIFOWithDropOldest();
}

TEST(BufferQueueModes, FIFOMode) {
    Svc::ComQueueTester tester;
    tester.testBufferQueueFIFOMode();
}

TEST(BufferQueueModes, LIFOMode) {
    Svc::ComQueueTester tester;
    tester.testBufferQueueLIFOMode();
}

TEST(BufferQueueOverflowModes, DropOldest) {
    Svc::ComQueueTester tester;
    tester.testBufferQueueDropOldestMode();
}

TEST(Commands, SetQueuePriority) {
    Svc::ComQueueTester tester;
    tester.testSetQueuePriorityCommand();
}

TEST(Commands, SetQueuePriorityInvalidIndex) {
    Svc::ComQueueTester tester;
    tester.testSetQueuePriorityInvalidIndex();
}

TEST(Commands, SetQueuePriorityInvalidBufferIndex) {
    Svc::ComQueueTester tester;
    tester.testSetQueuePriorityInvalidBufferIndex();
}

TEST(Commands, SetQueuePriorityNegativeIndex) {
    Svc::ComQueueTester tester;
    tester.testSetQueuePriorityNegativeIndex();
}

TEST(Commands, SetQueuePriorityInvalidPriority) {
    Svc::ComQueueTester tester;
    tester.testSetQueuePriorityInvalidPriority();
}

TEST(Commands, SetQueuePriorityNegativePriority) {
    Svc::ComQueueTester tester;
    tester.testSetQueuePriorityNegativePriority();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
