// ======================================================================
// \title Os/Generic/test/ut/LocklessPriorityQueueTsanTests.cpp
// \brief TSan stress tests and adversarial scenarios for the lockless priority queue
//
// These tests are designed to be compiled with -fsanitize=thread and exercise
// the lock-free state machine under high contention. They complement the
// existing CommonTests (queue rules) and LocklessPriorityQueueTests by
// amplifying thread counts, shrinking queue depth, and targeting corner cases
// that are most likely to expose data races or ABA issues.
// ======================================================================

#include <gtest/gtest.h>
#include <algorithm>
#include <atomic>
#include <cstring>
#include <limits>
#include <thread>
#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/String.hpp"
#include "Os/Generic/LocklessPriorityQueue.hpp"
#include "Os/Queue.hpp"

namespace {

// -----------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------

void pack_u32(U32 value, U8* buffer) {
    for (FwSizeType b = 0; b < sizeof(U32); b++) {
        buffer[b] = static_cast<U8>((value >> (8 * b)) & 0xFFu);
    }
}

U32 unpack_u32(const U8* buffer) {
    U32 value = 0;
    for (FwSizeType b = 0; b < sizeof(U32); b++) {
        value |= static_cast<U32>(buffer[b]) << (8 * b);
    }
    return value;
}

// -----------------------------------------------------------------------
// Generic multi-producer multi-consumer harness
// -----------------------------------------------------------------------

//! Maximum distinct priority values tracked by the harness.
constexpr U32 MAX_PRIORITIES = 8;

//! Per-message priority function: maps a message value to its send priority.
using PriorityFn = FwQueuePriorityType (*)(U32 value);

FwQueuePriorityType priority_mod8(U32 value) {
    return static_cast<FwQueuePriorityType>(value % MAX_PRIORITIES);
}

struct ConcurrentConfig {
    FwSizeType depth = 0;
    U32 producers = 0;
    U32 consumers = 0;
    U32 messagesPerProducer = 0;
    //! Messages sent to the queue before any thread starts (values total..total+prefill-1).
    U32 prefill = 0;
    //! Priority assigned to each message; nullptr sends everything at priority 0.
    PriorityFn priorityOf = nullptr;
    //! When true, expect the high-water mark to equal depth exactly (requires prefill == depth).
    bool expectHighMarkEqualsDepth = false;
};

struct ConcurrentState {
    Os::Queue queue;
    std::atomic<U32>* received;  // array of size expectedTotal
    std::atomic<U32> receivedByPriority[MAX_PRIORITIES];
    std::atomic<U32> consumed;
    std::atomic<bool> producersDone;
    U32 expectedTotal;
    PriorityFn priorityOf;

    ConcurrentState(U32 total, PriorityFn priorityFn)
        : received(new std::atomic<U32>[total]),
          consumed(0),
          producersDone(false),
          expectedTotal(total),
          priorityOf(priorityFn) {
        for (U32 i = 0; i < total; i++) {
            received[i].store(0, std::memory_order_relaxed);
        }
        for (U32 i = 0; i < MAX_PRIORITIES; i++) {
            receivedByPriority[i].store(0, std::memory_order_relaxed);
        }
    }
    ~ConcurrentState() { delete[] received; }

    FwQueuePriorityType priorityFor(U32 value) const {
        return (this->priorityOf != nullptr) ? this->priorityOf(value) : static_cast<FwQueuePriorityType>(0);
    }
};

void send_until_ok(ConcurrentState* state, U32 value) {
    U8 buffer[sizeof(U32)] = {0};
    pack_u32(value, buffer);
    const FwQueuePriorityType priority = state->priorityFor(value);
    Os::QueueInterface::Status status = Os::QueueInterface::Status::FULL;
    while (status == Os::QueueInterface::Status::FULL) {
        status = state->queue.send(buffer, sizeof(U32), priority, Os::QueueInterface::BlockingType::NONBLOCKING);
        if (status == Os::QueueInterface::Status::FULL) {
            std::this_thread::yield();
        }
    }
    ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);
}

