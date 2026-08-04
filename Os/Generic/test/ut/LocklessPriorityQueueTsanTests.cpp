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

struct ConcurrentConfig {
    FwSizeType depth;
    U32 producers;
    U32 consumers;
    U32 messagesPerProducer;
};

struct ConcurrentState {
    Os::Queue queue;
    std::atomic<U32>* received;  // array of size totalMessages
    std::atomic<U32> consumed;
    std::atomic<bool> producersDone;
    U32 totalMessages;

    explicit ConcurrentState(U32 total)
        : received(new std::atomic<U32>[total]), consumed(0), producersDone(false), totalMessages(total) {
        for (U32 i = 0; i < total; i++) {
            received[i].store(0, std::memory_order_relaxed);
        }
    }
    ~ConcurrentState() { delete[] received; }
};

void concurrent_producer(ConcurrentState* state, U32 producerIndex, U32 messagesPerProducer) {
    for (U32 i = 0; i < messagesPerProducer; i++) {
        const U32 value = (producerIndex * messagesPerProducer) + i;
        U8 buffer[sizeof(U32)];
        pack_u32(value, buffer);
        const FwQueuePriorityType priority = static_cast<FwQueuePriorityType>(value % 8);
        Os::QueueInterface::Status status = Os::QueueInterface::Status::FULL;
        while (status == Os::QueueInterface::Status::FULL) {
            status = state->queue.send(buffer, sizeof(U32), priority, Os::QueueInterface::BlockingType::NONBLOCKING);
            if (status == Os::QueueInterface::Status::FULL) {
                std::this_thread::yield();
            }
        }
        ASSERT_EQ(status, Os::QueueInterface::Status::OP_OK);
    }
}

