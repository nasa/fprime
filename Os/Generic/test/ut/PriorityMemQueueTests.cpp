// ======================================================================
// \title  Os/Generic/test/ut/PriorityMemQueueTests.cpp
// \author B. Duckett
// \brief  cpp file for tests for PriorityMemQueue implementation
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#include <gtest/gtest.h>
#include <limits>
#include "Fw/Time/TimeInterval.hpp"
#include "Os/Generic/PriorityMemQueue.hpp"
#include "Os/Task.hpp"
#include "Os/test/ConcurrentRule.hpp"
#include "STest/Random/Random.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

// Constants for testing
const U32 QUEUE_DEPTH = 10;
const U32 MESSAGE_SIZE = 128;
const U32 MAX_MESSAGES = 512;  // Max capacity for shadow queue (will be limited by per-priority config)
const U32 RANDOM_BOUND = 200;  // Reduced from 1000 to keep test time under 10 seconds
const U32 MAX_TEST_MESSAGE_ID = 1000000;
const FwEnumStoreType QUEUE_ID = 42;

// Test configuration
Os::Generic::PriorityMemQueue::QueuePriorityConfig priorityConfigs[3] = {
    {0, MESSAGE_SIZE, 128},  // Priority 0 - maxMsgSize matches MESSAGE_SIZE for random tests
    {1, MESSAGE_SIZE, 128},  // Priority 1
    {2, MESSAGE_SIZE, 128}   // Priority 2
};

Os::Generic::PriorityMemQueue::QueueConfig queueConfig = {
    QUEUE_ID,
    FW_NUM_ARRAY_ELEMENTS(priorityConfigs),  // numPriorities derived from array size
    &priorityConfigs[0]};

Os::Generic::PriorityMemQueue::QueueConfig configs[] = {queueConfig};

// Test helper to access private members of PriorityMemQueue
class PriorityMemQueueTestHelper {
  public:
    // Reset the configuration state for testing
    static void resetConfig() { Os::Generic::PriorityMemQueue::resetConfig(); }
};

// Forward declarations
namespace Ref {
namespace Test {
namespace PriorityMemQueue {

// Message structure for testing
struct QueueMessage {
    U8 data[MESSAGE_SIZE];
    FwSizeType size;
    FwQueuePriorityType priority;
    U32 id;

    QueueMessage() : size(0), priority(0), id(0) { memset(data, 0, sizeof(data)); }

    void randomize() {
        size = STest::Random::lowerUpper(1, MESSAGE_SIZE);
        // Randomly select from configured priorities 0-2 to exercise multi-priority behavior
        // in random testing. All three priorities are configured and enabled by EnablePriority rule.
        priority = static_cast<FwQueuePriorityType>(STest::Random::lowerUpper(0, 2));
        id = STest::Random::lowerUpper(0, MAX_TEST_MESSAGE_ID);

        for (FwSizeType i = 0; i < size; ++i) {
            data[i] = static_cast<U8>(STest::Random::lowerUpper(0, std::numeric_limits<U8>::max()));
        }
    }

    bool operator==(const QueueMessage& other) const {
        if (size != other.size || priority != other.priority || id != other.id) {
            return false;
        }

        for (FwSizeType i = 0; i < size; ++i) {
            if (data[i] != other.data[i]) {
                return false;
            }
        }

        return true;
    }
};

// Tester class for PriorityMemQueue
class Tester {
  public:
    Tester() : m_created(false) {}

    // Shadow model for verification
    struct ShadowQueue {
        bool created;
        QueueMessage messages[MAX_MESSAGES];
        U32 messageCount;
        U32 highWaterMark;
        bool priorityEnabled[Os::Generic::Queue::MAX_PRIORITIES];
        U32 priorityDepth[Os::Generic::Queue::MAX_PRIORITIES];              // Per-priority queue depth
        FwSizeType priorityMaxMsgSize[Os::Generic::Queue::MAX_PRIORITIES];  // Per-priority max message size
        U32 priorityMessageCount[Os::Generic::Queue::MAX_PRIORITIES];       // Messages in each priority queue
        U32 priorityHighWaterMark[Os::Generic::Queue::MAX_PRIORITIES];      // Per-priority high water marks

        ShadowQueue() : created(false), messageCount(0), highWaterMark(0) {
            for (U32 i = 0; i < Os::Generic::Queue::MAX_PRIORITIES; ++i) {
                priorityEnabled[i] = false;
                priorityDepth[i] = 0;
                priorityMaxMsgSize[i] = 0;
                priorityMessageCount[i] = 0;
                priorityHighWaterMark[i] = 0;
            }
            priorityEnabled[0] = true;  // Priority 0 is enabled by default
        }

        void config(const Os::Generic::PriorityMemQueue::QueuePriorityConfig* configs, U8 numPriorities) {
            // Configure per-priority limits
            for (U8 i = 0; i < numPriorities; ++i) {
                FwQueuePriorityType priority = configs[i].priority;
                priorityMaxMsgSize[priority] = configs[i].maxMsgSize;
                priorityDepth[priority] = static_cast<U32>(configs[i].numMsgs);
            }
        }

        void create() {
            created = true;
            messageCount = 0;
            highWaterMark = 0;
            for (U32 i = 0; i < Os::Generic::Queue::MAX_PRIORITIES; ++i) {
                priorityHighWaterMark[i] = 0;
            }
        }

        bool isEmpty() const { return messageCount == 0; }

        bool isFull() const {
            // With multi-priority queues, "full" means all enabled priorities are full.
            // This ensures FillQueue fills all enabled priorities before SendFull tries.
            // If ANY enabled priority has space, the queue is not full.
            for (FwQueuePriorityType p = 0; p < 3; ++p) {
                if (priorityEnabled[p] && priorityDepth[p] > 0) {
                    if (priorityMessageCount[p] < priorityDepth[p]) {
                        return false;  // This priority has space
                    }
                }
            }
            // All enabled priorities are full
            return true;
        }

        bool isPriorityFull(FwQueuePriorityType priority) const {
            if (priority >= Os::Generic::Queue::MAX_PRIORITIES) {
                return true;
            }
            return priorityMessageCount[priority] >= priorityDepth[priority];
        }

        Os::QueueInterface::Status send(const QueueMessage& msg) {
            if (!created) {
                return Os::QueueInterface::Status::UNINITIALIZED;
            }

            // Model the actual priority resolution logic: if priority not configured (no queue exists),
            // fall back to priority 0 (matches requirePrioritySizing=false behavior).
            // NOTE: Enable/disable state does NOT affect send, only receive.
            FwQueuePriorityType effectivePriority = msg.priority;
            if (priorityDepth[msg.priority] == 0) {
                effectivePriority = Os::Generic::Queue::DEFAULT_PRIORITY;
            }

            // Check per-priority queue depth
            if (isPriorityFull(effectivePriority)) {
                return Os::QueueInterface::Status::FULL;
            }

            // Check message size
            if (msg.size > priorityMaxMsgSize[effectivePriority]) {
                return Os::QueueInterface::Status::SIZE_MISMATCH;
            }

            // Add message to the queue preserving original priority (matches actual implementation)
            messages[messageCount++] = msg;
            priorityMessageCount[effectivePriority]++;

            // Update per-priority high water mark
            if (priorityMessageCount[effectivePriority] > priorityHighWaterMark[effectivePriority]) {
                priorityHighWaterMark[effectivePriority] = priorityMessageCount[effectivePriority];
            }

            // Update total high water mark for legacy tracking
            if (messageCount > highWaterMark) {
                highWaterMark = messageCount;
            }

            return Os::QueueInterface::Status::OP_OK;
        }

        Os::QueueInterface::Status receive(QueueMessage& msg) {
            if (!created) {
                return Os::QueueInterface::Status::UNINITIALIZED;
            }

            if (isEmpty()) {
                return Os::QueueInterface::Status::EMPTY;
            }

            // Find the highest priority message (higher number = higher priority)
            U32 highestPriorityIndex = 0;
            FwQueuePriorityType highestPriority = 0;  // Start from lowest priority

            for (U32 i = 0; i < messageCount; ++i) {
                if (priorityEnabled[messages[i].priority] && messages[i].priority > highestPriority) {
                    highestPriority = messages[i].priority;
                    highestPriorityIndex = i;
                }
            }

            // Copy the message
            msg = messages[highestPriorityIndex];

            // Update per-priority count
            if (msg.priority < Os::Generic::Queue::MAX_PRIORITIES) {
                priorityMessageCount[msg.priority]--;
            }

            // Remove the message from the queue
            for (U32 i = highestPriorityIndex; i < messageCount - 1; ++i) {
                messages[i] = messages[i + 1];
            }

            messageCount--;

            return Os::QueueInterface::Status::OP_OK;
        }