void concurrent_producer(ConcurrentState* state, U32 producerIndex, U32 messagesPerProducer) {
    for (U32 i = 0; i < messagesPerProducer; i++) {
        send_until_ok(state, (producerIndex * messagesPerProducer) + i);
    }
}

//! Consumer worker. Exits after a bounded number of consecutive empty polls once producers
//! finish so a lost message reports as a count shortfall instead of a hang.
void concurrent_consumer(ConcurrentState* state) {
    constexpr U32 EMPTY_POLL_LIMIT = 100000;
    U8 buffer[sizeof(U32)] = {0};
    FwSizeType actualSize = 0;
    FwQueuePriorityType priority = 0;
    U32 emptyPolls = 0;
    while (state->consumed.load(std::memory_order_acquire) < state->expectedTotal) {
        Os::QueueInterface::Status status = state->queue.receive(
            buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        if (status == Os::QueueInterface::Status::OP_OK) {
            emptyPolls = 0;
            ASSERT_EQ(actualSize, sizeof(U32));
            const U32 value = unpack_u32(buffer);
            ASSERT_LT(value, state->expectedTotal);
            ASSERT_LT(static_cast<U32>(priority), MAX_PRIORITIES);
            const U32 prior = state->received[value].fetch_add(1, std::memory_order_acq_rel);
            ASSERT_EQ(prior, 0u) << "duplicate delivery of value " << value;
            state->receivedByPriority[priority].fetch_add(1, std::memory_order_acq_rel);
            state->consumed.fetch_add(1, std::memory_order_acq_rel);
        } else if (status == Os::QueueInterface::Status::EMPTY) {
            if (state->producersDone.load(std::memory_order_acquire) && (++emptyPolls > EMPTY_POLL_LIMIT)) {
                break;  // main thread reports the shortfall with diagnostics
            }
            std::this_thread::yield();
        } else {
            FAIL() << "unexpected status " << static_cast<int>(status);
        }
    }
}

void run_concurrent(const ConcurrentConfig& cfg) {
    const U32 produced = cfg.producers * cfg.messagesPerProducer;
    const U32 total = produced + cfg.prefill;
    ConcurrentState state(total, cfg.priorityOf);
    Fw::String name("tsan-concurrent");
    ASSERT_EQ(state.queue.create(0, name, cfg.depth, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

    // Pre-fill before any thread starts (values produced..total-1). When prefill == depth this
    // deterministically drives the count -- and thus the high-water mark -- to exactly depth.
    ASSERT_LE(static_cast<FwSizeType>(cfg.prefill), cfg.depth);
    for (U32 i = 0; i < cfg.prefill; i++) {
        send_until_ok(&state, produced + i);
    }
    if (cfg.prefill > 0) {
        ASSERT_EQ(state.queue.getMessagesAvailable(), static_cast<FwSizeType>(cfg.prefill));
    }

    // Fixed-size arrays -- sized to the maximum we use in any test (16)
    static constexpr U32 MAX_THREADS = 16;
    ASSERT_LE(cfg.producers, MAX_THREADS);
    ASSERT_LE(cfg.consumers, MAX_THREADS);
    std::thread producers[MAX_THREADS];
    std::thread consumers[MAX_THREADS];

    for (U32 i = 0; i < cfg.consumers; i++) {
        consumers[i] = std::thread(concurrent_consumer, &state);
    }
    for (U32 i = 0; i < cfg.producers; i++) {
        producers[i] = std::thread(concurrent_producer, &state, i, cfg.messagesPerProducer);
    }

    for (U32 i = 0; i < cfg.producers; i++) {
        producers[i].join();
    }
    state.producersDone.store(true, std::memory_order_release);
    for (U32 i = 0; i < cfg.consumers; i++) {
        consumers[i].join();
    }

    EXPECT_EQ(state.consumed.load(std::memory_order_acquire), total);
    for (U32 i = 0; i < total; i++) {
        EXPECT_EQ(state.received[i].load(std::memory_order_relaxed), 1u) << "value " << i;
    }
    if (cfg.priorityOf != nullptr) {
        // Verify the received per-priority distribution matches what was sent.
        U32 expectedByPriority[MAX_PRIORITIES] = {0};
        for (U32 i = 0; i < total; i++) {
            expectedByPriority[cfg.priorityOf(i)]++;
        }
        for (U32 p = 0; p < MAX_PRIORITIES; p++) {
            EXPECT_EQ(state.receivedByPriority[p].load(std::memory_order_relaxed), expectedByPriority[p])
                << "priority=" << p;
        }
    }
    if (cfg.expectHighMarkEqualsDepth) {
        EXPECT_EQ(state.queue.getMessageHighWaterMark(), cfg.depth);
    }
    // The high-water mark must never exceed depth (count invariant, SDD section 8).
    EXPECT_LE(state.queue.getMessageHighWaterMark(), cfg.depth);
    EXPECT_EQ(state.queue.getMessagesAvailable(), 0u);
    state.queue.teardown();
}

// =======================================================================
// TSan Stress Tests
// =======================================================================

// High-thread-count stress: 8 producers, 8 consumers, 40k messages through a 32-slot queue.
// Repeated multiple iterations to exercise different scheduling orders.
TEST(TsanStress, HighThreadConcurrent) {
    constexpr U32 ITERATIONS = 20;
    const ConcurrentConfig cfg{32, 8, 8, 5000, 0, priority_mod8, false};
    for (U32 iter = 0; iter < ITERATIONS; iter++) {
        run_concurrent(cfg);
    }
}

// Extreme contention: many threads fighting over a tiny queue.
TEST(TsanStress, TinyQueueHighContention) {
    constexpr U32 ITERATIONS = 50;
    const ConcurrentConfig cfg{4, 8, 8, 500, 0, priority_mod8, false};
    for (U32 iter = 0; iter < ITERATIONS; iter++) {
        run_concurrent(cfg);
    }
}

// Asymmetric: many producers, few consumers -- queue spends time near FULL.
TEST(TsanStress, ManyProducersFewConsumers) {
    constexpr U32 ITERATIONS = 20;
    const ConcurrentConfig cfg{16, 12, 2, 1000, 0, priority_mod8, false};
    for (U32 iter = 0; iter < ITERATIONS; iter++) {
        run_concurrent(cfg);
    }
}

// Asymmetric: few producers, many consumers -- queue spends time near EMPTY.
TEST(TsanStress, FewProducersManyConsumers) {
    constexpr U32 ITERATIONS = 20;
    const ConcurrentConfig cfg{16, 2, 12, 5000, 0, priority_mod8, false};
    for (U32 iter = 0; iter < ITERATIONS; iter++) {
        run_concurrent(cfg);
    }
}

// =======================================================================
// Adversarial Scenarios
// =======================================================================

// Depth-1 queue: maximum contention per slot. Every send/receive pair must
// race for the single slot, exercising the CAS retry logic at its tightest.
TEST(Adversarial, DepthOnePingPong) {
    constexpr U32 ITERATIONS = 50;
    const ConcurrentConfig cfg{1, 4, 4, 500, 0, priority_mod8, false};
    for (U32 iter = 0; iter < ITERATIONS; iter++) {
        run_concurrent(cfg);
    }
}

// Full-queue storm: pre-fill the queue, then unleash many producers (all get FULL)
// while consumers drain. Verifies no message is lost during the transition.
TEST(Adversarial, FullQueueStorm) {
    const ConcurrentConfig cfg{16, 8, 8, 2000, 16, nullptr, false};
    run_concurrent(cfg);
}

// Single-slot ping-pong: one producer, one consumer, depth=1. The tightest
// possible producer-consumer handoff through the atomic state machine.
TEST(Adversarial, SingleSlotPingPong) {
    constexpr FwSizeType DEPTH = 1;
    constexpr U32 MESSAGES = 50000;

    Os::Queue queue;
    Fw::String name("ping-pong");
    ASSERT_EQ(queue.create(0, name, DEPTH, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

    std::thread producer([&queue, MESSAGES]() {
        for (U32 i = 0; i < MESSAGES; i++) {
            U8 buffer[sizeof(U32)] = {0};
            pack_u32(i, buffer);
            Os::QueueInterface::Status status = Os::QueueInterface::Status::FULL;
            while (status == Os::QueueInterface::Status::FULL) {
                status = queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
                if (status == Os::QueueInterface::Status::FULL) {
                    std::this_thread::yield();
                }
            }
        }
    });

    std::thread consumer([&queue, MESSAGES]() {
        U32 expected = 0;
        while (expected < MESSAGES) {
            U8 buffer[sizeof(U32)] = {0};
            FwSizeType actualSize = 0;
            FwQueuePriorityType priority = 0;
            Os::QueueInterface::Status status =
                queue.receive(buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
            if (status == Os::QueueInterface::Status::OP_OK) {
                ASSERT_EQ(actualSize, sizeof(U32));
                // With 1P/1C/depth=1, messages must arrive in FIFO order
                ASSERT_EQ(unpack_u32(buffer), expected) << "out-of-order at expected=" << expected;
                expected++;
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();
    queue.teardown();
}

// Verify that high-water mark is exact under contention. The queue is pre-filled to DEPTH
// before any consumer starts, so the mark must equal DEPTH; the count invariant (increment
// before READY, decrement before FREE) guarantees it can never exceed DEPTH.
TEST(Adversarial, HighWaterMarkAccuracy) {
    const ConcurrentConfig cfg{16, 8, 1, 2000, 16, nullptr, true};
    run_concurrent(cfg);
}

// Deterministic high-water mark: send 3, drain, send 1 -- the mark must be exactly 3.
TEST(Adversarial, HighWaterMarkIntermediate) {
    Os::Queue queue;
    Fw::String name("high-mark-intermediate");
    ASSERT_EQ(queue.create(0, name, 8, sizeof(U32)), Os::QueueInterface::Status::OP_OK);
    U8 buffer[sizeof(U32)] = {0};
    for (U32 i = 0; i < 3; i++) {
        pack_u32(i, buffer);
        ASSERT_EQ(queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::NONBLOCKING),
                  Os::QueueInterface::Status::OP_OK);
    }
    FwSizeType actualSize = 0;
    FwQueuePriorityType priority = 0;
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(
            queue.receive(buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority),
            Os::QueueInterface::Status::OP_OK);
    }
    pack_u32(0, buffer);
    ASSERT_EQ(queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::NONBLOCKING),
              Os::QueueInterface::Status::OP_OK);
    EXPECT_EQ(queue.getMessageHighWaterMark(), 3u);
    queue.teardown();
}

// Blocking-path stress: producers use BLOCKING sends into a small queue while consumers use
// BLOCKING receives, exercising the backoff/delay control flow under TSan.
TEST(Adversarial, BlockingPathStress) {
    static constexpr FwSizeType DEPTH = 4;
    static constexpr U32 THREADS = 4;
    static constexpr U32 MESSAGES_PER_PRODUCER = 1000;
    static constexpr U32 TOTAL = THREADS * MESSAGES_PER_PRODUCER;

    Os::Queue queue;
    Fw::String name("blocking-stress");
    ASSERT_EQ(queue.create(0, name, DEPTH, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

    std::atomic<U32> received[TOTAL];
    for (U32 i = 0; i < TOTAL; i++) {
        received[i].store(0, std::memory_order_relaxed);
    }

    std::thread producers[THREADS];
    for (U32 t = 0; t < THREADS; t++) {
        producers[t] = std::thread([&queue, t]() {
            for (U32 i = 0; i < MESSAGES_PER_PRODUCER; i++) {
                U8 buffer[sizeof(U32)] = {0};
                pack_u32((t * MESSAGES_PER_PRODUCER) + i, buffer);
                ASSERT_EQ(queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::BLOCKING),
                          Os::QueueInterface::Status::OP_OK);
            }
        });
    }

    std::thread consumers[THREADS];
    for (U32 t = 0; t < THREADS; t++) {
        consumers[t] = std::thread([&queue, &received]() {
            for (U32 i = 0; i < MESSAGES_PER_PRODUCER; i++) {
                U8 buffer[sizeof(U32)] = {0};
                FwSizeType actualSize = 0;
                FwQueuePriorityType priority = 0;
                ASSERT_EQ(queue.receive(buffer, sizeof(U32), Os::QueueInterface::BlockingType::BLOCKING, actualSize,
                                        priority),
                          Os::QueueInterface::Status::OP_OK);
                const U32 value = unpack_u32(buffer);
                ASSERT_LT(value, TOTAL);
                received[value].fetch_add(1, std::memory_order_acq_rel);
            }
        });
    }

    for (U32 t = 0; t < THREADS; t++) {
        producers[t].join();
    }
    for (U32 t = 0; t < THREADS; t++) {
        consumers[t].join();
    }
    for (U32 i = 0; i < TOTAL; i++) {
        EXPECT_EQ(received[i].load(std::memory_order_relaxed), 1u) << "value " << i;
    }
    EXPECT_EQ(queue.getMessagesAvailable(), 0u);
    queue.teardown();
}

// Create-teardown-recreate: verify the queue is fully functional after
// a teardown + create cycle. Tests internal state cleanup. Each
// run_concurrent invocation performs a full create/use/teardown cycle.
TEST(Adversarial, CreateTeardownRecreate) {
    constexpr U32 CYCLES = 5;
    const ConcurrentConfig cfg{8, 4, 4, 250, 0, nullptr, false};
    for (U32 cycle = 0; cycle < CYCLES; cycle++) {
        run_concurrent(cfg);
    }
}

// Sequence-wrap logic test: directly verify that the production comparison,
// LocklessPriorityQueue::isCandidatePreferred, handles wrap-around of the
// sequence counter correctly for equal-priority messages.
TEST(Adversarial, SequenceWrapComparison) {
    // At equal priority, the candidate is preferred exactly when it is "older"
    // (smaller in the wrap-aware modular ordering) than the current best.
    const U32 topBit = static_cast<U32>(1) << (std::numeric_limits<U32>::digits - 1);
    auto isOlder = [](U32 candidate, U32 best) -> bool {
        return Os::Generic::LocklessPriorityQueue::isCandidatePreferred(0, candidate, 0, best);
    };

    // Normal case: candidate < best  =>  candidate is older
    EXPECT_TRUE(isOlder(5, 10));
    EXPECT_FALSE(isOlder(10, 5));
    EXPECT_FALSE(isOlder(5, 5));  // equal => not older

    // Wrap case: candidate near U32_MAX, best near 0  =>  candidate is older
    EXPECT_TRUE(isOlder(std::numeric_limits<U32>::max() - 5, 3));
    // Reverse wrap: candidate near 0, best near U32_MAX  =>  candidate is newer
    EXPECT_FALSE(isOlder(3, std::numeric_limits<U32>::max() - 5));

    // Edge: exactly at the half-way boundary — both directions return true
    // because (0 - 0x80000000) == (0x80000000 - 0) == 0x80000000 in unsigned
    // arithmetic. This ambiguity is harmless in production: queue depth is
    // always far smaller than 2^31, so two active slots can never be exactly
    // half the U32 domain apart in sequence number.
    EXPECT_TRUE(isOlder(0, topBit));
    EXPECT_TRUE(isOlder(topBit, 0));

    // Slightly off the boundary — these ARE well-defined
    EXPECT_FALSE(isOlder(0, topBit + 1));  // 0 is newer than topBit+1
    EXPECT_TRUE(isOlder(topBit + 1, 0));   // topBit+1 is older than 0
    EXPECT_TRUE(isOlder(0, topBit - 1));   // 0 is older than topBit-1
    EXPECT_FALSE(isOlder(topBit - 1, 0));  // topBit-1 is newer than 0
}

// Mixed priority under contention: producers send messages with varying
// priorities. The concurrent phase verifies exactly-once delivery and
// per-priority distribution; the deterministic phase verifies strict
// highest-priority-first selection with FIFO tie-breaking.
TEST(Adversarial, MixedPriorityContention) {
    constexpr FwSizeType DEPTH = 32;
    constexpr U32 NUM_PRIORITIES = MAX_PRIORITIES;

    // Concurrent phase: exactly-once delivery and per-priority distribution under contention.
    const ConcurrentConfig cfg{DEPTH, 4, 4, 2000, 0, priority_mod8, false};
    run_concurrent(cfg);

    // Deterministic phase: with no concurrent senders, fill the queue with a shuffled mix of
    // priorities and verify a single-threaded drain returns strictly highest-priority-first,
    // FIFO within equal priorities.
    Os::Queue queue;
    Fw::String name("mixed-prio");
    ASSERT_EQ(queue.create(0, name, DEPTH, sizeof(U32)), Os::QueueInterface::Status::OP_OK);
    for (U32 i = 0; i < static_cast<U32>(DEPTH); i++) {
        U8 buffer[sizeof(U32)] = {0};
        pack_u32(i, buffer);
        const FwQueuePriorityType priority = static_cast<FwQueuePriorityType>((i * 5) % NUM_PRIORITIES);
        ASSERT_EQ(queue.send(buffer, sizeof(U32), priority, Os::QueueInterface::BlockingType::NONBLOCKING),
                  Os::QueueInterface::Status::OP_OK);
    }
    FwQueuePriorityType lastPriority = static_cast<FwQueuePriorityType>(NUM_PRIORITIES - 1);
    U32 lastValue = 0;
    bool havePrevious = false;
    for (U32 i = 0; i < static_cast<U32>(DEPTH); i++) {
        U8 buffer[sizeof(U32)] = {0};
        FwSizeType actualSize = 0;
        FwQueuePriorityType priority = 0;
        ASSERT_EQ(
            queue.receive(buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority),
            Os::QueueInterface::Status::OP_OK);
        const U32 value = unpack_u32(buffer);
        if (havePrevious && (priority == lastPriority)) {
            EXPECT_LT(lastValue, value) << "equal-priority FIFO violated at priority " << priority;
        } else {
            EXPECT_LE(priority, lastPriority) << "priority order violated";
        }
        lastPriority = priority;
        lastValue = value;
        havePrevious = true;
    }
    EXPECT_EQ(queue.getMessagesAvailable(), 0u);
    queue.teardown();
}

// Rapid teardown after drain: multiple consumers race to drain the queue,
// then teardown() is called immediately (by the harness, right after the
// joins). Verifies no use-after-free.
TEST(Adversarial, RapidDrainThenTeardown) {
    const ConcurrentConfig cfg{64, 1, 8, 9936, 64, nullptr, false};
    run_concurrent(cfg);
}

// Batched priority selection under contention: publish a full batch of mixed
// priorities with no consumer running, release all consumers through a
// barrier, and drain concurrently. Because the batch only shrinks during the
// drain, the highest available priority is non-increasing over time, so each
// consumer's own receive sequence must be non-increasing if every receive
// selects the highest priority available at its claim instant. The combined
// per-priority counts must also match the batch exactly.
TEST(Adversarial, BatchedPriorityDrain) {
    constexpr FwSizeType DEPTH = 32;
    constexpr U32 CONSUMERS = 4;
    constexpr U32 BATCHES = 200;
    constexpr U32 NUM_PRIORITIES = MAX_PRIORITIES;

    Os::Queue queue;
    Fw::String name("batched-prio");
    ASSERT_EQ(queue.create(0, name, DEPTH, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

    for (U32 batch = 0; batch < BATCHES; batch++) {
        // Publish a full batch of shuffled priorities with no consumer running.
        U32 sentByPriority[NUM_PRIORITIES] = {0};
        for (U32 i = 0; i < static_cast<U32>(DEPTH); i++) {
            U8 buffer[sizeof(U32)] = {0};
            pack_u32(i, buffer);
            const FwQueuePriorityType priority = static_cast<FwQueuePriorityType>(((i * 5) + batch) % NUM_PRIORITIES);
            sentByPriority[priority]++;
            ASSERT_EQ(queue.send(buffer, sizeof(U32), priority, Os::QueueInterface::BlockingType::NONBLOCKING),
                      Os::QueueInterface::Status::OP_OK);
        }

        std::atomic<U32> claimed(0);
        std::atomic<U32> receivedByPriority[NUM_PRIORITIES];
        for (U32 i = 0; i < NUM_PRIORITIES; i++) {
            receivedByPriority[i].store(0, std::memory_order_relaxed);
        }
        std::atomic<U32> readyCount(0);
        std::atomic<bool> go(false);

        std::thread consumers[CONSUMERS];
        for (U32 t = 0; t < CONSUMERS; t++) {
            consumers[t] = std::thread([&queue, &claimed, &receivedByPriority, &readyCount, &go, batch, DEPTH]() {
                // Barrier: wait until every consumer is ready so the drain is contended.
                readyCount.fetch_add(1, std::memory_order_acq_rel);
                while (!go.load(std::memory_order_acquire)) {
                    std::this_thread::yield();
                }
                U8 buffer[sizeof(U32)] = {0};
                FwSizeType actualSize = 0;
                FwQueuePriorityType priority = 0;
                FwQueuePriorityType lastPriority = 0;
                bool havePrevious = false;
                while (claimed.load(std::memory_order_acquire) < static_cast<U32>(DEPTH)) {
                    Os::QueueInterface::Status status = queue.receive(
                        buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
                    if (status == Os::QueueInterface::Status::OP_OK) {
                        // The drained set only shrinks, so this consumer's own receive
                        // sequence must be non-increasing in priority.
                        if (havePrevious) {
                            ASSERT_LE(priority, lastPriority) << "batch=" << batch;
                        }
                        lastPriority = priority;
                        havePrevious = true;
                        receivedByPriority[priority].fetch_add(1, std::memory_order_acq_rel);
                        claimed.fetch_add(1, std::memory_order_acq_rel);
                    } else {
                        std::this_thread::yield();
                    }
                }
            });
        }

        while (readyCount.load(std::memory_order_acquire) < CONSUMERS) {
            std::this_thread::yield();
        }
        go.store(true, std::memory_order_release);
        for (U32 t = 0; t < CONSUMERS; t++) {
            consumers[t].join();
        }

        ASSERT_EQ(claimed.load(std::memory_order_acquire), static_cast<U32>(DEPTH));
        for (U32 p = 0; p < NUM_PRIORITIES; p++) {
            EXPECT_EQ(receivedByPriority[p].load(std::memory_order_relaxed), sentByPriority[p])
                << "batch=" << batch << " priority=" << p;
        }
        ASSERT_EQ(queue.getMessagesAvailable(), 0u);
    }
    queue.teardown();
}

}  // namespace

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
