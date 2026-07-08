// ======================================================================
// \title  Os/Generic/test/ut/PriorityMemQueueInputValidationTests.cpp
// \author B. Duckett
// \brief  cpp file for input validation tests for PriorityMemQueue implementation
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#include <gtest/gtest.h>
#include <limits>
#include "Fw/Types/MemAllocator.hpp"
#include "Fw/Types/String.hpp"
#include "Os/Generic/PriorityMemQueue.hpp"

// Test helper to access private members
class PriorityMemQueueTestHelper {
  public:
    static void resetConfig() {
        // Use the public resetConfig() API
        Os::Generic::PriorityMemQueue::resetConfig();
    }
};

// Test fixture
class PriorityMemQueueInputValidation : public ::testing::Test {
  protected:
    void SetUp() override { PriorityMemQueueTestHelper::resetConfig(); }

    void TearDown() override { PriorityMemQueueTestHelper::resetConfig(); }
};

// ========================================================================
// Boundary/validation tests using table-driven approach
// ========================================================================

struct ValidationCase {
    const char* name;
    FwQueuePriorityType priority;
    FwSizeType msgSize;
    Os::QueueInterface::Status expected;
};

class BoundaryTest : public PriorityMemQueueInputValidation, public ::testing::WithParamInterface<ValidationCase> {};

TEST_P(BoundaryTest, PriorityAndSizeBoundaries) {
    auto testCase = GetParam();
    Os::Generic::PriorityMemQueue queue;

    // Configure all priorities for comprehensive testing
    Os::Generic::PriorityMemQueue::QueuePriorityConfig cfgs[Os::Generic::Queue::MAX_PRIORITIES];
    for (FwSizeType i = 0; i < Os::Generic::Queue::MAX_PRIORITIES; ++i) {
        cfgs[i] = {static_cast<FwQueuePriorityType>(i), 128, 5};
    }
    Os::Generic::PriorityMemQueue::QueueConfig qCfg = {100, Os::Generic::Queue::MAX_PRIORITIES, cfgs};
    Os::Generic::PriorityMemQueue::QueueConfig qCfgs[] = {qCfg};
    Os::Generic::PriorityMemQueue::configure(qCfgs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(100, Fw::String("Q"), 10, 128));

    U8 buf[129];
    auto status = queue.send(buf, testCase.msgSize, testCase.priority, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(testCase.expected, status) << "Test: " << testCase.name;

    queue.teardown();
    Os::Generic::PriorityMemQueue::resetConfig();
}

INSTANTIATE_TEST_SUITE_P(AllBoundaries,
                         BoundaryTest,
                         ::testing::Values(ValidationCase{"MinPriority", 0, 1, Os::QueueInterface::Status::OP_OK},
                                           ValidationCase{"MaxPriority", Os::Generic::Queue::MAX_PRIORITIES - 1, 1,
                                                          Os::QueueInterface::Status::OP_OK},
                                           ValidationCase{"InvalidPriority", Os::Generic::Queue::MAX_PRIORITIES, 10,
                                                          Os::QueueInterface::Status::INVALID_PRIORITY},
                                           ValidationCase{"InvalidPriorityPlus", Os::Generic::Queue::MAX_PRIORITIES + 1,
                                                          10, Os::QueueInterface::Status::INVALID_PRIORITY},
                                           ValidationCase{"MaxMsgSize", 0, 128, Os::QueueInterface::Status::OP_OK},
                                           ValidationCase{"OversizeMsgSize", 0, 129,
                                                          Os::QueueInterface::Status::SIZE_MISMATCH}));

// Uninitialized queue operations
TEST_F(PriorityMemQueueInputValidation, UninitializedOperations) {
    Os::Generic::PriorityMemQueue queue;
    U8 buf[128];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    // Send on uninitialized queue
    EXPECT_EQ(Os::QueueInterface::Status::UNINITIALIZED,
              queue.send(buf, 10, 0, Os::QueueInterface::BlockingType::NONBLOCKING));

    // Receive on uninitialized queue
    EXPECT_EQ(Os::QueueInterface::Status::UNINITIALIZED,
              queue.receive(buf, 128, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority));
}

// Priority enable/disable behavior
TEST_F(PriorityMemQueueInputValidation, PriorityEnableDisable) {
    Os::Generic::PriorityMemQueue::QueuePriorityConfig cfgs[] = {{0, 128, 10}, {1, 128, 10}, {2, 128, 10}};
    Os::Generic::PriorityMemQueue::QueueConfig qCfg = {150, 3, cfgs};
    Os::Generic::PriorityMemQueue::QueueConfig qCfgs[] = {qCfg};
    Os::Generic::PriorityMemQueue::configure(qCfgs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    Os::Generic::PriorityMemQueue queue;
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(150, Fw::String("Q"), 10, 128));

    U8 buf[128] = {0xAA};
    auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(queue.getHandle());

    // Send to priority 1, disable it, verify priority 0 received first
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.send(buf, 1, 1, Os::QueueInterface::BlockingType::NONBLOCKING));
    handle->disablePriority(1);

    buf[0] = 0xBB;
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.send(buf, 1, 0, Os::QueueInterface::BlockingType::NONBLOCKING));

    FwSizeType actualSize;
    FwQueuePriorityType priority;
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK,
              queue.receive(buf, 128, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority));
    EXPECT_EQ(0, priority);  // Priority 0 received first (1 disabled)
    EXPECT_EQ(0xBB, buf[0]);

    // Re-enable priority 1
    handle->enablePriority(1);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK,
              queue.receive(buf, 128, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority));
    EXPECT_EQ(1, priority);
    EXPECT_EQ(0xAA, buf[0]);

    queue.teardown();
}

