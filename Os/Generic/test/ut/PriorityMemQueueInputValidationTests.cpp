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
// Test: Invalid Priority
// ========================================================================
TEST_F(PriorityMemQueueInputValidation, InvalidPrioritySend) {
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("TestQueue");

    // Create queue with default priority
    Os::QueueInterface::Status status = queue.create(1, name, 10, 128);
    ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Try to send to an invalid priority (>= MAX_PRIORITIES)
    U8 buffer[128] = {0};
    FwQueuePriorityType invalidPriority = Os::Generic::Queue::MAX_PRIORITIES;

    status = queue.send(buffer, 10, invalidPriority, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::INVALID_PRIORITY);

    // Try with priority > MAX_PRIORITIES
    invalidPriority = Os::Generic::Queue::MAX_PRIORITIES + 1;
    status = queue.send(buffer, 10, invalidPriority, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::INVALID_PRIORITY);

    queue.teardown();
}

// ========================================================================
// Test: Valid Priority Min/Max Values
// ========================================================================
TEST_F(PriorityMemQueueInputValidation, ValidPriorityBoundaries) {
    Os::Generic::PriorityMemQueue queue;

    // Configure all priorities
    Os::Generic::PriorityMemQueue::QueuePriorityConfig priorityConfigs[Os::Generic::Queue::MAX_PRIORITIES];
    for (FwSizeType i = 0; i < Os::Generic::Queue::MAX_PRIORITIES; ++i) {
        priorityConfigs[i].priority = static_cast<FwQueuePriorityType>(i);
        priorityConfigs[i].maxMsgSize = 128;
        priorityConfigs[i].numMsgs = 5;
    }

    Os::Generic::PriorityMemQueue::QueueConfig queueConfig = {100, Os::Generic::Queue::MAX_PRIORITIES, priorityConfigs};

    Os::Generic::PriorityMemQueue::QueueConfig configs[] = {queueConfig};
    Os::Generic::PriorityMemQueue::configure(configs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    Fw::String name("TestQueue");
    Os::QueueInterface::Status status = queue.create(100, name, 10, 128);
    ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);

    U8 sendBuffer[128];
    U8 recvBuffer[128];
    FwSizeType actualSize;
    FwQueuePriorityType recvPriority;

    // Test minimum valid priority (0)
    sendBuffer[0] = 0xAA;
    status = queue.send(sendBuffer, 1, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);

    status = queue.receive(recvBuffer, sizeof(recvBuffer), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize,
                           recvPriority);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);
    EXPECT_EQ(recvBuffer[0], 0xAA);

    // Test maximum valid priority (MAX_PRIORITIES - 1)
    FwQueuePriorityType maxPriority = Os::Generic::Queue::MAX_PRIORITIES - 1;
    sendBuffer[0] = 0xBB;
    status = queue.send(sendBuffer, 1, maxPriority, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);

    status = queue.receive(recvBuffer, sizeof(recvBuffer), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize,
                           recvPriority);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);
    EXPECT_EQ(recvBuffer[0], 0xBB);
    EXPECT_EQ(recvPriority, maxPriority);

    queue.teardown();
}

// ========================================================================
// Test: Uninitialized Queue Send
// ========================================================================
TEST_F(PriorityMemQueueInputValidation, UninitializedQueueSend) {
    Os::Generic::PriorityMemQueue queue;

    // Try to send without creating queue
    U8 buffer[128] = {0};
    Os::QueueInterface::Status status =
        queue.send(buffer, 10, Os::Generic::Queue::DEFAULT_PRIORITY, Os::QueueInterface::BlockingType::NONBLOCKING);

    EXPECT_EQ(status, Os::QueueInterface::Status::UNINITIALIZED);
}

// ========================================================================
// Test: Uninitialized Queue Receive
// ========================================================================
TEST_F(PriorityMemQueueInputValidation, UninitializedQueueReceive) {
    Os::Generic::PriorityMemQueue queue;

    // Try to receive without creating queue
    U8 buffer[128];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    Os::QueueInterface::Status status =
        queue.receive(buffer, sizeof(buffer), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);

    EXPECT_EQ(status, Os::QueueInterface::Status::UNINITIALIZED);
}

// ========================================================================
// Test: isPriorityEnabled() indirectly via enable/disable behavior
// ========================================================================
TEST_F(PriorityMemQueueInputValidation, IsPriorityEnabledIndirect) {
    Os::Generic::PriorityMemQueue queue;

    // Configure multiple priorities
    Os::Generic::PriorityMemQueue::QueuePriorityConfig priorityConfigs[] = {{0, 128, 10}, {1, 128, 10}, {2, 128, 10}};

    Os::Generic::PriorityMemQueue::QueueConfig queueConfig = {150, 3, priorityConfigs};

    Os::Generic::PriorityMemQueue::QueueConfig configs[] = {queueConfig};
    Os::Generic::PriorityMemQueue::configure(configs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    Fw::String name("TestQueue");
    Os::QueueInterface::Status status = queue.create(150, name, 10, 128);
    ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);

    U8 buffer[128] = {0xAA};
    U8 recvBuffer[128];
    FwSizeType actualSize;
    FwQueuePriorityType recvPriority;

    // Send to priority 1 (enabled by config)
    status = queue.send(buffer, 1, 1, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Disable priority 1
    auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(queue.getHandle());
    handle->disablePriority(1);

    // Send to priority 0 (still enabled)
    buffer[0] = 0xBB;
    status = queue.send(buffer, 1, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Receive should get priority 0 message (priority 1 is disabled)
    status = queue.receive(recvBuffer, sizeof(recvBuffer), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize,
                           recvPriority);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);
    EXPECT_EQ(recvPriority, static_cast<FwQueuePriorityType>(0));
    EXPECT_EQ(recvBuffer[0], 0xBB);

    // Re-enable priority 1
    handle->enablePriority(1);

    // Now the priority 1 message should be available
    status = queue.receive(recvBuffer, sizeof(recvBuffer), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize,
                           recvPriority);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);
    EXPECT_EQ(recvPriority, static_cast<FwQueuePriorityType>(1));
    EXPECT_EQ(recvBuffer[0], 0xAA);

    queue.teardown();
}