        void enablePriority(FwQueuePriorityType priority) { priorityEnabled[priority] = true; }

        void disablePriority(FwQueuePriorityType priority) { priorityEnabled[priority] = false; }

        bool isPriorityEnabled(FwQueuePriorityType priority) const { return priorityEnabled[priority]; }
    };

    // Actual queue under test
    Os::Generic::PriorityMemQueue m_queue;

    // Shadow queue for verification
    ShadowQueue m_shadow;

    // State tracking
    bool m_created;

    // Create the queue
    Os::QueueInterface::Status create() {
        // Reset the configuration state first
        PriorityMemQueueTestHelper::resetConfig();

        // Configure the queue system
        Os::Generic::PriorityMemQueue::configure(configs, 1, true,
                                                 Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

        // Configure the shadow queue to match
        m_shadow.config(priorityConfigs, 3);

        Fw::String name("TestQueue");
        Os::QueueInterface::Status status = m_queue.create(QUEUE_ID, name, QUEUE_DEPTH, MESSAGE_SIZE);

        if (status == Os::QueueInterface::Status::OP_OK) {
            m_shadow.create();
            m_created = true;
        }

        return status;
    }

    // Teardown the queue
    void teardown() {
        m_queue.teardown();
        m_created = false;
        m_shadow = ShadowQueue();
    }

    // Send a message to the queue
    Os::QueueInterface::Status send(const QueueMessage& msg, Os::QueueInterface::BlockingType blockType) {
        Os::QueueInterface::Status status = m_queue.send(msg.data, msg.size, msg.priority, blockType);

        // Update shadow model for non-blocking sends (status immediately known).
        // For blocking sends, only update on success since failures shouldn't occur
        // (blocking waits for space). This maintains shadow/actual queue synchronization.
        if (blockType == Os::QueueInterface::BlockingType::NONBLOCKING || status == Os::QueueInterface::Status::OP_OK) {
            Os::QueueInterface::Status shadowStatus = m_shadow.send(msg);

            // Verify shadow status matches actual status
            if (status == Os::QueueInterface::Status::OP_OK) {
                EXPECT_EQ(shadowStatus, status);
            }
        }

        return status;
    }

    // Receive a message from the queue
    Os::QueueInterface::Status receive(QueueMessage& msg, Os::QueueInterface::BlockingType blockType) {
        FwSizeType actualSize;
        FwQueuePriorityType priority;

        Os::QueueInterface::Status status = m_queue.receive(msg.data, MESSAGE_SIZE, blockType, actualSize, priority);

        if (status == Os::QueueInterface::Status::OP_OK) {
            msg.size = actualSize;
            msg.priority = priority;

            // Receive from shadow queue for verification
            QueueMessage shadowMsg;
            Os::QueueInterface::Status shadowStatus = m_shadow.receive(shadowMsg);

            EXPECT_EQ(Os::QueueInterface::Status::OP_OK, shadowStatus);
            EXPECT_EQ(shadowMsg.size, msg.size);
            EXPECT_EQ(shadowMsg.priority, msg.priority);

            // Verify data
            for (FwSizeType i = 0; i < msg.size; ++i) {
                EXPECT_EQ(shadowMsg.data[i], msg.data[i]);
            }
        }

        return status;
    }

    // Enable a priority
    void enablePriority(FwQueuePriorityType priority) {
        auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(m_queue.getHandle());
        handle->enablePriority(priority);
        m_shadow.enablePriority(priority);
    }

    // Disable a priority
    void disablePriority(FwQueuePriorityType priority) {
        auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(m_queue.getHandle());
        handle->disablePriority(priority);
        m_shadow.disablePriority(priority);
    }

    // Verify queue state
    void verify() const {
        EXPECT_EQ(m_shadow.messageCount, m_queue.getMessagesAvailable());

        // Compute max of per-priority high water marks to match implementation
        U32 maxHwm = 0;
        for (U32 i = 0; i < Os::Generic::Queue::MAX_PRIORITIES; ++i) {
            if (m_shadow.priorityHighWaterMark[i] > maxHwm) {
                maxHwm = m_shadow.priorityHighWaterMark[i];
            }
        }
        EXPECT_EQ(maxHwm, m_queue.getMessageHighWaterMark());
    }

    // Check if the queue is created
    bool isCreated() const { return m_created; }

    // Check if the queue is empty
    bool isEmpty() const { return m_shadow.isEmpty(); }

    // Check if the queue is full
    bool isFull() const { return m_shadow.isFull(); }

    // Check if a priority is enabled
    bool isPriorityEnabled(FwQueuePriorityType priority) const {
        if (priority >= Os::Generic::Queue::MAX_PRIORITIES) {
            return false;
        }
        return m_shadow.priorityEnabled[priority];
    }

    // Generate a random message
    QueueMessage generateRandomMessage() const {
        QueueMessage msg;
        msg.randomize();
        return msg;
    }

// Rules for testing
#include "PriorityMemQueueRules.hpp"
};

}  // namespace PriorityMemQueue
}  // namespace Test
}  // namespace Ref

// ======================================================================
// Configuration Tests
// ======================================================================

// Test configuration with single queue and single priority
TEST(PriorityMemQueueConfig, SingleQueueSinglePriority) {
    printf("\n=== TEST: SingleQueueSinglePriority ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Create a config with 1 queue, 1 priority
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfig = {0, 5, 64};
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig = {100, 1, &testPriorityConfig};
    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig};

    // Configure the system
    Os::Generic::PriorityMemQueue::configure(testConfigs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create a queue using this configuration
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("TestQueue1");
    Os::QueueInterface::Status status = queue.create(100, name, 10, 128);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Test send/receive
    U8 sendData[64] = {1, 2, 3};
    status = queue.send(sendData, 3, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(3, actualSize);
    ASSERT_EQ(0, priority);

    queue.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Test configuration with multiple queues
TEST(PriorityMemQueueConfig, MultipleQueues) {
    printf("\n=== TEST: MultipleQueues ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Create configs for 2 different queue instances
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfig1[] = {{0, 5, 64}};
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig1 = {101, 1, testPriorityConfig1};

    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfig2[] = {{0, 10, 128}};
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig2 = {102, 1, testPriorityConfig2};

    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig1, testQueueConfig2};

    // Configure the system with 2 queue configs
    Os::Generic::PriorityMemQueue::configure(testConfigs, 2, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create both queues
    Os::Generic::PriorityMemQueue queue1, queue2;
    Fw::String name1("Queue1");
    Fw::String name2("Queue2");

    Os::QueueInterface::Status status1 = queue1.create(101, name1, 10, 128);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status1);

    Os::QueueInterface::Status status2 = queue2.create(102, name2, 10, 128);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status2);

    // Test both queues independently
    U8 sendData1[64] = {1};
    status1 = queue1.send(sendData1, 1, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status1);

    U8 sendData2[128] = {2};
    status2 = queue2.send(sendData2, 1, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status2);

    queue1.teardown();
    queue2.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Test configuration with many priorities (> 3)
TEST(PriorityMemQueueConfig, ManyPriorities) {
    printf("\n=== TEST: ManyPriorities ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Create a config with 5 priorities with different max message sizes
    const FwQueuePriorityType NUM_QUEUE_CONFIGS = 5;
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfigs[5] = {
        {0, 5, 64}, {1, 6, 32}, {2, 7, 48}, {3, 8, 64}, {4, 10, 32}};
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig = {103, NUM_QUEUE_CONFIGS, testPriorityConfigs};
    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig};

    // Configure the system
    Os::Generic::PriorityMemQueue::configure(testConfigs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create a queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("ManyPrioritiesQueue");
    Os::QueueInterface::Status status = queue.create(103, name, 10, 128);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Verify initial m_nonEmptyMask is 0
    auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(queue.getHandle());
    printf("--- Initial state: m_nonEmptyMask = 0x%x (expected 0x0) ---\n", handle->m_nonEmptyMask.load());
    ASSERT_EQ(0U, handle->m_nonEmptyMask.load());

    // Test 1: Send max size messages for each priority and verify success
    // Also verify m_nonEmptyMask bits are set correctly
    printf("--- Test 1: Send max size messages and verify m_nonEmptyMask tracking ---\n");
    for (FwQueuePriorityType p = 0; p < NUM_QUEUE_CONFIGS; ++p) {
        handle->enablePriority(p);
        U8 data[10];  // Large enough for any priority
        FwSizeType maxMsgSize = testPriorityConfigs[p].maxMsgSize;
        // Fill with pattern based on priority
        for (FwSizeType i = 0; i < maxMsgSize; ++i) {
            data[i] = static_cast<U8>(p * 10 + i);
        }
        status = queue.send(data, maxMsgSize, p, Os::QueueInterface::BlockingType::NONBLOCKING);
        printf("Priority %d: Sent %zu bytes (max=%zu) - status=%d\n", p, maxMsgSize, maxMsgSize, status);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

        // Verify m_nonEmptyMask bit is set for this priority
        U32 expectedMask = (1U << (p + 1)) - 1;  // All bits from 0 to p should be set
        U32 actualMask = handle->m_nonEmptyMask.load();
        printf("  After send: m_nonEmptyMask = 0x%x (expected 0x%x)\n", actualMask, expectedMask);
        ASSERT_EQ(expectedMask, actualMask) << "Bit for priority " << static_cast<int>(p) << " not set";
    }

    // Test 2: Verify sending oversized messages fails
    printf("--- Test 2: Verify oversized messages fail ---\n");
    for (FwQueuePriorityType p = 0; p < NUM_QUEUE_CONFIGS; ++p) {
        U8 data[11];  // Oversized data
        FwSizeType maxMsgSize = testPriorityConfigs[p].maxMsgSize;
        FwSizeType oversizeLen = maxMsgSize + 1;
        status = queue.send(data, oversizeLen, p, Os::QueueInterface::BlockingType::NONBLOCKING);
        printf("Priority %d: Sent %zu bytes (max=%zu) - status=%d (expected SIZE_MISMATCH=4)\n", p, oversizeLen,
               maxMsgSize, status);
        ASSERT_EQ(Os::QueueInterface::Status::SIZE_MISMATCH, status);
    }

    // Test 3: Receive messages and verify correct size/priority order
    // Also verify m_nonEmptyMask bits are cleared as priorities are emptied
    printf("--- Test 3: Receive and verify m_nonEmptyMask cleared when priority emptied ---\n");
    for (FwQueuePriorityType p = 0; p < NUM_QUEUE_CONFIGS; ++p) {
        FwQueuePriorityType expectedP = static_cast<FwQueuePriorityType>(NUM_QUEUE_CONFIGS - p - 1);
        U8 recvData[64];
        FwSizeType actualSize;
        FwQueuePriorityType priority;
        status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        FwSizeType expectedSize = testPriorityConfigs[expectedP].maxMsgSize;
        printf("Received: priority=%d (expected=%d), size=%zu (expected=%zu)\n", priority, expectedP, actualSize,
               expectedSize);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        ASSERT_EQ(expectedP, priority);
        ASSERT_EQ(expectedSize, actualSize);

        // Verify data pattern
        for (FwSizeType i = 0; i < actualSize; ++i) {
            ASSERT_EQ(static_cast<U8>(expectedP * 10 + i), recvData[i])
                << "Data mismatch at index " << i << " for priority " << static_cast<int>(expectedP);
        }

        // Verify m_nonEmptyMask bit for this priority is cleared (since it's now empty)
        // Expected: all priorities from 0 to (expectedP - 1) still have messages
        U32 expectedMask = (expectedP > 0) ? ((1U << expectedP) - 1) : 0;
        U32 actualMask = handle->m_nonEmptyMask.load();
        printf("  After receive: m_nonEmptyMask = 0x%x (expected 0x%x, priority %d cleared)\n", actualMask,
               expectedMask, expectedP);
        ASSERT_EQ(expectedMask, actualMask)
            << "Bit for priority " << static_cast<int>(expectedP) << " not cleared after emptying";
    }

    // Verify all bits are cleared now
    printf("All priorities emptied: m_nonEmptyMask = 0x%x (expected 0x0)\n", handle->m_nonEmptyMask.load());
    ASSERT_EQ(0U, handle->m_nonEmptyMask.load());

    // Test 4: Verify each priority queue holds exactly its configured depth
    printf("--- Test 4: Verify per-priority queue depth limits ---\n");

    for (FwQueuePriorityType p = 0; p < NUM_QUEUE_CONFIGS; ++p) {
        FwSizeType numMsgs = testPriorityConfigs[p].numMsgs;
        printf("Testing priority %d: depth=%zu\n", p, numMsgs);

        // Fill this priority queue to capacity
        U8 data[10];
        FwSizeType maxMsgSize = testPriorityConfigs[p].maxMsgSize;
        for (FwSizeType i = 0; i < maxMsgSize; ++i) {
            data[i] = static_cast<U8>(p);
        }

        for (FwSizeType i = 0; i < numMsgs; ++i) {
            status = queue.send(data, maxMsgSize, p, Os::QueueInterface::BlockingType::NONBLOCKING);
            ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status)
                << "Failed to send message " << i << " of " << numMsgs << " to priority " << static_cast<int>(p);
        }
        printf("Priority %d: Successfully sent %zu messages\n", p, numMsgs);

        // Try to send one more - should fail with FULL
        status = queue.send(data, maxMsgSize, p, Os::QueueInterface::BlockingType::NONBLOCKING);
        printf("Priority %d: Send beyond capacity - status=%d (expected FULL=8)\n", p, status);
        ASSERT_EQ(Os::QueueInterface::Status::FULL, status)
            << "Priority " << static_cast<int>(p) << " accepted more than " << numMsgs << " messages";

        // Receive all messages
        U8 recvData[64];
        FwSizeType actualSize;
        FwQueuePriorityType priority;
        for (FwSizeType i = 0; i < numMsgs; ++i) {
            status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
            ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status)
                << "Failed to receive message " << i << " from priority " << static_cast<int>(p);
            ASSERT_EQ(p, priority) << "Received wrong priority";
        }
        printf("Priority %d: Successfully received %zu messages\n", p, numMsgs);

        // Try to receive one more - should fail with EMPTY
        status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        printf("Priority %d: Receive beyond available - status=%d (expected EMPTY=2)\n", p, status);
        ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status)
            << "Priority " << static_cast<int>(p) << " returned more than " << numMsgs << " messages";
    }

    queue.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Test multiple reconfigurations
TEST(PriorityMemQueueConfig, MultipleReconfigurations) {
    printf("\n=== TEST: MultipleReconfigurations ===\n");

    // First configuration
    printf("--- Configuration 1 ---\n");
    PriorityMemQueueTestHelper::resetConfig();

    Os::Generic::PriorityMemQueue::QueuePriorityConfig priorityConfig1[] = {{0, 5, 64}};
    Os::Generic::PriorityMemQueue::QueueConfig queueConfig1 = {104, 1, priorityConfig1};
    Os::Generic::PriorityMemQueue::QueueConfig configs1[] = {queueConfig1};

    Os::Generic::PriorityMemQueue::configure(configs1, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    Os::Generic::PriorityMemQueue queue1;
    Fw::String name1("Queue1");
    Os::QueueInterface::Status status = queue1.create(104, name1, 10, 128);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    queue1.teardown();

    // Second configuration with different parameters
    printf("--- Configuration 2 ---\n");
    PriorityMemQueueTestHelper::resetConfig();

    Os::Generic::PriorityMemQueue::QueuePriorityConfig priorityConfig2[] = {{0, 10, 128}, {1, 10, 128}};
    Os::Generic::PriorityMemQueue::QueueConfig queueConfig2 = {105, 2, priorityConfig2};
    Os::Generic::PriorityMemQueue::QueueConfig configs2[] = {queueConfig2};

    Os::Generic::PriorityMemQueue::configure(configs2, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    Os::Generic::PriorityMemQueue queue2;
    Fw::String name2("Queue2");
    status = queue2.create(105, name2, 10, 128);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    queue2.teardown();

    // Third configuration with many queue configs
    printf("--- Configuration 3 (Large scale) ---\n");
    PriorityMemQueueTestHelper::resetConfig();

    // Create 10 different queue configs
    const FwSizeType NUM_QUEUES = 10;
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfigs3[NUM_QUEUES];
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfigs3[NUM_QUEUES];

    for (FwSizeType i = 0; i < NUM_QUEUES; ++i) {
        testPriorityConfigs3[i] = {0, 5, 64};
        testQueueConfigs3[i] = {static_cast<FwEnumStoreType>(200 + i), 1, &testPriorityConfigs3[i]};
    }

    Os::Generic::PriorityMemQueue::configure(testQueueConfigs3, NUM_QUEUES, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create and test a few of them
    Os::Generic::PriorityMemQueue queues[3];
    for (FwSizeType i = 0; i < 3; ++i) {
        Fw::String name("Queue");
        status = queues[i].create(static_cast<FwEnumStoreType>(200 + i), name, 10, 128);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    for (FwSizeType i = 0; i < 3; ++i) {
        queues[i].teardown();
    }

    printf("=== TEST PASSED ===\n\n");
}

// Test configuration with zero queues
TEST(PriorityMemQueueConfig, ZeroQueues) {
    printf("\n=== TEST: ZeroQueues ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Configure with 0 queue configs (not required)
    Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create a queue - should use default configuration
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("DefaultQueue");
    const FwSizeType queueDepth = 5;
    const FwSizeType maxMsgSize = 64;
    Os::QueueInterface::Status status = queue.create(106, name, queueDepth, maxMsgSize);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Test 1: Verify max message size
    printf("--- Test 1: Verify max message size ---\n");

    // Should succeed with max size message
    U8 maxSizeData[64];
    for (FwSizeType i = 0; i < maxMsgSize; ++i) {
        maxSizeData[i] = static_cast<U8>(i);
    }
    status = queue.send(maxSizeData, maxMsgSize, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    printf("Sent %zu bytes (max=%zu) - status=%d (expected OP_OK=0)\n", maxMsgSize, maxMsgSize, status);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Should fail with oversized message
    U8 oversizeData[65];
    status = queue.send(oversizeData, maxMsgSize + 1, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    printf("Sent %zu bytes (max=%zu) - status=%d (expected SIZE_MISMATCH=4)\n", maxMsgSize + 1, maxMsgSize, status);
    ASSERT_EQ(Os::QueueInterface::Status::SIZE_MISMATCH, status);

    // Read the message that was sent earlier
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(maxMsgSize, actualSize);

    // Test 2: Verify exactly queueDepth messages can be sent (no more, no less)
    printf("--- Test 2: Verify queue depth limits ---\n");

    // Fill queue to capacity
    U8 testData[64];
    for (FwSizeType i = 0; i < maxMsgSize; ++i) {
        testData[i] = static_cast<U8>(0xFF);
    }

    for (FwSizeType i = 0; i < queueDepth; ++i) {
        status = queue.send(testData, maxMsgSize, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status) << "Failed to send message " << i << " of " << queueDepth;
    }
    printf("Successfully sent %zu messages to capacity\n", queueDepth);

    // Try to send one more - should fail with FULL
    status = queue.send(testData, maxMsgSize, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    printf("Send beyond capacity - status=%d (expected FULL=8)\n", status);
    ASSERT_EQ(Os::QueueInterface::Status::FULL, status) << "Queue accepted more than " << queueDepth << " messages";

    // Receive all messages
    for (FwSizeType i = 0; i < queueDepth; ++i) {
        status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status) << "Failed to receive message " << i;
        ASSERT_EQ(maxMsgSize, actualSize);
        ASSERT_EQ(0, priority);
    }
    printf("Successfully received %zu messages\n", queueDepth);

    // Try to receive one more - should fail with EMPTY
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    printf("Receive beyond available - status=%d (expected EMPTY=2)\n", status);
    ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status) << "Queue returned more than " << queueDepth << " messages";

    queue.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Helper function to set up a queue with partial priority configuration
// Returns the created queue with priority 0 and 2 configured, priority 1 NOT configured.
// Sends an initial test message to priority 2 to verify the configured priority works
// before the main test begins (establishes baseline functionality).
static Os::Generic::PriorityMemQueue* setupPartialPriorityQueue(FwEnumStoreType queueId,
                                                                bool required,
                                                                const char* queueName) {
    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Configure with specific priorities - only 0 and 2, NOT 1
    static Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfigs[] = {
        {0, 64, 10}, {2, 32, 5}  // Only priority 0 and 2 are configured, priority 1 is NOT
    };
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig = {queueId, 2, testPriorityConfigs};
    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig};

    Os::Generic::PriorityMemQueue::configure(testConfigs, 1, required,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create a queue
    Os::Generic::PriorityMemQueue* queue = new Os::Generic::PriorityMemQueue();
    Fw::String name(queueName);
    Os::QueueInterface::Status status = queue->create(queueId, name, 10, 64);
    EXPECT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Enable priority 2
    auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(queue->getHandle());
    handle->enablePriority(2);

    // Test 1: Sending to configured priority should work
    printf("--- Test 1: Send to configured priority 2 (should succeed) ---\n");
    U8 data1[32];
    for (FwSizeType i = 0; i < 32; ++i) {
        data1[i] = static_cast<U8>(i);
    }
    status = queue->send(data1, 32, 2, Os::QueueInterface::BlockingType::NONBLOCKING);
    printf("Send to priority 2 - status=%d (expected OP_OK=0)\n", status);
    EXPECT_EQ(Os::QueueInterface::Status::OP_OK, status);

    return queue;
}

// Test that non-default priority falls back to priority 0 when required=false
TEST(PriorityMemQueueConfig, RequiredPrioritySizingFallback) {
    printf("\n=== TEST: RequiredPrioritySizingFallback ===\n");

    // Set up queue with required=false (allows fallback to priority 0)
    Os::Generic::PriorityMemQueue* queue = setupPartialPriorityQueue(107, false, "FallbackQueue");

    // Test 2: With required=false, sending to unconfigured priority 1 should fall back to priority 0
    printf("--- Test 2: Send to unconfigured priority 1 (should fall back to priority 0) ---\n");
    U8 data2[48];
    for (FwSizeType i = 0; i < 48; ++i) {
        data2[i] = static_cast<U8>(0xAA);
    }
    Os::QueueInterface::Status status = queue->send(data2, 48, 1, Os::QueueInterface::BlockingType::NONBLOCKING);
    printf("Send to priority 1 - status=%d (expected OP_OK=0, falls back to priority 0)\n", status);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Test 3: Verify messages are received - priority 2 first, then priority 0 (fallback)
    printf("--- Test 3: Verify messages received in correct order ---\n");
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    // Should receive priority 2 message first (higher priority)
    status = queue->receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    printf("Received message 1: size=%zu, priority=%d (expected priority=2)\n", actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(2, priority);
    ASSERT_EQ(32, actualSize);

    // Should receive priority 0 message (fallback from priority 1 request)
    status = queue->receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    printf("Received message 2: size=%zu, priority=%d (expected priority=0)\n", actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(0, priority);  // Should be priority 0 (fell back from priority 1)
    ASSERT_EQ(48, actualSize);

    queue->teardown();
    delete queue;
    printf("=== TEST PASSED ===\n\n");
}

// Test that non-default priority asserts when required=true
TEST(PriorityMemQueueConfig, OptionalPrioritySizingAssertion) {
    printf("\n=== TEST: OptionalPrioritySizingAssertion ===\n");

    // Set up queue with required=true (strict mode - should assert on unsupported priority)
    Os::Generic::PriorityMemQueue* queue = setupPartialPriorityQueue(108, true, "AssertQueue");

    // Test 2: With required=true, sending to unconfigured priority 1 should ASSERT
    printf("--- Test 2: Send to unconfigured priority 1 (should ASSERT) ---\n");
    U8 data2[48];
    for (FwSizeType i = 0; i < 48; ++i) {
        data2[i] = static_cast<U8>(0xFF);
    }
    ASSERT_DEATH_IF_SUPPORTED(queue->send(data2, 48, 1, Os::QueueInterface::BlockingType::NONBLOCKING),
                              "Assert:.*PriorityMemQueue\\.cpp");
    printf("Correctly asserted when sending to unconfigured priority 1 with required=true\n");

    // Clean up - receive the message from priority 2 that was sent in setup
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;
    Os::QueueInterface::Status status =
        queue->receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(2, priority);
    ASSERT_EQ(32, actualSize);

    queue->teardown();
    delete queue;
    printf("=== TEST PASSED ===\n\n");
}

// Test that calling config() twice without reset asserts
TEST(PriorityMemQueueConfig, DoubleConfigAssertion) {
    printf("\n=== TEST: DoubleConfigAssertion ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // First config call - should succeed
    printf("--- Test: First config() call (should succeed) ---\n");
    Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
    printf("First config() call succeeded\n");

    // Second config call without reset - should ASSERT
    printf("--- Test: Second config() call without reset (should ASSERT) ---\n");
    ASSERT_DEATH_IF_SUPPORTED(
        Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                                 Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE),
        "Assert:.*PriorityMemQueue\\.cpp");
    printf("Correctly asserted when calling config() twice without reset\n");

    // Clean up for subsequent tests
    PriorityMemQueueTestHelper::resetConfig();
    printf("=== TEST PASSED ===\n\n");
}

// Test priority enable/disable behavior and m_priorityMask tracking
TEST(PriorityMemQueueConfig, PriorityEnableDisable) {
    printf("\n=== TEST: PriorityEnableDisable ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Configure with 3 priorities
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfigs[] = {{0, 64, 10}, {1, 64, 10}, {2, 64, 10}};
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig = {200, 3, testPriorityConfigs};
    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig};
    Os::Generic::PriorityMemQueue::configure(testConfigs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create a queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("PriorityEnableDisableQueue");
    Os::QueueInterface::Status status = queue.create(200, name, 10, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Cast to PriorityMemQueueHandle to access priority-specific members
    auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(queue.getHandle());

    // Initially all priorities should be enabled (mask = 0b111 = 7)
    printf("--- Test 1: Check initial priority mask ---\n");
    ASSERT_EQ(7U, handle->m_priorityMask.load());
    printf("Initial m_priorityMask = 0x%x (expected 0x7)\n", handle->m_priorityMask.load());

    // Disable priority 1
    printf("--- Test 2: Disable priority 1 ---\n");
    handle->disablePriority(1);
    ASSERT_EQ(5U, handle->m_priorityMask.load());  // 0b101 = 5
    printf("After disabling priority 1: m_priorityMask = 0x%x (expected 0x5)\n", handle->m_priorityMask.load());

    // Send messages to all three priorities
    printf("--- Test 3: Send messages to all priorities ---\n");
    U8 data0[16], data1[16], data2[16];
    for (FwSizeType i = 0; i < 16; ++i) {
        data0[i] = static_cast<U8>(0xA0 + i);
        data1[i] = static_cast<U8>(0xB0 + i);
        data2[i] = static_cast<U8>(0xC0 + i);
    }

    // Check m_nonEmptyMask before sending (should be 0)
    ASSERT_EQ(0U, handle->m_nonEmptyMask.load());
    printf("Before sending: m_nonEmptyMask = 0x%x (expected 0x0)\n", handle->m_nonEmptyMask.load());

    status = queue.send(data0, 16, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    printf("Sent to priority 0: m_nonEmptyMask = 0x%x (expected 0x1)\n", handle->m_nonEmptyMask.load());
    ASSERT_EQ(1U, handle->m_nonEmptyMask.load());  // Bit 0 set

    status = queue.send(data1, 16, 1, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    printf("Sent to priority 1: m_nonEmptyMask = 0x%x (expected 0x3)\n", handle->m_nonEmptyMask.load());
    ASSERT_EQ(3U, handle->m_nonEmptyMask.load());  // Bits 0,1 set

    status = queue.send(data2, 16, 2, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    printf("Sent to priority 2: m_nonEmptyMask = 0x%x (expected 0x7)\n", handle->m_nonEmptyMask.load());
    ASSERT_EQ(7U, handle->m_nonEmptyMask.load());  // Bits 0,1,2 set

    // Receive - should get priority 2 first (highest enabled), not priority 1 (disabled)
    printf("--- Test 4: Receive should skip disabled priority 1 ---\n");
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(2, priority) << "Should receive priority 2, not disabled priority 1";
    ASSERT_EQ(16, actualSize);
    for (FwSizeType i = 0; i < 16; ++i) {
        ASSERT_EQ(data2[i], recvData[i]);
    }
    printf("Received from priority %d (expected 2, skipped disabled priority 1)\n", priority);
    printf("After receiving priority 2: m_nonEmptyMask = 0x%x (expected 0x3)\n", handle->m_nonEmptyMask.load());
    ASSERT_EQ(3U, handle->m_nonEmptyMask.load());  // Bits 0,1 still set

    // Next receive should get priority 0, still skipping priority 1
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(0, priority) << "Should receive priority 0, still skipping disabled priority 1";
    ASSERT_EQ(16, actualSize);
    printf("Received from priority %d (expected 0, still skipping disabled priority 1)\n", priority);
    printf("After receiving priority 0: m_nonEmptyMask = 0x%x (expected 0x2)\n", handle->m_nonEmptyMask.load());
    ASSERT_EQ(2U, handle->m_nonEmptyMask.load());  // Only bit 1 set

    // Queue should now appear empty (priority 1 is disabled)
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status)
        << "Queue should appear empty with only disabled priority 1 having messages";
    printf("Receive returned EMPTY (correct - priority 1 is disabled)\n");

    // Re-enable priority 1
    printf("--- Test 5: Re-enable priority 1 and verify message is now accessible ---\n");
    handle->enablePriority(1);
    ASSERT_EQ(7U, handle->m_priorityMask.load());  // Back to 0b111 = 7
    printf("After re-enabling priority 1: m_priorityMask = 0x%x (expected 0x7)\n", handle->m_priorityMask.load());

    // Now we should be able to receive the message from priority 1
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(1, priority) << "Should now receive priority 1 message";
    ASSERT_EQ(16, actualSize);
    for (FwSizeType i = 0; i < 16; ++i) {
        ASSERT_EQ(data1[i], recvData[i]);
    }
    printf("Successfully received priority 1 message after re-enabling\n");
    printf("After receiving priority 1: m_nonEmptyMask = 0x%x (expected 0x0)\n", handle->m_nonEmptyMask.load());
    ASSERT_EQ(0U, handle->m_nonEmptyMask.load());  // All messages consumed

    // Queue should be truly empty now
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status);
    printf("Queue is now truly empty\n");

    queue.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Test that creating a queue with duplicate ID asserts
TEST(PriorityMemQueueConfig, DuplicateQueueIdAssertion) {
    printf("\n=== TEST: DuplicateQueueIdAssertion ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Configure with a specific queue ID
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfigs[] = {{0, 64, 10}};
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig = {201, 1, testPriorityConfigs};
    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig};
    Os::Generic::PriorityMemQueue::configure(testConfigs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create first queue with ID 201
    printf("--- Test 1: Create first queue with ID 201 (should succeed) ---\n");
    Os::Generic::PriorityMemQueue queue1;
    Fw::String name1("Queue1");
    Os::QueueInterface::Status status = queue1.create(201, name1, 10, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    printf("First queue created successfully\n");

    // Try to create second queue with same ID 201 - should ASSERT
    printf("--- Test 2: Attempt to create second queue with same ID 201 (should ASSERT) ---\n");
    ASSERT_DEATH_IF_SUPPORTED(
        {
            Os::Generic::PriorityMemQueue queue2;
            Fw::String name2("Queue2");
            queue2.create(201, name2, 10, 64);
        },
        "Assert:.*PriorityMemQueue\\.cpp");
    printf("Correctly asserted when trying to create queue with duplicate ID\n");

    // Test 3: Verify that after teardown, the ID can be reused
    printf("--- Test 3: After teardown, ID can be reused ---\n");
    queue1.teardown();
    printf("First queue torn down\n");

    // Now creating a queue with ID 201 should work
    Os::Generic::PriorityMemQueue queue3;
    Fw::String name3("Queue3");
    status = queue3.create(201, name3, 10, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    printf("Successfully reused ID 201 after first queue teardown\n");

    queue3.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// ========================================================================
// Multi-threaded Blocking Behavior Tests
// ========================================================================

// Context for blocking receive test
// NOTE: PriorityMemQueue uses a fast/slow path receive implementation that assumes
// a single-reader thread to eliminate lost-notification race conditions. These tests
// validate the blocking behavior with a single receiver thread.
struct BlockingReceiveContext {
    Os::Generic::PriorityMemQueue* queue;
    bool messageReceived;
    U8 receivedData[64];
    FwSizeType receivedSize;
    FwQueuePriorityType receivedPriority;
    Os::QueueInterface::Status status;
    Os::Mutex mutex;
    bool started;

    BlockingReceiveContext()
        : queue(nullptr),
          messageReceived(false),
          receivedSize(0),
          receivedPriority(0),
          status(Os::QueueInterface::Status::OP_OK),
          started(false) {}
};

// Thread routine that performs blocking receive
static void blockingReceiveTask(void* arg) {
    BlockingReceiveContext* ctx = static_cast<BlockingReceiveContext*>(arg);

    // Signal that we've started
    ctx->mutex.take();
    ctx->started = true;
    ctx->mutex.release();

    printf("[Receiver Thread] Starting blocking receive...\n");

    // This will block until a message is available
    ctx->status =
        ctx->queue->receive(ctx->receivedData, sizeof(ctx->receivedData), Os::QueueInterface::BlockingType::BLOCKING,
                            ctx->receivedSize, ctx->receivedPriority);

    ctx->mutex.take();
    ctx->messageReceived = true;
    ctx->mutex.release();

    printf("[Receiver Thread] Blocking receive completed with status=%d\n", ctx->status);
}

// Test that blocking receive actually blocks and then receives
TEST(PriorityMemQueueBlocking, BlockingReceive) {
    printf("\n=== TEST: BlockingReceive (Multi-threaded) ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Use default configuration (single priority)
    Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create a queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("BlockingQueue");
    Os::QueueInterface::Status status = queue.create(109, name, 5, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Set up context for receiver thread
    BlockingReceiveContext ctx;
    ctx.queue = &queue;

    // Start receiver thread - it will block waiting for a message
    printf("--- Test: Spawn receiver thread that will block on empty queue ---\n");
    Os::Task receiverTask;
    Os::Task::Arguments args(Fw::String("ReceiverTask"), blockingReceiveTask, &ctx, Os::Task::TASK_PRIORITY_DEFAULT,
                             Os::Task::TASK_DEFAULT);

    Os::Task::Status taskStatus = receiverTask.start(args);
    ASSERT_EQ(Os::Task::Status::OP_OK, taskStatus);

    // Wait for receiver to start
    printf("[Main Thread] Waiting for receiver thread to start...\n");
    for (int i = 0; i < 50 && !ctx.started; ++i) {
        Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms
    }
    ASSERT_TRUE(ctx.started);

    // Give receiver time to block
    printf("[Main Thread] Giving receiver time to block...\n");
    Os::Task::delay(Fw::TimeInterval(0, 10000));  // 10ms

    // Verify message hasn't been received yet (thread should be blocked)
    ctx.mutex.take();
    bool receivedBeforeSend = ctx.messageReceived;
    ctx.mutex.release();
    ASSERT_FALSE(receivedBeforeSend) << "Message received before send!";

    // Now send a message - this should unblock the receiver
    printf("[Main Thread] Sending message to unblock receiver...\n");
    U8 testData[32];
    for (FwSizeType i = 0; i < sizeof(testData); ++i) {
        testData[i] = static_cast<U8>(i + 42);
    }

    status = queue.send(testData, sizeof(testData), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    printf("[Main Thread] Message sent, waiting for receiver to complete...\n");

    // Wait for receiver to complete
    Os::Task::Status joinStatus = receiverTask.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus);

    // Verify the message was received correctly
    printf("--- Verifying received message ---\n");
    ASSERT_TRUE(ctx.messageReceived);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, ctx.status);
    ASSERT_EQ(sizeof(testData), ctx.receivedSize);
    ASSERT_EQ(0, ctx.receivedPriority);

    // Verify data content
    for (FwSizeType i = 0; i < sizeof(testData); ++i) {
        ASSERT_EQ(testData[i], ctx.receivedData[i]) << "Data mismatch at index " << i;
    }

    queue.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Context for blocking send test
struct BlockingSendContext {
    Os::Generic::PriorityMemQueue* queue;
    bool messageSent;
    Os::QueueInterface::Status status;
    Os::Mutex mutex;
    bool started;

    BlockingSendContext()
        : queue(nullptr), messageSent(false), status(Os::QueueInterface::Status::OP_OK), started(false) {}
};

// Thread routine that performs blocking send
static void blockingSendTask(void* arg) {
    BlockingSendContext* ctx = static_cast<BlockingSendContext*>(arg);

    // Signal that we've started
    ctx->mutex.take();
    ctx->started = true;
    ctx->mutex.release();

    printf("[Sender Thread] Starting blocking send...\n");

    U8 data[32];
    for (FwSizeType i = 0; i < sizeof(data); ++i) {
        data[i] = static_cast<U8>(0xBB);
    }

    // This will block until space is available
    ctx->status = ctx->queue->send(data, sizeof(data), 0, Os::QueueInterface::BlockingType::BLOCKING);

    ctx->mutex.take();
    ctx->messageSent = true;
    ctx->mutex.release();

    printf("[Sender Thread] Blocking send completed with status=%d\n", ctx->status);
}

// Test that blocking send actually blocks when queue is full
TEST(PriorityMemQueueBlocking, BlockingSend) {
    printf("\n=== TEST: BlockingSend (Multi-threaded) ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Use default configuration (single priority)
    Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create a small queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("BlockingSendQueue");
    Os::QueueInterface::Status status = queue.create(110, name, 2, 64);  // Only 2 messages capacity
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Fill the queue to capacity
    printf("--- Test: Filling queue to capacity ---\n");
    U8 fillData[32];
    for (int i = 0; i < 2; ++i) {
        for (FwSizeType j = 0; j < sizeof(fillData); ++j) {
            fillData[j] = static_cast<U8>(i);
        }
        status = queue.send(fillData, sizeof(fillData), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }
    printf("[Main Thread] Queue is now full\n");

    // Set up context for sender thread
    BlockingSendContext ctx;
    ctx.queue = &queue;

    // Start sender thread - it will block because queue is full
    printf("--- Test: Spawn sender thread that will block on full queue ---\n");
    Os::Task senderTask;
    Os::Task::Arguments args(Fw::String("SenderTask"), blockingSendTask, &ctx, Os::Task::TASK_PRIORITY_DEFAULT,
                             Os::Task::TASK_DEFAULT);

    Os::Task::Status taskStatus = senderTask.start(args);
    ASSERT_EQ(Os::Task::Status::OP_OK, taskStatus);

    // Wait for sender to start
    printf("[Main Thread] Waiting for sender thread to start...\n");
    for (int i = 0; i < 50 && !ctx.started; ++i) {
        Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms
    }
    ASSERT_TRUE(ctx.started);

    // Give sender time to block
    printf("[Main Thread] Giving sender time to block...\n");
    Os::Task::delay(Fw::TimeInterval(0, 10000));  // 10ms

    // Verify message hasn't been sent yet (thread should be blocked)
    ctx.mutex.take();
    bool sentBeforeReceive = ctx.messageSent;
    ctx.mutex.release();
    ASSERT_FALSE(sentBeforeReceive) << "Message sent before space was available!";

    // Now receive a message - this should unblock the sender
    printf("[Main Thread] Receiving message to make space...\n");
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    status =
        queue.receive(recvData, sizeof(recvData), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    printf("[Main Thread] Message received, waiting for sender to complete...\n");

    // Wait for sender to complete
    Os::Task::Status joinStatus = senderTask.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus);

    // Verify the send completed successfully
    printf("--- Verifying send completed ---\n");
    ASSERT_TRUE(ctx.messageSent);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, ctx.status);

    queue.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Test that validates the race condition fix: rapid send after receiver blocks
// This test verifies that the fast/slow path implementation correctly handles
// the scenario where a message is sent very quickly after the receiver starts blocking,
// which could cause a lost notification in the old implementation.
TEST(PriorityMemQueueBlocking, RapidSendAfterBlock) {
    printf("\n=== TEST: RapidSendAfterBlock (Race Condition Validation) ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Use default configuration (single priority)
    Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create a queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("RapidBlockQueue");
    Os::QueueInterface::Status status = queue.create(110, name, 5, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Set up context for receiver thread
    BlockingReceiveContext ctx;
    ctx.queue = &queue;

    // Start receiver thread - it will block waiting for a message
    printf("--- Test: Spawn receiver that blocks, then rapidly send message ---\n");
    Os::Task receiverTask;
    Os::Task::Arguments args(Fw::String("RapidRecvTask"), blockingReceiveTask, &ctx, Os::Task::TASK_PRIORITY_DEFAULT,
                             Os::Task::TASK_DEFAULT);

    Os::Task::Status taskStatus = receiverTask.start(args);
    ASSERT_EQ(Os::Task::Status::OP_OK, taskStatus);

    // Wait for receiver to start
    printf("[Main Thread] Waiting for receiver to start...\n");
    for (int i = 0; i < 50 && !ctx.started; ++i) {
        Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms
    }
    ASSERT_TRUE(ctx.started);

    // Give receiver minimal time to enter blocking state
    // This tight timing increases the likelihood of hitting the race window
    // where notification could be lost in the old implementation
    printf("[Main Thread] Brief delay, then sending message (testing race window)...\n");
    Os::Task::delay(Fw::TimeInterval(0, 5000));  // Only 5ms - very tight timing

    // Send message immediately - in old implementation, this could arrive during
    // the race window between exiting critical section and entering wait
    printf("[Main Thread] Sending message to potentially race with blocking...\n");
    U8 testData[32];
    for (FwSizeType i = 0; i < sizeof(testData); ++i) {
        testData[i] = static_cast<U8>(0xAA);
    }

    status = queue.send(testData, sizeof(testData), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    printf("[Main Thread] Message sent, waiting for receiver...\n");

    // Wait for receiver to complete
    // With the fix, receiver should unblock within a short time
    Os::Task::Status joinStatus = receiverTask.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus);

    // Verify the message was received correctly
    // If this fails, the notification was lost (old bug reproduced)
    printf("--- Verifying message was received (not lost) ---\n");
    ASSERT_TRUE(ctx.messageReceived) << "Message notification was lost!";
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, ctx.status);
    ASSERT_EQ(sizeof(testData), ctx.receivedSize);
    ASSERT_EQ(0, ctx.receivedPriority);

    // Verify data content
    for (FwSizeType i = 0; i < sizeof(testData); ++i) {
        ASSERT_EQ(testData[i], ctx.receivedData[i]) << "Data mismatch at index " << i;
    }

    queue.teardown();
    printf("=== TEST PASSED: No lost notification ===\n\n");
}

// ========================================================================
// Additional Coverage Tests
// ========================================================================

// Test FIFO ordering within a single priority
TEST(PriorityMemQueueOrdering, FIFOWithinPriority) {
    printf("\n=== TEST: FIFOWithinPriority ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Configure single priority
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfigs[] = {{1, 64, 10}};
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig = {300, 1, testPriorityConfigs};
    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig};
    Os::Generic::PriorityMemQueue::configure(testConfigs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("FIFOQueue");
    Os::QueueInterface::Status status = queue.create(300, name, 10, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Enable priority 1
    auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(queue.getHandle());
    handle->enablePriority(1);

    // Send 5 messages with distinct patterns, all at priority 1
    printf("--- Sending 5 messages with distinct patterns ---\n");
    const FwSizeType numMessages = 5;
    U8 sendData[numMessages][16];
    for (FwSizeType i = 0; i < numMessages; ++i) {
        for (FwSizeType j = 0; j < 16; ++j) {
            sendData[i][j] = static_cast<U8>((i * 10) + j);  // Unique pattern per message
        }
        status = queue.send(sendData[i], 16, 1, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        printf("Sent message %zu with pattern starting with %u\n", i, sendData[i][0]);
    }

    // Receive all messages and verify FIFO order
    printf("--- Verifying FIFO receive order ---\n");
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    for (FwSizeType i = 0; i < numMessages; ++i) {
        status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        ASSERT_EQ(1, priority);
        ASSERT_EQ(16, actualSize);

        // Verify data matches expected message in FIFO order
        for (FwSizeType j = 0; j < 16; ++j) {
            ASSERT_EQ(sendData[i][j], recvData[j]) << "FIFO violation: message " << i << " byte " << j;
        }
        printf("Received message %zu with pattern starting with %u (expected %u) - FIFO verified\n", i, recvData[0],
               sendData[i][0]);
    }

    queue.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Test exact size boundaries for each priority
TEST(PriorityMemQueueEdgeCases, SizeMismatchBoundaries) {
    printf("\n=== TEST: SizeMismatchBoundaries ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Configure priorities with different max message sizes
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfigs[] = {
        {0, 32, 10},  // Priority 0: 32 bytes max
        {1, 64, 10},  // Priority 1: 64 bytes max
        {2, 128, 10}  // Priority 2: 128 bytes max
    };
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig = {301, 3, testPriorityConfigs};
    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig};
    Os::Generic::PriorityMemQueue::configure(testConfigs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("BoundaryQueue");
    Os::QueueInterface::Status status = queue.create(301, name, 10, 128);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Enable all priorities
    auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(queue.getHandle());
    handle->enablePriority(1);
    handle->enablePriority(2);

    printf("--- Testing exact size boundaries ---\n");

    // Test each priority
    for (FwQueuePriorityType p = 0; p < 3; ++p) {
        FwSizeType maxSize = testPriorityConfigs[p].maxMsgSize;
        U8* buffer = new U8[maxSize + 1];

        // Fill with test pattern
        for (FwSizeType i = 0; i < maxSize + 1; ++i) {
            buffer[i] = static_cast<U8>(p * 50 + i);
        }

        // Test exact max size (should succeed)
        status = queue.send(buffer, maxSize, p, Os::QueueInterface::BlockingType::NONBLOCKING);
        printf("Priority %d: Send %zu bytes (max=%zu) - status=%d (expected OP_OK=0)\n", p, maxSize, maxSize, status);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

        // Test max size + 1 (should fail with SIZE_MISMATCH)
        status = queue.send(buffer, maxSize + 1, p, Os::QueueInterface::BlockingType::NONBLOCKING);
        printf("Priority %d: Send %zu bytes (max=%zu) - status=%d (expected SIZE_MISMATCH=4)\n", p, maxSize + 1,
               maxSize, status);
        ASSERT_EQ(Os::QueueInterface::Status::SIZE_MISMATCH, status);

        delete[] buffer;

        // Drain the successful message
        U8 recvBuffer[128];
        FwSizeType actualSize;
        FwQueuePriorityType recvPriority;
        status =
            queue.receive(recvBuffer, 128, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, recvPriority);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        ASSERT_EQ(maxSize, actualSize);
    }

    queue.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Test semaphore count tracking
TEST(PriorityMemQueueEdgeCases, SemaphoreCountTracking) {
    printf("\n=== TEST: SemaphoreCountTracking ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Use default configuration
    Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create small queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("SemaphoreQueue");
    const FwSizeType queueDepth = 10;
    Os::QueueInterface::Status status = queue.create(302, name, queueDepth, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    printf("--- Filling queue to capacity and verifying semaphore count ---\n");

    // Send messages to capacity
    U8 testData[32];
    for (FwSizeType i = 0; i < queueDepth; ++i) {
        testData[0] = static_cast<U8>(i);
        status = queue.send(testData, 32, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }
    printf("Sent %zu messages\n", queueDepth);

    // Verify messages available matches queue depth
    FwSizeType available = queue.getMessagesAvailable();
    printf("Messages available: %zu (expected %zu)\n", available, queueDepth);
    ASSERT_EQ(queueDepth, available);

    // Receive all messages
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;
    for (FwSizeType i = 0; i < queueDepth; ++i) {
        status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }
    printf("Received %zu messages\n", queueDepth);

    // Verify queue is empty
    available = queue.getMessagesAvailable();
    printf("Messages available after drain: %zu (expected 0)\n", available);
    ASSERT_EQ(0, available);

    // Try to receive from empty queue (should get EMPTY, not block)
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status);
    printf("Receive from empty queue returned EMPTY - semaphore count correct\n");

    queue.teardown();
    printf("=== TEST PASSED ===\n\n");
}

// Test teardown with messages still pending
TEST(PriorityMemQueueEdgeCases, TeardownWithPendingMessages) {
    printf("\n=== TEST: TeardownWithPendingMessages ===\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Use default configuration
    Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("TeardownQueue");
    Os::QueueInterface::Status status = queue.create(303, name, 10, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    printf("--- Sending messages without receiving ---\n");

    // Send several messages
    U8 testData[32];
    const FwSizeType numMessages = 5;
    for (FwSizeType i = 0; i < numMessages; ++i) {
        testData[0] = static_cast<U8>(i);
        status = queue.send(testData, 32, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    FwSizeType available = queue.getMessagesAvailable();
    printf("Sent %zu messages, %zu still pending\n", numMessages, available);
    ASSERT_EQ(numMessages, available);

    // Teardown without receiving - should not crash or assert
    printf("--- Tearing down with %zu messages still in queue ---\n", available);
    queue.teardown();

    printf("Teardown completed successfully with pending messages\n");
    printf("=== TEST PASSED ===\n\n");
}

// Context for multiple concurrent receivers test
struct MultiReceiverContext {
    Os::Generic::PriorityMemQueue* queue;
    std::atomic<U32> messagesReceived;
    std::atomic<bool> started;
    Os::Mutex mutex;
    bool receivedCorrectly[2];

    MultiReceiverContext() : queue(nullptr), messagesReceived(0), started(false) {
        receivedCorrectly[0] = false;
        receivedCorrectly[1] = false;
    }
};

// Thread routine for concurrent receiver
static void concurrentReceiverTask(void* arg) {
    MultiReceiverContext* ctx = static_cast<MultiReceiverContext*>(arg);

    ctx->mutex.take();
    ctx->started.store(true, std::memory_order_release);
    ctx->mutex.release();

    printf("[Receiver Thread] Starting blocking receive...\n");

    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    Os::QueueInterface::Status status = ctx->queue->receive(
        recvData, sizeof(recvData), Os::QueueInterface::BlockingType::BLOCKING, actualSize, priority);

    if (status == Os::QueueInterface::Status::OP_OK) {
        U32 count = ctx->messagesReceived.fetch_add(1, std::memory_order_acq_rel);
        printf("[Receiver Thread] Received message (count=%u)\n", count + 1);
    }
}

// Test multiple concurrent receivers (documents behavior)
// NOTE: PriorityMemQueue's fast/slow path receive assumes single-reader design for optimal
// priority ordering guarantees. Multiple concurrent receivers work correctly and compete
// for messages, but priority ordering between readers is not guaranteed.
// This test documents that multiple readers can successfully receive messages.
TEST(PriorityMemQueueBlocking, MultipleConcurrentReceivers) {
    printf("\n=== TEST: MultipleConcurrentReceivers (Behavior Test) ===\n");
    printf("NOTE: Testing multiple concurrent receivers competing for messages.\n");
    printf("Priority ordering guarantees require single-reader pattern.\n\n");

    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Use default configuration
    Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Create queue
    Os::Generic::PriorityMemQueue queue;
    Fw::String name("MultiReceiverQueue");
    Os::QueueInterface::Status status = queue.create(304, name, 5, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Set up context for receivers
    MultiReceiverContext ctx;
    ctx.queue = &queue;

    // Start two receiver threads (both will block on empty queue)
    printf("--- Starting two receiver threads on empty queue ---\n");
    Os::Task receiverTask1, receiverTask2;

    Os::Task::Arguments args1(Fw::String("Receiver1"), concurrentReceiverTask, &ctx, Os::Task::TASK_PRIORITY_DEFAULT,
                              Os::Task::TASK_DEFAULT);
    Os::Task::Arguments args2(Fw::String("Receiver2"), concurrentReceiverTask, &ctx, Os::Task::TASK_PRIORITY_DEFAULT,
                              Os::Task::TASK_DEFAULT);

    Os::Task::Status taskStatus1 = receiverTask1.start(args1);
    ASSERT_EQ(Os::Task::Status::OP_OK, taskStatus1);

    Os::Task::Status taskStatus2 = receiverTask2.start(args2);
    ASSERT_EQ(Os::Task::Status::OP_OK, taskStatus2);

    // Wait for both receivers to start
    printf("[Main Thread] Waiting for receivers to start...\n");
    for (int i = 0; i < 50 && !ctx.started.load(std::memory_order_acquire); ++i) {
        Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms (microseconds)
    }
    ASSERT_TRUE(ctx.started.load(std::memory_order_acquire));

    // Give receivers time to block
    Os::Task::delay(Fw::TimeInterval(0, 10000));  // 10ms (microseconds)

    // Send TWO messages - one for each receiver
    // This documents that multiple receivers work, but compete for messages
    printf("[Main Thread] Sending two messages for both receivers...\n");
    U8 testData1[32] = {0xAB};
    status = queue.send(testData1, 32, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    U8 testData2[32] = {0xCD};
    status = queue.send(testData2, 32, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Wait for receivers to complete
    printf("[Main Thread] Waiting for receivers to complete...\n");
    Os::Task::Status joinStatus1 = receiverTask1.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus1);

    Os::Task::Status joinStatus2 = receiverTask2.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus2);

    // Check results - both should have received one message each
    U32 received = ctx.messagesReceived.load(std::memory_order_acquire);
    printf("[Main Thread] Total messages received by both threads: %u\n", received);

    // EXPECTED: 2 messages received (one per thread)
    // Multiple concurrent receivers work but compete for messages from the queue
    EXPECT_EQ(2U, received) << "Multiple concurrent receivers: two sent, " << received
                            << " received (expected 2, one per thread)";

    queue.teardown();
    printf("=== TEST COMPLETE ===\n");
    printf("Multiple concurrent receivers work but compete for messages.\n");
    printf("Single-reader pattern is preferred for priority message ordering guarantees.\n\n");
}

// NOTE: High-water mark accuracy under concurrency is NOT tested.
// The HWM is a diagnostic/guideline metric, not a control path value.
// It uses best-effort atomic updates with a retry limit. Under extreme
// concurrent load, the CAS loop may cap at MAX_CAS_RETRIES (100) and assert.
// Testing this would require carefully orchestrated concurrent stress beyond
// the scope of functional unit tests.

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}

// Basic test for creation and teardown
TEST(PriorityMemQueueBasic, CreateTeardown) {
    Ref::Test::PriorityMemQueue::Tester tester;
    Ref::Test::PriorityMemQueue::Tester::Create create_rule;
    Ref::Test::PriorityMemQueue::Tester::Teardown teardown_rule;

    create_rule.action(tester);
    ASSERT_TRUE(tester.isCreated());
    teardown_rule.action(tester);
    ASSERT_FALSE(tester.isCreated());
}

// Test for basic queue operations
TEST(PriorityMemQueueBasic, SendReceive) {
    Ref::Test::PriorityMemQueue::Tester tester;
    Ref::Test::PriorityMemQueue::Tester::Create create_rule;
    Ref::Test::PriorityMemQueue::Tester::Send send_rule;
    Ref::Test::PriorityMemQueue::Tester::Receive receive_rule;
    Ref::Test::PriorityMemQueue::Tester::Teardown teardown_rule;

    create_rule.action(tester);
    send_rule.action(tester);
    receive_rule.action(tester);
    teardown_rule.action(tester);
}

// Test for priority management
TEST(PriorityMemQueuePriority, EnableDisable) {
    Ref::Test::PriorityMemQueue::Tester tester;
    Ref::Test::PriorityMemQueue::Tester::Create create_rule;
    Ref::Test::PriorityMemQueue::Tester::EnablePriority enable_rule;
    Ref::Test::PriorityMemQueue::Tester::DisablePriority disable_rule;
    Ref::Test::PriorityMemQueue::Tester::Teardown teardown_rule;

    create_rule.action(tester);
    enable_rule.action(tester);
    disable_rule.action(tester);
    teardown_rule.action(tester);
}

// Test for priority order
TEST(PriorityMemQueuePriority, PriorityOrder) {
    Ref::Test::PriorityMemQueue::Tester tester;
    Ref::Test::PriorityMemQueue::Tester::Create create_rule;
    Ref::Test::PriorityMemQueue::Tester::PriorityOrder priority_rule;
    Ref::Test::PriorityMemQueue::Tester::Teardown teardown_rule;

    create_rule.action(tester);
    priority_rule.action(tester);
    teardown_rule.action(tester);
}

// Test for queue full behavior
TEST(PriorityMemQueueEdgeCases, QueueFull) {
    Ref::Test::PriorityMemQueue::Tester tester;
    Ref::Test::PriorityMemQueue::Tester::Create create_rule;
    Ref::Test::PriorityMemQueue::Tester::FillQueue fill_rule;
    Ref::Test::PriorityMemQueue::Tester::SendFull send_full_rule;
    Ref::Test::PriorityMemQueue::Tester::Teardown teardown_rule;

    create_rule.action(tester);
    fill_rule.action(tester);
    send_full_rule.action(tester);
    teardown_rule.action(tester);
}

// Test for queue empty behavior
TEST(PriorityMemQueueEdgeCases, QueueEmpty) {
    Ref::Test::PriorityMemQueue::Tester tester;
    Ref::Test::PriorityMemQueue::Tester::Create create_rule;
    Ref::Test::PriorityMemQueue::Tester::ReceiveEmpty receive_empty_rule;
    Ref::Test::PriorityMemQueue::Tester::Teardown teardown_rule;

    create_rule.action(tester);
    receive_empty_rule.action(tester);
    teardown_rule.action(tester);
}

// Random scenario test
TEST(PriorityMemQueueRandom, RandomOperations) {
    Ref::Test::PriorityMemQueue::Tester tester;
    Ref::Test::PriorityMemQueue::Tester::Create create_rule;
    Ref::Test::PriorityMemQueue::Tester::Send send_rule;
    Ref::Test::PriorityMemQueue::Tester::Receive receive_rule;
    Ref::Test::PriorityMemQueue::Tester::EnablePriority enable_rule;
    Ref::Test::PriorityMemQueue::Tester::DisablePriority disable_rule;
    Ref::Test::PriorityMemQueue::Tester::SendFull send_full_rule;
    Ref::Test::PriorityMemQueue::Tester::ReceiveEmpty receive_empty_rule;
    Ref::Test::PriorityMemQueue::Tester::Teardown teardown_rule;

    // Place rules into a list
    STest::Rule<Ref::Test::PriorityMemQueue::Tester>* rules[] = {&create_rule,        &send_rule,    &receive_rule,
                                                                 &enable_rule,        &disable_rule, &send_full_rule,
                                                                 &receive_empty_rule, &teardown_rule};

    // Create a random scenario
    STest::RandomScenario<Ref::Test::PriorityMemQueue::Tester> random("Random PriorityMemQueue Operations", rules,
                                                                      FW_NUM_ARRAY_ELEMENTS(rules));

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Ref::Test::PriorityMemQueue::Tester> bounded("Bounded Random PriorityMemQueue Operations",
                                                                        random, RANDOM_BOUND);

    // Run the scenario
    const U32 numSteps = bounded.run(tester);
    printf("Ran %u steps.\n", numSteps);
}
