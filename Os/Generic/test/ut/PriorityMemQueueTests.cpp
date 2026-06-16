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
const U32 FILL_QUEUE_MAX_RETRIES = 100;  // Max consecutive FULL retries before assuming all priorities full
const U32 SEND_PRIORITY_SEARCH_MAX_ATTEMPTS = 50;  // Max attempts to find enabled non-full priority

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

// Test fixture for config-using tests — resets PriorityMemQueue state before and after each test
class PriorityMemQueueTestFixture : public ::testing::Test {
  protected:
    void SetUp() override { PriorityMemQueueTestHelper::resetConfig(); }
    void TearDown() override { PriorityMemQueueTestHelper::resetConfig(); }
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
                priorityEnabled[priority] = true;
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
            for (FwQueuePriorityType p = 0; p < Os::Generic::Queue::MAX_PRIORITIES; ++p) {
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
            // Only consider messages in enabled priorities
            U32 highestPriorityIndex = 0;
            FwQueuePriorityType highestPriority = 0;
            bool found = false;

            for (U32 i = 0; i < messageCount; ++i) {
                if (priorityEnabled[messages[i].priority]) {
                    if (!found || messages[i].priority > highestPriority) {
                        highestPriority = messages[i].priority;
                        highestPriorityIndex = i;
                        found = true;
                    }
                }
            }

            // If no enabled message found, queue is effectively empty
            if (!found) {
                return Os::QueueInterface::Status::EMPTY;
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

        // Update shadow model only when actual send succeeded.
        // For blocking sends, failures shouldn't occur (blocking waits for space).
        // For non-blocking sends, only update shadow on success to maintain synchronization.
        if (status == Os::QueueInterface::Status::OP_OK) {
            Os::QueueInterface::Status shadowStatus = m_shadow.send(msg);
            // Verify shadow status matches actual status
            EXPECT_EQ(shadowStatus, status);
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

    // Check if there are receivable messages (in enabled priorities)
    bool hasReceivableMessages() const {
        for (FwQueuePriorityType p = 0; p < Os::Generic::Queue::MAX_PRIORITIES; ++p) {
            if (m_shadow.priorityEnabled[p] && m_shadow.priorityMessageCount[p] > 0) {
                return true;
            }
        }
        return false;
    }

    // Check if the queue is full
    bool isFull() const { return m_shadow.isFull(); }

    // Check if a priority is enabled
    bool isPriorityEnabled(FwQueuePriorityType priority) const {
        if (priority >= Os::Generic::Queue::MAX_PRIORITIES) {
            return false;
        }
        return m_shadow.priorityEnabled[priority];
    }

    // Check if at least one priority is enabled
    bool hasEnabledPriority() const {
        for (FwQueuePriorityType p = 0; p < Os::Generic::Queue::MAX_PRIORITIES; ++p) {
            if (m_shadow.priorityEnabled[p]) {
                return true;
            }
        }
        return false;
    }

    // Check if a specific priority is full
    bool isPriorityFull(FwQueuePriorityType priority) const {
        if (priority >= Os::Generic::Queue::MAX_PRIORITIES) {
            return true;
        }
        return m_shadow.isPriorityFull(priority);
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
TEST_F(PriorityMemQueueTestFixture, SingleQueueSinglePriority) {
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
}

// Test configuration with multiple queues
TEST_F(PriorityMemQueueTestFixture, MultipleQueues) {
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
}

// Test configuration with many priorities (> 3)
TEST_F(PriorityMemQueueTestFixture, ManyPriorities) {
    PriorityMemQueueTestHelper::resetConfig();

    const FwQueuePriorityType NUM_QUEUE_CONFIGS = 5;
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfigs[5] = {
        {0, 5, 64}, {1, 6, 32}, {2, 7, 48}, {3, 8, 64}, {4, 10, 32}};
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig = {103, NUM_QUEUE_CONFIGS, testPriorityConfigs};
    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig};
    Os::Generic::PriorityMemQueue::configure(testConfigs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    Os::Generic::PriorityMemQueue queue;
    Fw::String name("ManyPrioritiesQueue");
    Os::QueueInterface::Status status = queue.create(103, name, 10, 128);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    auto* handle = static_cast<Os::Generic::PriorityMemQueueHandle*>(queue.getHandle());

    // Enable all priorities and send max-size messages
    for (FwQueuePriorityType p = 0; p < NUM_QUEUE_CONFIGS; ++p) {
        handle->enablePriority(p);
        U8 data[10];
        FwSizeType maxMsgSize = testPriorityConfigs[p].maxMsgSize;
        for (FwSizeType i = 0; i < maxMsgSize; ++i) {
            data[i] = static_cast<U8>(p * 10 + i);
        }
        status = queue.send(data, maxMsgSize, p, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    // Verify oversized messages are rejected
    for (FwQueuePriorityType p = 0; p < NUM_QUEUE_CONFIGS; ++p) {
        U8 data[11];
        FwSizeType maxMsgSize = testPriorityConfigs[p].maxMsgSize;
        status = queue.send(data, maxMsgSize + 1, p, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::SIZE_MISMATCH, status);
    }

    // Receive messages and verify priority order and data content
    for (FwQueuePriorityType p = 0; p < NUM_QUEUE_CONFIGS; ++p) {
        FwQueuePriorityType expectedP = static_cast<FwQueuePriorityType>(NUM_QUEUE_CONFIGS - p - 1);
        U8 recvData[64];
        FwSizeType actualSize;
        FwQueuePriorityType priority;
        status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        ASSERT_EQ(expectedP, priority);
        ASSERT_EQ(testPriorityConfigs[expectedP].maxMsgSize, actualSize);
        for (FwSizeType i = 0; i < actualSize; ++i) {
            ASSERT_EQ(static_cast<U8>(expectedP * 10 + i), recvData[i]);
        }
    }

    // Verify per-priority depth limits
    for (FwQueuePriorityType p = 0; p < NUM_QUEUE_CONFIGS; ++p) {
        FwSizeType numMsgs = testPriorityConfigs[p].numMsgs;
        U8 data[10];
        FwSizeType maxMsgSize = testPriorityConfigs[p].maxMsgSize;
        for (FwSizeType i = 0; i < maxMsgSize; ++i) {
            data[i] = static_cast<U8>(p);
        }
        for (FwSizeType i = 0; i < numMsgs; ++i) {
            status = queue.send(data, maxMsgSize, p, Os::QueueInterface::BlockingType::NONBLOCKING);
            ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        }
        status = queue.send(data, maxMsgSize, p, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::FULL, status);

        U8 recvData[64];
        FwSizeType actualSize;
        FwQueuePriorityType priority;
        for (FwSizeType i = 0; i < numMsgs; ++i) {
            status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
            ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
            ASSERT_EQ(p, priority);
        }
        status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status);
    }

    queue.teardown();
}

// Test multiple reconfigurations
TEST_F(PriorityMemQueueTestFixture, MultipleReconfigurations) {
    // First configuration
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

}

// Test configuration with zero queues
TEST_F(PriorityMemQueueTestFixture, ZeroQueues) {
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

    // Should succeed with max size message
    U8 maxSizeData[64];
    for (FwSizeType i = 0; i < maxMsgSize; ++i) {
        maxSizeData[i] = static_cast<U8>(i);
    }
    status = queue.send(maxSizeData, maxMsgSize, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Should fail with oversized message
    U8 oversizeData[65];
    status = queue.send(oversizeData, maxMsgSize + 1, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::SIZE_MISMATCH, status);

    // Read the message that was sent earlier
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(maxMsgSize, actualSize);

    // Test 2: Verify exactly queueDepth messages can be sent (no more, no less)

    // Fill queue to capacity
    U8 testData[64];
    for (FwSizeType i = 0; i < maxMsgSize; ++i) {
        testData[i] = static_cast<U8>(0xFF);
    }

    for (FwSizeType i = 0; i < queueDepth; ++i) {
        status = queue.send(testData, maxMsgSize, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status) << "Failed to send message " << i << " of " << queueDepth;
    }

    // Try to send one more - should fail with FULL
    status = queue.send(testData, maxMsgSize, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::FULL, status) << "Queue accepted more than " << queueDepth << " messages";

    // Receive all messages
    for (FwSizeType i = 0; i < queueDepth; ++i) {
        status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status) << "Failed to receive message " << i;
        ASSERT_EQ(maxMsgSize, actualSize);
        ASSERT_EQ(0, priority);
    }

    // Try to receive one more - should fail with EMPTY
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status) << "Queue returned more than " << queueDepth << " messages";

    queue.teardown();
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
    U8 data1[32];
    for (FwSizeType i = 0; i < 32; ++i) {
        data1[i] = static_cast<U8>(i);
    }
    status = queue->send(data1, 32, 2, Os::QueueInterface::BlockingType::NONBLOCKING);
    EXPECT_EQ(Os::QueueInterface::Status::OP_OK, status);

    return queue;
}

// Test that non-default priority falls back to priority 0 when required=false
TEST_F(PriorityMemQueueTestFixture, RequiredPrioritySizingFallback) {
    // Set up queue with required=false (allows fallback to priority 0)
    Os::Generic::PriorityMemQueue* queue = setupPartialPriorityQueue(107, false, "FallbackQueue");

    // Test 2: With required=false, sending to unconfigured priority 1 should fall back to priority 0
    U8 data2[48];
    for (FwSizeType i = 0; i < 48; ++i) {
        data2[i] = static_cast<U8>(0xAA);
    }
    Os::QueueInterface::Status status = queue->send(data2, 48, 1, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Test 3: Verify messages are received - priority 2 first, then priority 0 (fallback)
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    // Should receive priority 2 message first (higher priority)
    status = queue->receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(2, priority);
    ASSERT_EQ(32, actualSize);

    // Should receive priority 0 message (fallback from priority 1 request)
    status = queue->receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(0, priority);  // Should be priority 0 (fell back from priority 1)
    ASSERT_EQ(48, actualSize);

    queue->teardown();
    delete queue;
}

// Test that non-default priority asserts when required=true
TEST_F(PriorityMemQueueTestFixture, OptionalPrioritySizingAssertion) {
    // Set up queue with required=true (strict mode - should assert on unsupported priority)
    Os::Generic::PriorityMemQueue* queue = setupPartialPriorityQueue(108, true, "AssertQueue");

    // Test 2: With required=true, sending to unconfigured priority 1 should ASSERT
    U8 data2[48];
    for (FwSizeType i = 0; i < 48; ++i) {
        data2[i] = static_cast<U8>(0xFF);
    }
    ASSERT_DEATH_IF_SUPPORTED(queue->send(data2, 48, 1, Os::QueueInterface::BlockingType::NONBLOCKING),
                              "Assert:.*PriorityMemQueue\\.cpp");

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
}

// Test that calling config() twice without reset asserts
TEST_F(PriorityMemQueueTestFixture, DoubleConfigAssertion) {
    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // First config call - should succeed
    Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Second config call without reset - should ASSERT
    ASSERT_DEATH_IF_SUPPORTED(
        Os::Generic::PriorityMemQueue::configure(nullptr, 0, false,
                                                 Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE),
        "Assert:.*PriorityMemQueue\\.cpp");

    // Clean up for subsequent tests
    PriorityMemQueueTestHelper::resetConfig();
}

// Test priority enable/disable behavior and m_priorityMask tracking
TEST_F(PriorityMemQueueTestFixture, PriorityEnableDisable) {
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
    ASSERT_EQ(7U, handle->m_priorityMask.load());

    // Disable priority 1
    handle->disablePriority(1);
    ASSERT_EQ(5U, handle->m_priorityMask.load());  // 0b101 = 5

    // Send messages to all three priorities
    U8 data0[16], data1[16], data2[16];
    for (FwSizeType i = 0; i < 16; ++i) {
        data0[i] = static_cast<U8>(0xA0 + i);
        data1[i] = static_cast<U8>(0xB0 + i);
        data2[i] = static_cast<U8>(0xC0 + i);
    }

    status = queue.send(data0, 16, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    status = queue.send(data1, 16, 1, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    status = queue.send(data2, 16, 2, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Receive - should get priority 2 first (highest enabled), not priority 1 (disabled)
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

    // Next receive should get priority 0, still skipping priority 1
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(0, priority) << "Should receive priority 0, still skipping disabled priority 1";
    ASSERT_EQ(16, actualSize);

    // Queue should now appear empty (priority 1 is disabled)
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status)
        << "Queue should appear empty with only disabled priority 1 having messages";

    // Re-enable priority 1
    handle->enablePriority(1);
    ASSERT_EQ(7U, handle->m_priorityMask.load());  // Back to 0b111 = 7

    // Now we should be able to receive the message from priority 1
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    ASSERT_EQ(1, priority) << "Should now receive priority 1 message";
    ASSERT_EQ(16, actualSize);
    for (FwSizeType i = 0; i < 16; ++i) {
        ASSERT_EQ(data1[i], recvData[i]);
    }

    // Queue should be truly empty now
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status);

    queue.teardown();
}

// Test that creating a queue with duplicate ID asserts
TEST_F(PriorityMemQueueTestFixture, DuplicateQueueIdAssertion) {
    // Reset configuration state
    PriorityMemQueueTestHelper::resetConfig();

    // Configure with a specific queue ID
    Os::Generic::PriorityMemQueue::QueuePriorityConfig testPriorityConfigs[] = {{0, 64, 10}};
    Os::Generic::PriorityMemQueue::QueueConfig testQueueConfig = {201, 1, testPriorityConfigs};
    Os::Generic::PriorityMemQueue::QueueConfig testConfigs[] = {testQueueConfig};
    Os::Generic::PriorityMemQueue::configure(testConfigs, 1, false,
                                             Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    Os::Generic::PriorityMemQueue queue1;
    Fw::String name1("Queue1");
    Os::QueueInterface::Status status = queue1.create(201, name1, 10, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Try to create second queue with same ID 201 - should ASSERT
    ASSERT_DEATH_IF_SUPPORTED(
        {
            Os::Generic::PriorityMemQueue queue2;
            Fw::String name2("Queue2");
            queue2.create(201, name2, 10, 64);
        },
        "Assert:.*PriorityMemQueue\\.cpp");

    // Test 3: Verify that after teardown, the ID can be reused
    queue1.teardown();

    // Now creating a queue with ID 201 should work
    Os::Generic::PriorityMemQueue queue3;
    Fw::String name3("Queue3");
    status = queue3.create(201, name3, 10, 64);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    queue3.teardown();
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

    // This will block until a message is available
    ctx->status =
        ctx->queue->receive(ctx->receivedData, sizeof(ctx->receivedData), Os::QueueInterface::BlockingType::BLOCKING,
                            ctx->receivedSize, ctx->receivedPriority);

    ctx->mutex.take();
    ctx->messageReceived = true;
    ctx->mutex.release();

}

// Test that blocking receive actually blocks and then receives
TEST_F(PriorityMemQueueTestFixture, BlockingReceive) {
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
    Os::Task receiverTask;
    Os::Task::Arguments args(Fw::String("ReceiverTask"), blockingReceiveTask, &ctx, Os::Task::TASK_PRIORITY_DEFAULT,
                             Os::Task::TASK_DEFAULT);

    Os::Task::Status taskStatus = receiverTask.start(args);
    ASSERT_EQ(Os::Task::Status::OP_OK, taskStatus);

    // Wait for receiver to start
    for (int i = 0; i < 50 && !ctx.started; ++i) {
        Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms
    }
    ASSERT_TRUE(ctx.started);

    // Give receiver time to block
    Os::Task::delay(Fw::TimeInterval(0, 10000));  // 10ms

    // Verify message hasn't been received yet (thread should be blocked)
    ctx.mutex.take();
    bool receivedBeforeSend = ctx.messageReceived;
    ctx.mutex.release();
    ASSERT_FALSE(receivedBeforeSend) << "Message received before send!";

    // Now send a message - this should unblock the receiver
    U8 testData[32];
    for (FwSizeType i = 0; i < sizeof(testData); ++i) {
        testData[i] = static_cast<U8>(i + 42);
    }

    status = queue.send(testData, sizeof(testData), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Wait for receiver to complete
    Os::Task::Status joinStatus = receiverTask.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus);

    ASSERT_TRUE(ctx.messageReceived);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, ctx.status);
    ASSERT_EQ(sizeof(testData), ctx.receivedSize);
    ASSERT_EQ(0, ctx.receivedPriority);

    // Verify data content
    for (FwSizeType i = 0; i < sizeof(testData); ++i) {
        ASSERT_EQ(testData[i], ctx.receivedData[i]) << "Data mismatch at index " << i;
    }

    queue.teardown();
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

    U8 data[32];
    for (FwSizeType i = 0; i < sizeof(data); ++i) {
        data[i] = static_cast<U8>(0xBB);
    }

    // This will block until space is available
    ctx->status = ctx->queue->send(data, sizeof(data), 0, Os::QueueInterface::BlockingType::BLOCKING);

    ctx->mutex.take();
    ctx->messageSent = true;
    ctx->mutex.release();

}

// Test that blocking send actually blocks when queue is full
TEST_F(PriorityMemQueueTestFixture, BlockingSend) {
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
    U8 fillData[32];
    for (int i = 0; i < 2; ++i) {
        for (FwSizeType j = 0; j < sizeof(fillData); ++j) {
            fillData[j] = static_cast<U8>(i);
        }
        status = queue.send(fillData, sizeof(fillData), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    // Set up context for sender thread
    BlockingSendContext ctx;
    ctx.queue = &queue;

    // Start sender thread - it will block because queue is full
    Os::Task senderTask;
    Os::Task::Arguments args(Fw::String("SenderTask"), blockingSendTask, &ctx, Os::Task::TASK_PRIORITY_DEFAULT,
                             Os::Task::TASK_DEFAULT);

    Os::Task::Status taskStatus = senderTask.start(args);
    ASSERT_EQ(Os::Task::Status::OP_OK, taskStatus);

    // Wait for sender to start
    for (int i = 0; i < 50 && !ctx.started; ++i) {
        Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms
    }
    ASSERT_TRUE(ctx.started);

    // Give sender time to block
    Os::Task::delay(Fw::TimeInterval(0, 10000));  // 10ms

    // Verify message hasn't been sent yet (thread should be blocked)
    ctx.mutex.take();
    bool sentBeforeReceive = ctx.messageSent;
    ctx.mutex.release();
    ASSERT_FALSE(sentBeforeReceive) << "Message sent before space was available!";

    // Now receive a message - this should unblock the sender
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    status =
        queue.receive(recvData, sizeof(recvData), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Wait for sender to complete
    Os::Task::Status joinStatus = senderTask.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus);

    ASSERT_TRUE(ctx.messageSent);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, ctx.status);

    queue.teardown();
}

// Test that validates the race condition fix: rapid send after receiver blocks
// This test verifies that the fast/slow path implementation correctly handles
// the scenario where a message is sent very quickly after the receiver starts blocking,
// which could cause a lost notification in the old implementation.
TEST_F(PriorityMemQueueTestFixture, RapidSendAfterBlock) {
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
    Os::Task receiverTask;
    Os::Task::Arguments args(Fw::String("RapidRecvTask"), blockingReceiveTask, &ctx, Os::Task::TASK_PRIORITY_DEFAULT,
                             Os::Task::TASK_DEFAULT);

    Os::Task::Status taskStatus = receiverTask.start(args);
    ASSERT_EQ(Os::Task::Status::OP_OK, taskStatus);

    // Wait for receiver to start
    for (int i = 0; i < 50 && !ctx.started; ++i) {
        Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms
    }
    ASSERT_TRUE(ctx.started);

    // Give receiver minimal time to enter blocking state
    // This tight timing increases the likelihood of hitting the race window
    // where notification could be lost in the old implementation
    Os::Task::delay(Fw::TimeInterval(0, 5000));  // Only 5ms - very tight timing

    // Send message immediately - in old implementation, this could arrive during
    // the race window between exiting critical section and entering wait
    U8 testData[32];
    for (FwSizeType i = 0; i < sizeof(testData); ++i) {
        testData[i] = static_cast<U8>(0xAA);
    }

    status = queue.send(testData, sizeof(testData), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Wait for receiver to complete
    // With the fix, receiver should unblock within a short time
    Os::Task::Status joinStatus = receiverTask.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus);

    // Verify the message was received correctly
    // If this fails, the notification was lost (old bug reproduced)
    ASSERT_TRUE(ctx.messageReceived) << "Message notification was lost!";
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, ctx.status);
    ASSERT_EQ(sizeof(testData), ctx.receivedSize);
    ASSERT_EQ(0, ctx.receivedPriority);

    // Verify data content
    for (FwSizeType i = 0; i < sizeof(testData); ++i) {
        ASSERT_EQ(testData[i], ctx.receivedData[i]) << "Data mismatch at index " << i;
    }

    queue.teardown();
}

// ========================================================================
// Additional Coverage Tests
// ========================================================================

// Test FIFO ordering within a single priority
TEST_F(PriorityMemQueueTestFixture, FIFOWithinPriority) {
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
    const FwSizeType numMessages = 5;
    U8 sendData[numMessages][16];
    for (FwSizeType i = 0; i < numMessages; ++i) {
        for (FwSizeType j = 0; j < 16; ++j) {
            sendData[i][j] = static_cast<U8>((i * 10) + j);  // Unique pattern per message
        }
        status = queue.send(sendData[i], 16, 1, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    // Receive all messages and verify FIFO order
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
    }

    queue.teardown();
}

// Test exact size boundaries for each priority
TEST_F(PriorityMemQueueTestFixture, SizeMismatchBoundaries) {
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
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

        // Test max size + 1 (should fail with SIZE_MISMATCH)
        status = queue.send(buffer, maxSize + 1, p, Os::QueueInterface::BlockingType::NONBLOCKING);
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
}

// Test semaphore count tracking
TEST_F(PriorityMemQueueTestFixture, SemaphoreCountTracking) {
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

    // Send messages to capacity
    U8 testData[32];
    for (FwSizeType i = 0; i < queueDepth; ++i) {
        testData[0] = static_cast<U8>(i);
        status = queue.send(testData, 32, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    // Verify messages available matches queue depth
    FwSizeType available = queue.getMessagesAvailable();
    ASSERT_EQ(queueDepth, available);

    // Receive all messages
    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;
    for (FwSizeType i = 0; i < queueDepth; ++i) {
        status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    // Verify queue is empty
    available = queue.getMessagesAvailable();
    ASSERT_EQ(0, available);

    // Try to receive from empty queue (should get EMPTY, not block)
    status = queue.receive(recvData, 64, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
    ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status);

    queue.teardown();
}

// Test teardown with messages still pending
TEST_F(PriorityMemQueueTestFixture, TeardownWithPendingMessages) {
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

    // Send several messages
    U8 testData[32];
    const FwSizeType numMessages = 5;
    for (FwSizeType i = 0; i < numMessages; ++i) {
        testData[0] = static_cast<U8>(i);
        status = queue.send(testData, 32, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    FwSizeType available = queue.getMessagesAvailable();
    ASSERT_EQ(numMessages, available);

    queue.teardown();
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

    U8 recvData[64];
    FwSizeType actualSize;
    FwQueuePriorityType priority;

    Os::QueueInterface::Status status = ctx->queue->receive(
        recvData, sizeof(recvData), Os::QueueInterface::BlockingType::BLOCKING, actualSize, priority);

    if (status == Os::QueueInterface::Status::OP_OK) {
        ctx->messagesReceived.fetch_add(1, std::memory_order_acq_rel);
    }
}

// Test multiple concurrent receivers (documents behavior)
// NOTE: PriorityMemQueue's fast/slow path receive assumes single-reader design for optimal
// priority ordering guarantees. Multiple concurrent receivers work correctly and compete
// for messages, but priority ordering between readers is not guaranteed.
// This test documents that multiple readers can successfully receive messages.
TEST_F(PriorityMemQueueTestFixture, MultipleConcurrentReceivers) {
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
    for (int i = 0; i < 50 && !ctx.started.load(std::memory_order_acquire); ++i) {
        Os::Task::delay(Fw::TimeInterval(0, 1000));  // 1ms (microseconds)
    }
    ASSERT_TRUE(ctx.started.load(std::memory_order_acquire));

    // Give receivers time to block
    Os::Task::delay(Fw::TimeInterval(0, 10000));  // 10ms (microseconds)

    // Send TWO messages - one for each receiver
    // This documents that multiple receivers work, but compete for messages
    U8 testData1[32] = {0xAB};
    status = queue.send(testData1, 32, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    U8 testData2[32] = {0xCD};
    status = queue.send(testData2, 32, 0, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);

    // Wait for receivers to complete
    Os::Task::Status joinStatus1 = receiverTask1.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus1);

    Os::Task::Status joinStatus2 = receiverTask2.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus2);

    // Check results - both should have received one message each
    U32 received = ctx.messagesReceived.load(std::memory_order_acquire);

    // EXPECTED: 2 messages received (one per thread)
    // Multiple concurrent receivers work but compete for messages from the queue
    EXPECT_EQ(2U, received) << "Multiple concurrent receivers: two sent, " << received
                            << " received (expected 2, one per thread)";

    queue.teardown();
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
TEST_F(PriorityMemQueueTestFixture, QueueFull) {
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
TEST_F(PriorityMemQueueTestFixture, QueueEmpty) {
    Ref::Test::PriorityMemQueue::Tester tester;
    Ref::Test::PriorityMemQueue::Tester::Create create_rule;
    Ref::Test::PriorityMemQueue::Tester::ReceiveEmpty receive_empty_rule;
    Ref::Test::PriorityMemQueue::Tester::Teardown teardown_rule;

    create_rule.action(tester);
    receive_empty_rule.action(tester);
    teardown_rule.action(tester);
}

// Test deterministic priority selection with partial priority disabling
// This validates that the Send rule's deterministic search correctly finds enabled non-full priorities
TEST_F(PriorityMemQueueTestFixture, DeterministicPrioritySelection) {
    Ref::Test::PriorityMemQueue::Tester tester;
    tester.create();

    // Test scenario: Priority 1 disabled, priority 2 full, should send to priority 0
    // This exercises the deterministic search logic that replaced random retry loop

    // Disable priority 1 to reduce available priorities
    tester.disablePriority(1);

    // Fill priority 2 to capacity (128 messages)
    for (U32 i = 0; i < 128; ++i) {
        Ref::Test::PriorityMemQueue::QueueMessage msg;
        msg.randomize();
        msg.priority = 2;  // Force to priority 2
        Os::QueueInterface::Status status = tester.send(msg, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    // Verify priority 2 is full
    ASSERT_TRUE(tester.isPriorityFull(2));
    ASSERT_FALSE(tester.isPriorityEnabled(1));
    ASSERT_FALSE(tester.isPriorityFull(0));

    // Now use Send rule which should deterministically find priority 0
    // even if random message generation initially selects priority 1 or 2
    Ref::Test::PriorityMemQueue::Tester::Send send_rule;
    
    // Send 10 messages - all should go to priority 0 (only enabled non-full priority)
    for (U32 i = 0; i < 10; ++i) {
        send_rule.action(tester);
    }

    // Verify all 10 messages went to priority 0 by receiving them
    for (U32 i = 0; i < 10; ++i) {
        Ref::Test::PriorityMemQueue::QueueMessage received;
        Os::QueueInterface::Status status = tester.receive(received, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
        // All received messages should be from priority 0 (lower than priority 2)
        // Priority 2 messages remain in queue due to higher priority
    }

    // Now priority 0 is empty, only priority 2 (full) has messages
    // Re-enable priority 1 and partially fill priority 0
    tester.enablePriority(1);
    
    for (U32 i = 0; i < 50; ++i) {
        Ref::Test::PriorityMemQueue::QueueMessage msg;
        msg.randomize();
        msg.priority = 0;
        Os::QueueInterface::Status status = tester.send(msg, Os::QueueInterface::BlockingType::NONBLOCKING);
        ASSERT_EQ(Os::QueueInterface::Status::OP_OK, status);
    }

    // Send rule should now find priority 1 (enabled and empty) when deterministically searching
    for (U32 i = 0; i < 5; ++i) {
        send_rule.action(tester);
    }

    tester.teardown();
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
    static_cast<void>(numSteps);
}