// ========================================================================
// Test: Required Priority Sizing Mode
// ========================================================================
TEST_F(PriorityMemQueueInputValidation, RequiredPrioritySizingMode) {
    // Configure with required priority sizing
    Os::Generic::PriorityMemQueue::QueuePriorityConfig priorityConfigs[] = {
        {0, 128, 10}  // Only priority 0 configured
    };

    Os::Generic::PriorityMemQueue::QueueConfig queueConfig = {200, 1, priorityConfigs};

    Os::Generic::PriorityMemQueue::QueueConfig configs[] = {queueConfig};
    Os::Generic::PriorityMemQueue::configure(
        configs, 1, true, Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);  // required = true

    Os::Generic::PriorityMemQueue queue;
    Fw::String name("TestQueue");
    Os::QueueInterface::Status status = queue.create(200, name, 10, 128);
    ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Sending to configured priority should work
    U8 buffer[128] = {1, 2, 3};
    status = queue.send(buffer, 3, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Note: Sending to unconfigured priority with required=true triggers FW_ASSERT
    // which will abort the test. We verify the non-required mode as a contrast:

    queue.teardown();

    // Test non-required mode (should fall back to default priority)
    PriorityMemQueueTestHelper::resetConfig();
    Os::Generic::PriorityMemQueue::configure(
        configs, 1, false, Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);  // required = false

    Os::Generic::PriorityMemQueue queue2;
    Fw::String name2("TestQueue2");
    status = queue2.create(200, name2, 10, 128);
    ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Sending to unconfigured priority should fall back to default (with warning)
    status = queue2.send(buffer, 3, 1, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);  // Falls back to default priority

    queue2.teardown();
}

// ========================================================================
// Test: Verify Valid Argument Min/Max Values
// ========================================================================
TEST_F(PriorityMemQueueInputValidation, ValidMessageSizeBoundaries) {
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("TestQueue");

    // Create queue with max message size
    const FwSizeType maxMsgSize = 512;
    Os::QueueInterface::Status status = queue.create(1, name, 10, maxMsgSize);
    ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Test minimum message size (1 byte)
    U8 minBuffer[1] = {0xAA};
    status =
        queue.send(minBuffer, 1, Os::Generic::Queue::DEFAULT_PRIORITY, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Test maximum message size
    U8* maxBuffer = new U8[maxMsgSize];
    for (FwSizeType i = 0; i < maxMsgSize; ++i) {
        maxBuffer[i] = static_cast<U8>(i & 0xFF);
    }
    status = queue.send(maxBuffer, maxMsgSize, Os::Generic::Queue::DEFAULT_PRIORITY,
                        Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Receive and verify
    U8 recvBuffer[maxMsgSize];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    // Receive min size message
    status = queue.receive(recvBuffer, maxMsgSize, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);
    EXPECT_EQ(actualSize, static_cast<FwSizeType>(1));
    EXPECT_EQ(recvBuffer[0], 0xAA);

    // Receive max size message
    status = queue.receive(recvBuffer, maxMsgSize, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);
    EXPECT_EQ(actualSize, maxMsgSize);
    for (FwSizeType i = 0; i < maxMsgSize; ++i) {
        EXPECT_EQ(recvBuffer[i], static_cast<U8>(i & 0xFF));
    }

    delete[] maxBuffer;
    queue.teardown();
}

// ========================================================================
// Test: Valid Queue Depth Boundaries
// ========================================================================
TEST_F(PriorityMemQueueInputValidation, ValidQueueDepthBoundaries) {
    // Test with depth of 1
    Os::Generic::PriorityMemQueue queue1;
    Fw::String name1("TestQueue1");
    Os::QueueInterface::Status status = queue1.create(1, name1, 1, 128);
    ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);

    U8 buffer[128] = {0x11};
    status =
        queue1.send(buffer, 1, Os::Generic::Queue::DEFAULT_PRIORITY, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Queue should be full now
    buffer[0] = 0x22;
    status =
        queue1.send(buffer, 1, Os::Generic::Queue::DEFAULT_PRIORITY, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::FULL);

    queue1.teardown();

    // Test with larger depth
    Os::Generic::PriorityMemQueue queue2;
    Fw::String name2("TestQueue2");
    const FwSizeType largeDepth = 100;
    status = queue2.create(2, name2, largeDepth, 128);
    ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);

    // Fill the queue to depth
    for (FwSizeType i = 0; i < largeDepth; ++i) {
        buffer[0] = static_cast<U8>(i);
        status =
            queue2.send(buffer, 1, Os::Generic::Queue::DEFAULT_PRIORITY, Os::QueueInterface::BlockingType::NONBLOCKING);
        EXPECT_EQ(status, Os::QueueInterface::Status::OP_OK);
    }

    // Should be full now
    status =
        queue2.send(buffer, 1, Os::Generic::Queue::DEFAULT_PRIORITY, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(status, Os::QueueInterface::Status::FULL);

    queue2.teardown();
}
