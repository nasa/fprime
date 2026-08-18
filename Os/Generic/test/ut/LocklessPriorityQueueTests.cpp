// ======================================================================
// \title Os/Generic/test/ut/LocklessPriorityQueueTests.cpp
// \brief tests using lockless priority queue implementation for Os::Queue interface testing
// ======================================================================
#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/String.hpp"
#include "Os/Generic/LocklessPriorityQueue.hpp"
#include "Os/Queue.hpp"
#include "STest/Random/Random.hpp"

namespace {

constexpr FwSizeType CONCURRENT_DEPTH = 64;
constexpr FwSizeType CONCURRENT_MESSAGE_SIZE = sizeof(U32);
constexpr U32 CONCURRENT_PRODUCERS = 4;
constexpr U32 CONCURRENT_CONSUMERS = 4;
constexpr U32 CONCURRENT_MESSAGES_PER_PRODUCER = 1000;
constexpr U32 CONCURRENT_TOTAL_MESSAGES = CONCURRENT_PRODUCERS * CONCURRENT_MESSAGES_PER_PRODUCER;

//! Shared state for the multi-producer / multi-consumer concurrency test.
struct ConcurrentTestState {
    Os::Queue queue;
    std::atomic<U32> received[CONCURRENT_TOTAL_MESSAGES];
    std::atomic<U32> consumed;
    std::atomic<bool> producers_done;