// Required vs non-required priority sizing
TEST_F(PriorityMemQueueInputValidation, RequiredSizing) {
    Os::Generic::PriorityMemQueue::QueuePriorityConfig cfg[] = {{0, 128, 10}};
    Os::Generic::PriorityMemQueue::QueueConfig qCfg = {200, 1, cfg};
    Os::Generic::PriorityMemQueue::QueueConfig qCfgs[] = {qCfg};

    // Required=true: configured priorities work
    Os::Generic::PriorityMemQueue::configure(qCfgs, 1, true,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
    Os::Generic::PriorityMemQueue queue1;
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue1.create(200, Fw::String("Q1"), 10, 128));
    U8 buf[128] = {1, 2, 3};
    EXPECT_EQ(Os::QueueInterface::Status::OP_OK, queue1.send(buf, 3, 0, Os::QueueInterface::BlockingType::NONBLOCKING));
    queue1.teardown();

    // Required=false: unconfigured priorities fall back to default
    PriorityMemQueueTestHelper::resetConfig();
    Os::Generic::PriorityMemQueue::configure(qCfgs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
    Os::Generic::PriorityMemQueue queue2;
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue2.create(200, Fw::String("Q2"), 10, 128));
    EXPECT_EQ(Os::QueueInterface::Status::OP_OK, queue2.send(buf, 3, 1, Os::QueueInterface::BlockingType::NONBLOCKING));
    queue2.teardown();
}

// Message size and queue depth boundaries (table-driven)
TEST_F(PriorityMemQueueInputValidation, DepthAndSizeLimits) {
    struct {
        FwSizeType depth, msgSize;
    } cases[] = {{1, 128}, {100, 512}};

    for (auto& c : cases) {
        Os::Generic::PriorityMemQueue queue;
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(1, Fw::String("Q"), c.depth, c.msgSize));

        U8 buf[512];
        // Fill to depth
        for (FwSizeType i = 0; i < c.depth; ++i) {
            EXPECT_EQ(Os::QueueInterface::Status::OP_OK,
                      queue.send(buf, 1, 0, Os::QueueInterface::BlockingType::NONBLOCKING));
        }
        // Next send should fail (full)
        EXPECT_EQ(Os::QueueInterface::Status::FULL,
                  queue.send(buf, 1, 0, Os::QueueInterface::BlockingType::NONBLOCKING));

        queue.teardown();
    }
}

// Explicit edge validation for MAX_PRIORITIES-1 across all configured priorities
TEST_F(PriorityMemQueueInputValidation, MaxPriorityEdgeAllConfigs) {
    Os::Generic::PriorityMemQueue queue;

    // Configure all priorities up to MAX_PRIORITIES
    Os::Generic::PriorityMemQueue::QueuePriorityConfig cfgs[Os::Generic::Queue::MAX_PRIORITIES];
    for (FwSizeType i = 0; i < Os::Generic::Queue::MAX_PRIORITIES; ++i) {
        cfgs[i] = {static_cast<FwQueuePriorityType>(i), 128, 5};
    }
    Os::Generic::PriorityMemQueue::QueueConfig qCfg = {101, Os::Generic::Queue::MAX_PRIORITIES, cfgs};
    Os::Generic::PriorityMemQueue::QueueConfig qCfgs[] = {qCfg};
    Os::Generic::PriorityMemQueue::configure(qCfgs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(101, Fw::String("MaxPriQ"), 10, 128));

    // Enable all priorities
    for (FwQueuePriorityType p = 0; p < Os::Generic::Queue::MAX_PRIORITIES; ++p) {
        queue.m_handle.enablePriority(p);
    }

    U8 buf[128];

    // Test valid edge: MAX_PRIORITIES - 1 (highest valid priority)
    buf[0] = 0xAA;
    auto statusValid =
        queue.send(buf, 1, Os::Generic::Queue::MAX_PRIORITIES - 1, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(Os::QueueInterface::Status::OP_OK, statusValid)
        << "MAX_PRIORITIES-1 (" << (Os::Generic::Queue::MAX_PRIORITIES - 1) << ") should be valid";

    // Test invalid edge: MAX_PRIORITIES (first invalid priority)
    buf[0] = 0xBB;
    auto statusInvalid =
        queue.send(buf, 1, Os::Generic::Queue::MAX_PRIORITIES, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(Os::QueueInterface::Status::INVALID_PRIORITY, statusInvalid)
        << "MAX_PRIORITIES (" << Os::Generic::Queue::MAX_PRIORITIES << ") should be invalid";

    // Test well beyond: MAX_PRIORITIES + 100
    buf[0] = 0xCC;
    auto statusBeyond =
        queue.send(buf, 1, Os::Generic::Queue::MAX_PRIORITIES + 100, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(Os::QueueInterface::Status::INVALID_PRIORITY, statusBeyond) << "MAX_PRIORITIES+100 should be invalid";

    // Verify only the valid message was enqueued
    U8 recvBuf[128];
    FwSizeType size;
    FwQueuePriorityType priority;
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK,
              queue.receive(recvBuf, 128, Os::QueueInterface::BlockingType::NONBLOCKING, size, priority));
    EXPECT_EQ(0xAA, recvBuf[0]) << "Should receive the MAX_PRIORITIES-1 message";
    EXPECT_EQ(Os::Generic::Queue::MAX_PRIORITIES - 1, priority);

    // Queue should now be empty (invalid sends rejected)
    EXPECT_EQ(Os::QueueInterface::Status::EMPTY,
              queue.receive(recvBuf, 128, Os::QueueInterface::BlockingType::NONBLOCKING, size, priority));

    queue.teardown();
    Os::Generic::PriorityMemQueue::resetConfig();
}