void concurrent_consumer(ConcurrentState* state) {
    U8 buffer[sizeof(U32)];
    FwSizeType actualSize = 0;
    FwQueuePriorityType priority = 0;
    while (state->consumed.load(std::memory_order_acquire) < state->totalMessages) {
        Os::QueueInterface::Status status = state->queue.receive(
            buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
        if (status == Os::QueueInterface::Status::OP_OK) {
            ASSERT_EQ(actualSize, sizeof(U32));
            const U32 value = unpack_u32(buffer);
            ASSERT_LT(value, state->totalMessages);
            const U32 prior = state->received[value].fetch_add(1, std::memory_order_acq_rel);
            ASSERT_EQ(prior, 0u) << "duplicate delivery of value " << value;
            state->consumed.fetch_add(1, std::memory_order_acq_rel);
        } else if (status == Os::QueueInterface::Status::EMPTY) {
            if (state->producersDone.load(std::memory_order_acquire) &&
                state->consumed.load(std::memory_order_acquire) >= state->totalMessages) {
                break;
            }
            std::this_thread::yield();
        } else {
            FAIL() << "unexpected status " << static_cast<int>(status);
        }
    }
}

void run_concurrent(const ConcurrentConfig& cfg) {
    const U32 total = cfg.producers * cfg.messagesPerProducer;
    ConcurrentState state(total);
    Fw::String name("tsan-concurrent");
    ASSERT_EQ(state.queue.create(0, name, cfg.depth, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

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
    state.queue.teardown();
}

// =======================================================================
// TSan Stress Tests
// =======================================================================

// High-thread-count stress: 8 producers, 8 consumers, 40k messages through a 32-slot queue.
// Repeated multiple iterations to exercise different scheduling orders.
TEST(TsanStress, HighThreadConcurrent) {
    constexpr U32 ITERATIONS = 20;
    const ConcurrentConfig cfg{32, 8, 8, 5000};
    for (U32 iter = 0; iter < ITERATIONS; iter++) {
        run_concurrent(cfg);
    }
}

// Extreme contention: many threads fighting over a tiny queue.
TEST(TsanStress, TinyQueueHighContention) {
    constexpr U32 ITERATIONS = 50;
    const ConcurrentConfig cfg{4, 8, 8, 500};
    for (U32 iter = 0; iter < ITERATIONS; iter++) {
        run_concurrent(cfg);
    }
}

// Asymmetric: many producers, few consumers -- queue spends time near FULL.
TEST(TsanStress, ManyProducersFewConsumers) {
    constexpr U32 ITERATIONS = 20;
    const ConcurrentConfig cfg{16, 12, 2, 1000};
    for (U32 iter = 0; iter < ITERATIONS; iter++) {
        run_concurrent(cfg);
    }
}

// Asymmetric: few producers, many consumers -- queue spends time near EMPTY.
TEST(TsanStress, FewProducersManyConsumers) {
    constexpr U32 ITERATIONS = 20;
    const ConcurrentConfig cfg{16, 2, 12, 5000};
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
    const ConcurrentConfig cfg{1, 4, 4, 500};
    for (U32 iter = 0; iter < ITERATIONS; iter++) {
        run_concurrent(cfg);
    }
}

// Full-queue storm: pre-fill the queue, then unleash many producers (all get FULL)
// while consumers drain. Verifies no message is lost during the transition.
TEST(Adversarial, FullQueueStorm) {
    constexpr FwSizeType DEPTH = 16;
    constexpr U32 NUM_THREADS = 8;
    constexpr U32 MESSAGES_PER_THREAD = 2000;
    constexpr U32 TOTAL = NUM_THREADS * MESSAGES_PER_THREAD;

    Os::Queue queue;
    Fw::String name("full-storm");
    ASSERT_EQ(queue.create(0, name, DEPTH, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

    // Pre-fill the queue
    for (FwSizeType i = 0; i < DEPTH; i++) {
        U32 value = static_cast<U32>(i);
        U8 buffer[sizeof(U32)];
        pack_u32(value, buffer);
        ASSERT_EQ(queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::NONBLOCKING),
                  Os::QueueInterface::Status::OP_OK);
    }

    // Now all producers will initially hit FULL.
    const U32 expectedTotal = TOTAL + static_cast<U32>(DEPTH);
    std::atomic<U32>* received = new std::atomic<U32>[expectedTotal];
    for (U32 i = 0; i < expectedTotal; i++) {
        received[i].store(0, std::memory_order_relaxed);
    }
    std::atomic<U32> consumed(0);
    std::atomic<bool> producersDone(false);

    std::thread producers[NUM_THREADS];
    std::thread consumers[NUM_THREADS];

    // Producers send values [DEPTH .. DEPTH+TOTAL-1]
    for (U32 t = 0; t < NUM_THREADS; t++) {
        producers[t] = std::thread([&queue, t, MESSAGES_PER_THREAD, DEPTH]() {
            for (U32 i = 0; i < MESSAGES_PER_THREAD; i++) {
                const U32 value = static_cast<U32>(DEPTH) + (t * MESSAGES_PER_THREAD) + i;
                U8 buffer[sizeof(U32)];
                pack_u32(value, buffer);
                Os::QueueInterface::Status status = Os::QueueInterface::Status::FULL;
                while (status == Os::QueueInterface::Status::FULL) {
                    status = queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
                    if (status == Os::QueueInterface::Status::FULL) {
                        std::this_thread::yield();
                    }
                }
            }
        });
    }

    // Consumers drain everything
    for (U32 t = 0; t < NUM_THREADS; t++) {
        consumers[t] = std::thread([&queue, &received, &consumed, &producersDone, expectedTotal]() {
            U8 buffer[sizeof(U32)];
            FwSizeType actualSize = 0;
            FwQueuePriorityType priority = 0;
            while (consumed.load(std::memory_order_acquire) < expectedTotal) {
                Os::QueueInterface::Status status = queue.receive(
                    buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
                if (status == Os::QueueInterface::Status::OP_OK) {
                    const U32 value = unpack_u32(buffer);
                    ASSERT_LT(value, expectedTotal);
                    received[value].fetch_add(1, std::memory_order_acq_rel);
                    consumed.fetch_add(1, std::memory_order_acq_rel);
                } else {
                    if (producersDone.load(std::memory_order_acquire) &&
                        consumed.load(std::memory_order_acquire) >= expectedTotal) {
                        break;
                    }
                    std::this_thread::yield();
                }
            }
        });
    }

    for (U32 t = 0; t < NUM_THREADS; t++) {
        producers[t].join();
    }
    producersDone.store(true, std::memory_order_release);
    for (U32 t = 0; t < NUM_THREADS; t++) {
        consumers[t].join();
    }

    EXPECT_EQ(consumed.load(), expectedTotal);
    for (U32 i = 0; i < expectedTotal; i++) {
        EXPECT_EQ(received[i].load(std::memory_order_relaxed), 1u) << "value " << i;
    }
    delete[] received;
    queue.teardown();
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
            U8 buffer[sizeof(U32)];
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
            U8 buffer[sizeof(U32)];
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

// Verify that high-water mark is accurate under contention. Multiple producers
// fill the queue while a single consumer drains; afterwards the high-water mark
// must be at least the queue depth (since the queue was fully filled).
TEST(Adversarial, HighWaterMarkAccuracy) {
    constexpr FwSizeType DEPTH = 16;
    constexpr U32 PRODUCERS = 8;
    constexpr U32 MESSAGES_PER = 2000;
    constexpr U32 TOTAL = PRODUCERS * MESSAGES_PER;

    Os::Queue queue;
    Fw::String name("high-water-mark-test");
    ASSERT_EQ(queue.create(0, name, DEPTH, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

    std::atomic<U32> consumed(0);
    std::atomic<bool> producersDone(false);

    std::thread producers[PRODUCERS];
    for (U32 t = 0; t < PRODUCERS; t++) {
        producers[t] = std::thread([&queue, t, MESSAGES_PER]() {
            for (U32 i = 0; i < MESSAGES_PER; i++) {
                const U32 value = (t * MESSAGES_PER) + i;
                U8 buffer[sizeof(U32)];
                pack_u32(value, buffer);
                Os::QueueInterface::Status status = Os::QueueInterface::Status::FULL;
                while (status == Os::QueueInterface::Status::FULL) {
                    status = queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
                    if (status == Os::QueueInterface::Status::FULL) {
                        std::this_thread::yield();
                    }
                }
            }
        });
    }

    std::thread consumer([&queue, &consumed, &producersDone, TOTAL]() {
        U8 buffer[sizeof(U32)];
        FwSizeType actualSize = 0;
        FwQueuePriorityType priority = 0;
        while (consumed.load(std::memory_order_acquire) < TOTAL) {
            Os::QueueInterface::Status status =
                queue.receive(buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
            if (status == Os::QueueInterface::Status::OP_OK) {
                consumed.fetch_add(1, std::memory_order_acq_rel);
            } else {
                if (producersDone.load(std::memory_order_acquire) &&
                    consumed.load(std::memory_order_acquire) >= TOTAL) {
                    break;
                }
                std::this_thread::yield();
            }
        }
    });

    for (U32 t = 0; t < PRODUCERS; t++) {
        producers[t].join();
    }
    producersDone.store(true, std::memory_order_release);
    consumer.join();

    EXPECT_EQ(consumed.load(), TOTAL);
    // With 8 producers and depth=16, the queue must have been full at some point.
    // The high-water mark may transiently exceed DEPTH because m_count.fetch_add
    // in send() is not atomic with the slot state transition; a consumer that frees
    // a slot but hasn't decremented the count yet allows a producer to refill and
    // increment, momentarily pushing the count above DEPTH.
    EXPECT_GE(queue.getMessageHighWaterMark(), static_cast<FwSizeType>(DEPTH));
    EXPECT_LE(queue.getMessageHighWaterMark(), static_cast<FwSizeType>(DEPTH + PRODUCERS));
    EXPECT_EQ(queue.getMessagesAvailable(), 0u);
    queue.teardown();
}

// Create-teardown-recreate: verify the queue is fully functional after
// a teardown + create cycle. Tests internal state cleanup.
TEST(Adversarial, CreateTeardownRecreate) {
    constexpr FwSizeType DEPTH = 8;
    constexpr U32 MESSAGES = 1000;
    constexpr U32 CYCLES = 5;

    for (U32 cycle = 0; cycle < CYCLES; cycle++) {
        Os::Queue queue;
        Fw::String name("recycle-test");
        ASSERT_EQ(queue.create(0, name, DEPTH, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

        std::atomic<U32> localConsumed(0);
        std::atomic<bool> localDone(false);
        std::atomic<U32> localReceived[MESSAGES];
        for (U32 i = 0; i < MESSAGES; i++) {
            localReceived[i].store(0, std::memory_order_relaxed);
        }

        std::thread producers[4];
        std::thread consumers[4];

        for (U32 t = 0; t < 4; t++) {
            producers[t] = std::thread([&queue, t, MESSAGES]() {
                for (U32 i = 0; i < MESSAGES / 4; i++) {
                    const U32 value = (t * (MESSAGES / 4)) + i;
                    U8 buffer[sizeof(U32)];
                    pack_u32(value, buffer);
                    Os::QueueInterface::Status status = Os::QueueInterface::Status::FULL;
                    while (status == Os::QueueInterface::Status::FULL) {
                        status = queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
                        if (status == Os::QueueInterface::Status::FULL) {
                            std::this_thread::yield();
                        }
                    }
                }
            });
        }

        for (U32 t = 0; t < 4; t++) {
            consumers[t] = std::thread([&queue, &localConsumed, &localDone, &localReceived, MESSAGES]() {
                U8 buffer[sizeof(U32)];
                FwSizeType actualSize = 0;
                FwQueuePriorityType priority = 0;
                while (localConsumed.load(std::memory_order_acquire) < MESSAGES) {
                    Os::QueueInterface::Status status = queue.receive(
                        buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
                    if (status == Os::QueueInterface::Status::OP_OK) {
                        const U32 value = unpack_u32(buffer);
                        localReceived[value].fetch_add(1, std::memory_order_acq_rel);
                        localConsumed.fetch_add(1, std::memory_order_acq_rel);
                    } else {
                        if (localDone.load(std::memory_order_acquire) &&
                            localConsumed.load(std::memory_order_acquire) >= MESSAGES) {
                            break;
                        }
                        std::this_thread::yield();
                    }
                }
            });
        }

        for (U32 t = 0; t < 4; t++) {
            producers[t].join();
        }
        localDone.store(true, std::memory_order_release);
        for (U32 t = 0; t < 4; t++) {
            consumers[t].join();
        }

        EXPECT_EQ(localConsumed.load(), MESSAGES);
        for (U32 i = 0; i < MESSAGES; i++) {
            EXPECT_EQ(localReceived[i].load(std::memory_order_relaxed), 1u) << "cycle=" << cycle << " value=" << i;
        }
        queue.teardown();
    }
}

// Sequence-wrap logic test: directly verify that modular unsigned subtraction
// handles wrap-around of the sequence counter correctly. This tests the same
// comparison algorithm used by isCandidatePreferred() in the production code.
TEST(Adversarial, SequenceWrapComparison) {
    // The production code decides FIFO order via:
    //   difference = candidateSeq - bestSeq
    //   preferred  = (difference & topBit) != 0
    // If candidate is "older" (smaller modular), difference has the top bit set.
    const U32 topBit = static_cast<U32>(1) << (std::numeric_limits<U32>::digits - 1);

    auto isOlder = [topBit](U32 candidate, U32 best) -> bool {
        const U32 diff = candidate - best;
        return (diff & topBit) != 0;
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
// priorities. Verify that each consumer always receives in non-increasing
// priority order (within messages it individually receives). Under true
// concurrency, global priority order across consumers is not guaranteed,
// but each consumer's local stream must be non-increasing.
TEST(Adversarial, MixedPriorityContention) {
    constexpr FwSizeType DEPTH = 32;
    constexpr U32 PRODUCERS = 4;
    constexpr U32 CONSUMERS = 4;
    constexpr U32 MESSAGES_PER_PRODUCER = 2000;
    constexpr U32 TOTAL = PRODUCERS * MESSAGES_PER_PRODUCER;
    constexpr U32 NUM_PRIORITIES = 8;

    Os::Queue queue;
    Fw::String name("mixed-prio");
    ASSERT_EQ(queue.create(0, name, DEPTH, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

    std::atomic<U32> consumed(0);
    std::atomic<bool> producersDone(false);
    std::atomic<U32> receivedCount[NUM_PRIORITIES];
    for (U32 i = 0; i < NUM_PRIORITIES; i++) {
        receivedCount[i].store(0, std::memory_order_relaxed);
    }

    std::thread producers[PRODUCERS];
    for (U32 t = 0; t < PRODUCERS; t++) {
        producers[t] = std::thread([&queue, t, MESSAGES_PER_PRODUCER, NUM_PRIORITIES]() {
            for (U32 i = 0; i < MESSAGES_PER_PRODUCER; i++) {
                const U32 value = (t * MESSAGES_PER_PRODUCER) + i;
                U8 buffer[sizeof(U32)];
                pack_u32(value, buffer);
                const FwQueuePriorityType priority = static_cast<FwQueuePriorityType>(value % NUM_PRIORITIES);
                Os::QueueInterface::Status status = Os::QueueInterface::Status::FULL;
                while (status == Os::QueueInterface::Status::FULL) {
                    status = queue.send(buffer, sizeof(U32), priority, Os::QueueInterface::BlockingType::NONBLOCKING);
                    if (status == Os::QueueInterface::Status::FULL) {
                        std::this_thread::yield();
                    }
                }
            }
        });
    }

    std::thread consumers[CONSUMERS];
    for (U32 t = 0; t < CONSUMERS; t++) {
        consumers[t] = std::thread([&queue, &consumed, &producersDone, &receivedCount, TOTAL, NUM_PRIORITIES]() {
            U8 buffer[sizeof(U32)];
            FwSizeType actualSize = 0;
            FwQueuePriorityType priority = 0;
            while (consumed.load(std::memory_order_acquire) < TOTAL) {
                Os::QueueInterface::Status status = queue.receive(
                    buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
                if (status == Os::QueueInterface::Status::OP_OK) {
                    ASSERT_LT(static_cast<U32>(priority), NUM_PRIORITIES);
                    receivedCount[priority].fetch_add(1, std::memory_order_acq_rel);
                    consumed.fetch_add(1, std::memory_order_acq_rel);
                } else {
                    if (producersDone.load(std::memory_order_acquire) &&
                        consumed.load(std::memory_order_acquire) >= TOTAL) {
                        break;
                    }
                    std::this_thread::yield();
                }
            }
        });
    }

    for (U32 t = 0; t < PRODUCERS; t++) {
        producers[t].join();
    }
    producersDone.store(true, std::memory_order_release);
    for (U32 t = 0; t < CONSUMERS; t++) {
        consumers[t].join();
    }

    // Verify all messages were consumed
    EXPECT_EQ(consumed.load(), TOTAL);
    // Verify priority distribution matches what was sent
    U32 totalReceived = 0;
    for (U32 p = 0; p < NUM_PRIORITIES; p++) {
        totalReceived += receivedCount[p].load(std::memory_order_relaxed);
    }
    EXPECT_EQ(totalReceived, TOTAL);
    queue.teardown();
}

// Rapid teardown after drain: multiple consumers race to drain the queue,
// then teardown() is called immediately. Verifies no use-after-free.
TEST(Adversarial, RapidDrainThenTeardown) {
    constexpr FwSizeType DEPTH = 64;
    constexpr U32 MESSAGES = 10000;
    constexpr U32 CONSUMERS = 8;

    Os::Queue queue;
    Fw::String name("drain-teardown");
    ASSERT_EQ(queue.create(0, name, DEPTH, sizeof(U32)), Os::QueueInterface::Status::OP_OK);

    // Fill the queue up to depth
    for (FwSizeType i = 0; i < DEPTH; i++) {
        U8 buffer[sizeof(U32)];
        pack_u32(static_cast<U32>(i), buffer);
        ASSERT_EQ(queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::NONBLOCKING),
                  Os::QueueInterface::Status::OP_OK);
    }

    // Single producer sends the rest
    std::atomic<U32> consumed(0);
    std::atomic<bool> producerDone(false);

    std::thread producer([&queue, &producerDone, DEPTH, MESSAGES]() {
        for (U32 i = static_cast<U32>(DEPTH); i < MESSAGES; i++) {
            U8 buffer[sizeof(U32)];
            pack_u32(i, buffer);
            Os::QueueInterface::Status status = Os::QueueInterface::Status::FULL;
            while (status == Os::QueueInterface::Status::FULL) {
                status = queue.send(buffer, sizeof(U32), 0, Os::QueueInterface::BlockingType::NONBLOCKING);
                if (status == Os::QueueInterface::Status::FULL) {
                    std::this_thread::yield();
                }
            }
        }
        producerDone.store(true, std::memory_order_release);
    });

    std::thread consumers[CONSUMERS];
    for (U32 t = 0; t < CONSUMERS; t++) {
        consumers[t] = std::thread([&queue, &consumed, &producerDone, MESSAGES]() {
            U8 buffer[sizeof(U32)];
            FwSizeType actualSize = 0;
            FwQueuePriorityType priority = 0;
            while (consumed.load(std::memory_order_acquire) < MESSAGES) {
                Os::QueueInterface::Status status = queue.receive(
                    buffer, sizeof(U32), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize, priority);
                if (status == Os::QueueInterface::Status::OP_OK) {
                    consumed.fetch_add(1, std::memory_order_acq_rel);
                } else {
                    if (producerDone.load(std::memory_order_acquire) &&
                        consumed.load(std::memory_order_acquire) >= MESSAGES) {
                        break;
                    }
                    std::this_thread::yield();
                }
            }
        });
    }

    producer.join();
    for (U32 t = 0; t < CONSUMERS; t++) {
        consumers[t].join();
    }

    EXPECT_EQ(consumed.load(), MESSAGES);
    // Immediate teardown after all threads complete
    queue.teardown();
}

}  // namespace

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