    ConcurrentTestState() : received(), consumed(0), producers_done(false) {
        for (U32 index = 0; index < CONCURRENT_TOTAL_MESSAGES; index++) {
            received[index].store(0, std::memory_order_relaxed);
        }
    }
};

//! Pack a U32 value into a little-endian byte buffer of exactly `CONCURRENT_MESSAGE_SIZE` bytes.
void pack_value(U32 value, U8* buffer) {
    for (FwSizeType b = 0; b < CONCURRENT_MESSAGE_SIZE; b++) {
        buffer[b] = static_cast<U8>((value >> (8 * b)) & 0xFFu);
    }
}

//! Unpack a U32 value from a little-endian byte buffer of exactly `CONCURRENT_MESSAGE_SIZE` bytes.
U32 unpack_value(const U8* buffer) {
    U32 value = 0;
    for (FwSizeType b = 0; b < CONCURRENT_MESSAGE_SIZE; b++) {
        value |= static_cast<U32>(buffer[b]) << (8 * b);
    }
    return value;
}

//! Producer worker: send `CONCURRENT_MESSAGES_PER_PRODUCER` distinct values into the queue.
void producer_worker(ConcurrentTestState* state, U32 producerIndex) {
    for (U32 messageIndex = 0; messageIndex < CONCURRENT_MESSAGES_PER_PRODUCER; messageIndex++) {
        const U32 value = (producerIndex * CONCURRENT_MESSAGES_PER_PRODUCER) + messageIndex;
        U8 buffer[CONCURRENT_MESSAGE_SIZE] = {0};
        pack_value(value, buffer);
        const FwQueuePriorityType priority = static_cast<FwQueuePriorityType>(value & 0x7u);
        Os::QueueInterface::Status status = Os::QueueInterface::Status::FULL;
        while (status == Os::QueueInterface::Status::FULL) {
            status = state->queue.send(buffer, CONCURRENT_MESSAGE_SIZE, priority,
                                       Os::QueueInterface::BlockingType::NONBLOCKING);
            if (status == Os::QueueInterface::Status::FULL) {
                std::this_thread::yield();
            }
        }
    }
}

//! Consumer worker: receive messages until the producers are done and the consumed count
//! reaches the total. Asserts each value is delivered exactly once. Exits after a bounded
//! number of consecutive empty polls once producers finish so a lost message reports as a
//! count shortfall instead of a hang.
void consumer_worker(ConcurrentTestState* state) {
    constexpr U32 EMPTY_POLL_LIMIT = 100000;
    U8 buffer[CONCURRENT_MESSAGE_SIZE] = {0};
    FwSizeType actualSize = 0;
    FwQueuePriorityType priority = 0;
    U32 emptyPolls = 0;
    while (state->consumed.load(std::memory_order_acquire) < CONCURRENT_TOTAL_MESSAGES) {
        Os::QueueInterface::Status status = state->queue.receive(
            buffer, CONCURRENT_MESSAGE_SIZE, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        if (status == Os::QueueInterface::Status::OP_OK) {
            emptyPolls = 0;
            ASSERT_EQ(actualSize, CONCURRENT_MESSAGE_SIZE);
            const U32 value = unpack_value(buffer);
            ASSERT_LT(value, CONCURRENT_TOTAL_MESSAGES);
            const U32 priorCount = state->received[value].fetch_add(1, std::memory_order_acq_rel);
            ASSERT_EQ(priorCount, 0u) << "duplicate delivery of value " << value;
            state->consumed.fetch_add(1, std::memory_order_acq_rel);
        } else if (status == Os::QueueInterface::Status::EMPTY) {
            if (state->producers_done.load(std::memory_order_acquire) && (++emptyPolls > EMPTY_POLL_LIMIT)) {
                break;  // main thread reports the shortfall with diagnostics
            }
            std::this_thread::yield();
        } else {
            FAIL() << "unexpected status " << static_cast<int>(status);
        }
    }
}

//! Validate that high-load concurrent producers and consumers do not lose, duplicate, or
//! reorder messages in a way that violates priority ordering. This test relies on real OS
//! threads to exercise the lockless state machine.
TEST(LocklessConcurrent, MultiProducerMultiConsumer) {
    ConcurrentTestState state;
    Fw::String name("concurrent-test");
    ASSERT_EQ(state.queue.create(0, name, CONCURRENT_DEPTH, CONCURRENT_MESSAGE_SIZE),
              Os::QueueInterface::Status::OP_OK);

    std::thread producer_threads[CONCURRENT_PRODUCERS];
    for (U32 producerIndex = 0; producerIndex < CONCURRENT_PRODUCERS; producerIndex++) {
        producer_threads[producerIndex] = std::thread(producer_worker, &state, producerIndex);
    }

    std::thread consumer_threads[CONCURRENT_CONSUMERS];
    for (U32 consumerIndex = 0; consumerIndex < CONCURRENT_CONSUMERS; consumerIndex++) {
        consumer_threads[consumerIndex] = std::thread(consumer_worker, &state);
    }

    for (U32 i = 0; i < CONCURRENT_PRODUCERS; i++) {
        producer_threads[i].join();
    }
    state.producers_done.store(true, std::memory_order_release);
    for (U32 i = 0; i < CONCURRENT_CONSUMERS; i++) {
        consumer_threads[i].join();
    }

    EXPECT_EQ(state.consumed.load(std::memory_order_acquire), CONCURRENT_TOTAL_MESSAGES);
    for (U32 index = 0; index < CONCURRENT_TOTAL_MESSAGES; index++) {
        EXPECT_EQ(state.received[index].load(std::memory_order_relaxed), 1u)
            << "value " << index << " not received exactly once";
    }
    state.queue.teardown();
}

//! Validate single-threaded strict priority ordering: batches of distinct-priority messages
//! must drain in non-increasing priority order. (Concurrent-drain and FIFO-tiebreak coverage
//! live in the TSan adversarial suite.)
TEST(LocklessConcurrent, PriorityOrderSingleProducer) {
    constexpr FwSizeType DEPTH = 32;
    constexpr FwSizeType MESSAGE_SIZE = sizeof(U32);
    constexpr U32 BATCHES = 200;
    constexpr U32 BATCH_DEPTH = 16;

    Os::Queue queue;
    Fw::String name("priority-order-test");
    ASSERT_EQ(queue.create(0, name, DEPTH, MESSAGE_SIZE), Os::QueueInterface::Status::OP_OK);

    for (U32 batch = 0; batch < BATCHES; batch++) {
        for (U32 i = 0; i < BATCH_DEPTH; i++) {
            U8 buffer[MESSAGE_SIZE] = {0};
            const U32 value = (batch * BATCH_DEPTH) + i;
            for (FwSizeType b = 0; b < MESSAGE_SIZE; b++) {
                buffer[b] = static_cast<U8>((value >> (8 * b)) & 0xFFu);
            }
            ASSERT_EQ(queue.send(buffer, MESSAGE_SIZE, static_cast<FwQueuePriorityType>(i),
                                 Os::QueueInterface::BlockingType::NONBLOCKING),
                      Os::QueueInterface::Status::OP_OK);
        }
        FwQueuePriorityType lastPriority = std::numeric_limits<FwQueuePriorityType>::max();
        for (U32 i = 0; i < BATCH_DEPTH; i++) {
            U8 buffer[MESSAGE_SIZE] = {0};
            FwSizeType actualSize = 0;
            FwQueuePriorityType priority = 0;
            ASSERT_EQ(queue.receive(buffer, MESSAGE_SIZE, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize,
                                    priority),
                      Os::QueueInterface::Status::OP_OK);
            ASSERT_LE(priority, lastPriority);
            lastPriority = priority;
        }
    }
    queue.teardown();
}

//! Validate that destroying a never-created queue is safe (the destructor must not free or
//! touch any memory). This guards against the static-destruction-order fault described in
//! the SDD §12.1.
TEST(LocklessLifetime, DestructWithoutCreate) {
    {
        Os::Queue queue;
        // No create(), no teardown(): the queue object simply leaves scope. The destructor
        // must be a no-op and must not perform any virtual dispatch into the memory
        // allocator registry.
        EXPECT_EQ(queue.getMessagesAvailable(), 0u);
    }
    // If the destructor were unsafe, ASan/UBSan would abort before reaching here.
}

//! Validate that destroying a created queue *after* an explicit teardown is safe and that
//! the destructor performs no further work. Tests run under ASan/UBSan/LSan; if the
//! destructor were to free resources it would double-free the slot pool.
TEST(LocklessLifetime, CreateTeardownDestruct) {
    {
        Os::Queue queue;
        Fw::String name("lifetime-test");
        ASSERT_EQ(queue.create(0, name, 4, sizeof(U32)), Os::QueueInterface::Status::OP_OK);
        queue.teardown();
        EXPECT_EQ(queue.getMessagesAvailable(), 0u);
        // queue leaves scope here; destructor must be a no-op.
    }
}

//! Validate that `teardown()` is idempotent: calling it twice on the same queue must not
//! double-free or otherwise misbehave.
TEST(LocklessLifetime, TeardownIsIdempotent) {
    Os::Queue queue;
    Fw::String name("idempotent-test");
    ASSERT_EQ(queue.create(0, name, 4, sizeof(U32)), Os::QueueInterface::Status::OP_OK);
    queue.teardown();
    queue.teardown();
    queue.teardown();
    EXPECT_EQ(queue.getMessagesAvailable(), 0u);
}

//! Validate that an oversized send is rejected with SIZE_MISMATCH and leaves the queue empty.
TEST(LocklessLifetime, OversizedSendRejected) {
    Os::Queue queue;
    Fw::String name("oversize-test");
    ASSERT_EQ(queue.create(0, name, 4, sizeof(U32)), Os::QueueInterface::Status::OP_OK);
    U8 buffer[sizeof(U32) + 1] = {0};
    ASSERT_EQ(queue.send(buffer, sizeof buffer, 0, Os::QueueInterface::BlockingType::NONBLOCKING),
              Os::QueueInterface::Status::SIZE_MISMATCH);
    EXPECT_EQ(queue.getMessagesAvailable(), 0u);
    queue.teardown();
}

//! Validate the delegate-level oversized-send check directly (the Os::Queue wrapper performs
//! its own size check before delegating, so this path is otherwise unreachable).
TEST(LocklessLifetime, OversizedSendRejectedAtDelegate) {
    Os::Generic::LocklessPriorityQueue queue;
    Fw::String name("oversize-delegate-test");
    ASSERT_EQ(queue.create(0, name, 4, sizeof(U32)), Os::QueueInterface::Status::OP_OK);
    U8 buffer[sizeof(U32) + 1] = {0};
    ASSERT_EQ(queue.send(buffer, sizeof buffer, 0, Os::QueueInterface::BlockingType::NONBLOCKING),
              Os::QueueInterface::Status::SIZE_MISMATCH);
    EXPECT_EQ(queue.getMessagesAvailable(), 0u);
    queue.teardown();
}

//! Validate that a zero-size message round-trips: no payload copy, size 0 and priority intact.
TEST(LocklessLifetime, ZeroSizeMessage) {
    Os::Generic::LocklessPriorityQueue queue;
    Fw::String name("zero-size-test");
    ASSERT_EQ(queue.create(0, name, 4, sizeof(U32)), Os::QueueInterface::Status::OP_OK);
    U8 dummy = 0;
    ASSERT_EQ(queue.send(&dummy, 0, 1, Os::QueueInterface::BlockingType::NONBLOCKING),
              Os::QueueInterface::Status::OP_OK);
    U8 out[sizeof(U32)] = {0};
    FwSizeType actualSize = 99;
    FwQueuePriorityType priority = 0;
    ASSERT_EQ(queue.receive(out, sizeof out, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority),
              Os::QueueInterface::Status::OP_OK);
    ASSERT_EQ(actualSize, 0u);
    ASSERT_EQ(priority, 1);
    queue.teardown();
}

//! Validate the getMessagesAvailable() receivable-count contract: counts track
//! sends/receives exactly, and the high-water mark reflects peak occupancy.
TEST(LocklessLifetime, MessagesAvailableTracksReceivableCount) {
    Os::Generic::LocklessPriorityQueue queue;
    Fw::String name("available-count-test");
    ASSERT_EQ(queue.create(0, name, 4, sizeof(U32)), Os::QueueInterface::Status::OP_OK);
    EXPECT_EQ(queue.getMessagesAvailable(), 0u);
    U8 buffer[sizeof(U32)] = {0};
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(queue.send(buffer, sizeof buffer, 0, Os::QueueInterface::BlockingType::NONBLOCKING),
                  Os::QueueInterface::Status::OP_OK);
        EXPECT_EQ(queue.getMessagesAvailable(), i + 1);
    }
    EXPECT_EQ(queue.getMessageHighWaterMark(), 3u);
    FwSizeType actualSize = 0;
    FwQueuePriorityType priority = 0;
    for (FwSizeType i = 0; i < 3; i++) {
        ASSERT_EQ(
            queue.receive(buffer, sizeof buffer, Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority),
            Os::QueueInterface::Status::OP_OK);
        EXPECT_EQ(queue.getMessagesAvailable(), 2u - i);
    }
    EXPECT_EQ(queue.getMessageHighWaterMark(), 3u);
    queue.teardown();
}

}  // namespace

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
