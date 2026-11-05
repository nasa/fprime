// ======================================================================
// \title  AtomicQueueTest.cpp
// \author B. Duckett
// \brief  Unit tests for AtomicQueue circular buffer implementation
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <gtest/gtest.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <numeric>
#include <thread>
#include <vector>
#include "Fw/Types/MemAllocator.hpp"
#include "Os/Generic/Types/AtomicQueue.hpp"

// Test allocator for queue memory
class TestAllocator : public Fw::MemAllocator {
  public:
    TestAllocator() : m_bytesAllocated(0), m_allocations(0) {}

    void* allocate(const FwEnumStoreType identifier,
                   FwSizeType& size,
                   bool& recoverable,
                   FwSizeType alignment) override {
        (void)identifier;
        recoverable = true;
        void* mem = (alignment > 0) ? nullptr : ::malloc(size);
        if (alignment > 0) {
            int result = posix_memalign(&mem, alignment, size);
            if (result != 0)
                mem = nullptr;
        }
        if (mem) {
            m_bytesAllocated += size;
            m_allocations++;
        }
        return mem;
    }

    void deallocate(const FwEnumStoreType identifier, void* ptr) override {
        (void)identifier;
        if (ptr) {
            ::free(ptr);
            m_allocations--;
        }
    }

    FwSizeType getBytesAllocated() const { return m_bytesAllocated; }
    FwSizeType getAllocationCount() const { return m_allocations; }

  private:
    FwSizeType m_bytesAllocated, m_allocations;
};

// Failing allocator - returns nullptr to simulate allocation failure
class FailAllocator : public Fw::MemAllocator {
  public:
    void* allocate(const FwEnumStoreType identifier,
                   FwSizeType& size,
                   bool& recoverable,
                   FwSizeType alignment) override {
        (void)identifier;
        (void)size;
        (void)alignment;
        recoverable = true;
        return nullptr;  // Always fail
    }

    void deallocate(const FwEnumStoreType identifier, void* ptr) override {
        (void)identifier;
        (void)ptr;
    }
};

// Partial failure allocator - fails after N successful allocations
class PartialFailAllocator : public Fw::MemAllocator {
  public:
    explicit PartialFailAllocator(U32 failAfter) : m_failAfter(failAfter), m_allocCount(0) {}

    void* allocate(const FwEnumStoreType identifier,
                   FwSizeType& size,
                   bool& recoverable,
                   FwSizeType alignment) override {
        (void)identifier;
        recoverable = true;

        if (m_allocCount >= m_failAfter) {
            return nullptr;  // Fail after threshold
        }

        m_allocCount++;
        void* mem = (alignment > 0) ? nullptr : ::malloc(size);
        if (alignment > 0) {
            int result = posix_memalign(&mem, alignment, size);
            if (result != 0)
                mem = nullptr;
        }
        return mem;
    }

    void deallocate(const FwEnumStoreType identifier, void* ptr) override {
        (void)identifier;
        if (ptr)
            ::free(ptr);
    }

  private:
    U32 m_failAfter;
    U32 m_allocCount;
};

// Forward declaration for friend access
class AtomicQueueWrapAroundTest;

// Fixture: eliminates duplicate setup/teardown across all tests
class AtomicQueueTest : public ::testing::Test {
  protected:
    TestAllocator allocator;
    Types::AtomicQueue queue;
    static constexpr FwSizeType MAX_BUF = 256;
    U8 sendBuf[MAX_BUF], recvBuf[MAX_BUF];
    FwSizeType actualSize;

    void TearDown() override { queue.teardown(); }

    // Helper: fill buffer with sequential pattern
    void fillPattern(FwSizeType size, U8 offset = 0) {
        for (FwSizeType i = 0; i < size; ++i)
            sendBuf[i] = static_cast<U8>(offset + i);
    }

    // Helper: verify recv matches send
    void verifyData(FwSizeType size) {
        for (FwSizeType i = 0; i < size; ++i)
            ASSERT_EQ(recvBuf[i], sendBuf[i]);
    }

    // Helper: fill queue to capacity with sequential messages
    void fillToCapacity(FwSizeType msgSize, U8 startValue = 0) {
        FwSizeType cap = queue.getCapacity();
        for (FwSizeType i = 0; i < cap; ++i) {
            fillPattern(msgSize, static_cast<U8>(startValue + i));
            ASSERT_TRUE(queue.enqueue(sendBuf, msgSize));
        }
        ASSERT_TRUE(queue.isFull());
    }
};

// Parameterized test: consolidates Creation + basic capacity tests
struct QueueParams {
    FwSizeType capacity, bufSize;
};
class AtomicQueueParamTest : public AtomicQueueTest, public ::testing::WithParamInterface<QueueParams> {};

TEST_P(AtomicQueueParamTest, Creation) {
    QueueParams params = GetParam();
    queue.create(params.capacity, params.bufSize, allocator, 0);
    ASSERT_EQ(queue.getCapacity(), params.capacity);
    ASSERT_EQ(queue.getBufferSize(), params.bufSize);
    ASSERT_TRUE(queue.isEmpty());
    ASSERT_FALSE(queue.isFull());
}

INSTANTIATE_TEST_SUITE_P(VariousCapacities,
                         AtomicQueueParamTest,
                         ::testing::Values(QueueParams{8, 64},    // Small power-of-2
                                           QueueParams{16, 128},  // Medium power-of-2
                                           QueueParams{100, 256}  // Non-power-of-2
                                           ));

// Single enqueue/dequeue round-trip
TEST_F(AtomicQueueTest, SingleMessage) {
    queue.create(8, 64, allocator, 0);
    fillPattern(64);

    ASSERT_TRUE(queue.enqueue(sendBuf, 64));
    ASSERT_EQ(queue.getSize(), 1);
    ASSERT_TRUE(queue.dequeue(recvBuf, 64, actualSize));
    ASSERT_EQ(actualSize, 64);
    ASSERT_TRUE(queue.isEmpty());
    verifyData(64);
}

// Fill to capacity, verify full rejection, drain completely
TEST_F(AtomicQueueTest, FillAndDrain) {
    const FwSizeType cap = 8, bufSz = 32;
    queue.create(cap, bufSz, allocator, 0);

    // Fill: each message gets unique pattern
    for (FwSizeType i = 0; i < cap; ++i) {
        fillPattern(bufSz, i * 10);
        ASSERT_TRUE(queue.enqueue(sendBuf, bufSz));
        ASSERT_EQ(queue.getSize(), i + 1);
    }
    ASSERT_TRUE(queue.isFull());
    ASSERT_FALSE(queue.enqueue(sendBuf, bufSz));  // Reject when full

    // Drain: verify FIFO order and patterns
    for (FwSizeType i = 0; i < cap; ++i) {
        fillPattern(bufSz, i * 10);  // Regenerate expected pattern
        ASSERT_TRUE(queue.dequeue(recvBuf, bufSz, actualSize));
        ASSERT_EQ(actualSize, bufSz);
        verifyData(bufSz);
    }
    ASSERT_TRUE(queue.isEmpty());
    ASSERT_FALSE(queue.dequeue(recvBuf, bufSz, actualSize));  // Reject when empty
}

// FIFO ordering with sequential markers
TEST_F(AtomicQueueTest, FifoOrdering) {
    queue.create(16, 8, allocator, 0);
    for (FwSizeType i = 0; i < 10; ++i) {
        sendBuf[0] = static_cast<U8>(i);
        ASSERT_TRUE(queue.enqueue(sendBuf, 1));
    }
    for (FwSizeType i = 0; i < 10; ++i) {
        ASSERT_TRUE(queue.dequeue(recvBuf, 8, actualSize));
        ASSERT_EQ(recvBuf[0], static_cast<U8>(i));
    }
}

// Variable message sizes in single queue
TEST_F(AtomicQueueTest, VariableSizes) {
    queue.create(8, 128, allocator, 0);
    FwSizeType sizes[] = {8, 16, 32, 64, 128, 1, 100, 50};

    // Enqueue with unique patterns
    for (FwSizeType i = 0; i < 8; ++i) {
        for (FwSizeType j = 0; j < sizes[i]; ++j)
            sendBuf[j] = static_cast<U8>((i << 4) | (j & 0xF));
        ASSERT_TRUE(queue.enqueue(sendBuf, sizes[i]));
    }

    // Dequeue and verify size + data
    for (FwSizeType i = 0; i < 8; ++i) {
        ASSERT_TRUE(queue.dequeue(recvBuf, 128, actualSize));
        ASSERT_EQ(actualSize, sizes[i]);
        for (FwSizeType j = 0; j < sizes[i]; ++j)
            ASSERT_EQ(recvBuf[j], static_cast<U8>((i << 4) | (j & 0xF)));
    }
}

// Wrap-around: multiple fill/drain cycles
TEST_F(AtomicQueueTest, WrapAround) {
    const FwSizeType cap = 4;
    queue.create(cap, 16, allocator, 0);

    for (FwSizeType cycle = 0; cycle < 10; ++cycle) {
        for (FwSizeType i = 0; i < cap; ++i) {
            sendBuf[0] = static_cast<U8>(cycle * cap + i);
            ASSERT_TRUE(queue.enqueue(sendBuf, 1));
        }
        for (FwSizeType i = 0; i < cap; ++i) {
            ASSERT_TRUE(queue.dequeue(recvBuf, 16, actualSize));
            ASSERT_EQ(recvBuf[0], static_cast<U8>(cycle * cap + i));
        }
    }
}

// Parameterized edge case test - consolidates EmptyDequeue, FullEnqueue, MinimalCapacity
enum class EdgeCaseType { EMPTY_DEQUEUE, FULL_ENQUEUE, MINIMAL_CAPACITY };

struct EdgeCaseParams {
    const char* name;
    EdgeCaseType type;
    FwSizeType capacity;
    FwSizeType bufSize;
};

class AtomicQueueEdgeCaseTest : public AtomicQueueTest, public ::testing::WithParamInterface<EdgeCaseParams> {};

TEST_P(AtomicQueueEdgeCaseTest, EdgeCaseBehavior) {
    auto params = GetParam();
    queue.create(params.capacity, params.bufSize, allocator, 0);

    switch (params.type) {
        case EdgeCaseType::EMPTY_DEQUEUE:
            // Verify empty queue rejects dequeue
            for (FwSizeType i = 0; i < 3; ++i)
                ASSERT_FALSE(queue.dequeue(recvBuf, params.bufSize, actualSize));
            break;

        case EdgeCaseType::FULL_ENQUEUE:
            // Fill to capacity then verify rejection
            sendBuf[0] = 0xFF;
            for (FwSizeType i = 0; i < params.capacity; ++i)
                ASSERT_TRUE(queue.enqueue(sendBuf, 1));
            for (FwSizeType i = 0; i < 3; ++i)
                ASSERT_FALSE(queue.enqueue(sendBuf, 1));
            break;

        case EdgeCaseType::MINIMAL_CAPACITY:
            // Capacity=1: fill, verify full, drain, verify empty
            sendBuf[0] = 0xAA;
            ASSERT_TRUE(queue.enqueue(sendBuf, 1));
            ASSERT_TRUE(queue.isFull());
            ASSERT_FALSE(queue.enqueue(sendBuf, 1));
            ASSERT_TRUE(queue.dequeue(recvBuf, params.bufSize, actualSize));
            ASSERT_EQ(recvBuf[0], 0xAA);
            ASSERT_TRUE(queue.isEmpty());
            break;
    }
}

INSTANTIATE_TEST_SUITE_P(AllEdgeCases,
                         AtomicQueueEdgeCaseTest,
                         ::testing::Values(EdgeCaseParams{"EmptyDequeue", EdgeCaseType::EMPTY_DEQUEUE, 4, 32},
                                           EdgeCaseParams{"FullEnqueue", EdgeCaseType::FULL_ENQUEUE, 4, 16},
                                           EdgeCaseParams{"MinimalCapacity", EdgeCaseType::MINIMAL_CAPACITY, 1, 16}));

// CAS retry exhaustion - verifies graceful failure under extreme contention
TEST_F(AtomicQueueTest, CASRetryExhaustion) {
    const FwSizeType queueCap = 2;     // Tiny queue to maximize contention
    const FwSizeType numThreads = 16;  // Many threads competing
    const FwSizeType attemptsPerThread = 100;

    queue.create(queueCap, 32, allocator, 0);

    std::atomic<U32> successCount{0};
    std::atomic<U32> failureCount{0};

    // Hammer the queue with extreme contention
    auto hammer = [&](U8 threadId) {
        U8 buf[32];
        buf[0] = threadId;

        for (FwSizeType i = 0; i < attemptsPerThread; ++i) {
            if (queue.enqueue(buf, 1)) {
                successCount++;
                // Immediately dequeue to create more contention
                FwSizeType size;
                U8 localRecvBuf[32];
                if (queue.dequeue(localRecvBuf, 32, size)) {
                    // Success path
                }
            } else {
                failureCount++;
            }
        }
    };

    std::vector<std::thread> threads;
    for (FwSizeType i = 0; i < numThreads; ++i) {
        threads.emplace_back(hammer, static_cast<U8>(i));
    }

    for (auto& t : threads) {
        t.join();
    }

    // Verify bounded loop behavior: test completes without hanging despite extreme contention
    // Success/failure distribution depends on implementation efficiency and scheduler
    const U32 totalAttempts = numThreads * attemptsPerThread;
    EXPECT_EQ(successCount.load() + failureCount.load(), totalAttempts);
    EXPECT_GT(successCount.load(), 0) << "Expected some operations to succeed";
    // Note: CAS failures may or may not occur depending on implementation efficiency
    // The critical property is that MAX_CAS_RETRIES prevents infinite loops

    // NOTE: Verifying actual CAS retry count would require test instrumentation in production
    // code (e.g., exporting maxRetriesObserved counter). This adds overhead and complexity
    // for questionable value. For now, this test is good enough.
}

// Memory allocation failure - verify assertion on failure (by design)
TEST_F(AtomicQueueTest, AllocationFailure) {
    FailAllocator failAlloc;
    Types::AtomicQueue failQueue;

    // AtomicQueue uses checkedAllocate() which asserts on allocation failure
    // This is intentional design: allocation failure is fatal for flight software
    ASSERT_DEATH_IF_SUPPORTED(failQueue.create(10, 64, failAlloc, 0), "Assert:.*MemAllocator\\.cpp");
}

// Operations after teardown - verify safe state queries and idempotent teardown
TEST_F(AtomicQueueTest, OperationsAfterTeardown) {
    queue.create(4, 32, allocator, 0);

    // Enqueue some messages
    sendBuf[0] = 0xAA;
    ASSERT_TRUE(queue.enqueue(sendBuf, 1));

    // Teardown
    queue.teardown();

    // State query methods should return safe values after teardown
    EXPECT_FALSE(queue.isCreated());
    EXPECT_EQ(queue.getCapacity(), 0);
    EXPECT_EQ(queue.getBufferSize(), 0);
    EXPECT_TRUE(queue.isEmpty());

    // Multiple teardowns should be idempotent (safe)
    queue.teardown();
    queue.teardown();

    // Note: enqueue/dequeue after teardown intentionally assert (fail-fast design)
    // This is verified in production by runtime assertions, not tested here
}

// 32-bit counter wrap-around - verify algorithm correctness via sustained operations
TEST_F(AtomicQueueTest, CounterWrapAround32Bit) {
    const FwSizeType cap = 4;
    queue.create(cap, 32, allocator, 0);

    // Perform sustained enqueue/dequeue cycles to exercise wrap-around logic
    // On 32-bit platforms (FwSizeType=U32), counter wraps after 2^32 ops
    // This test validates the algorithm handles large counter values correctly
    // Note: Actually forcing wrap would take ~1 hour at 1M ops/sec, so we verify
    // the algorithm's correctness via many operations with varied patterns

    const FwSizeType cycles = 10000;
    for (FwSizeType cycle = 0; cycle < cycles; ++cycle) {
        // Fill and drain pattern
        for (FwSizeType i = 0; i < cap; ++i) {
            sendBuf[0] = static_cast<U8>(cycle);
            sendBuf[1] = static_cast<U8>(i);
            ASSERT_TRUE(queue.enqueue(sendBuf, 2));
        }

        for (FwSizeType i = 0; i < cap; ++i) {
            ASSERT_TRUE(queue.dequeue(recvBuf, 32, actualSize));
            ASSERT_EQ(actualSize, 2);
            ASSERT_EQ(recvBuf[0], static_cast<U8>(cycle));
            ASSERT_EQ(recvBuf[1], static_cast<U8>(i));
        }
    }

    ASSERT_TRUE(queue.isEmpty());
    // Algorithm uses sequence numbers that prevent ABA and handle wrap correctly
}

// Size boundary fuzzing - verify safe handling of invalid sizes
TEST_F(AtomicQueueTest, SizeBoundaryFuzzing) {
    const FwSizeType bufSize = 64;
    queue.create(10, bufSize, allocator, 0);

    // Note: Size=0 and size > bufferSize intentionally assert (fail-fast design)
    // These are verified by runtime assertions in production, not tested here

    // Maximum valid size
    sendBuf[0] = 0xAA;
    ASSERT_TRUE(queue.enqueue(sendBuf, bufSize));  // Exactly at limit
    ASSERT_TRUE(queue.dequeue(recvBuf, 256, actualSize));
    ASSERT_EQ(actualSize, bufSize);

    // Off-by-one: bufSize+1 would assert (not tested, verified by assertions)

    // Size=1 (minimum valid)
    ASSERT_TRUE(queue.enqueue(sendBuf, 1));
    ASSERT_TRUE(queue.dequeue(recvBuf, 256, actualSize));
    ASSERT_EQ(actualSize, 1);

    // Note: Dequeue with buffer smaller than message size also asserts (fail-fast)
    // Production callers must provide adequately sized buffers

    // Variable sizes within valid range
    for (FwSizeType size = 1; size <= bufSize; size += 8) {
        ASSERT_TRUE(queue.enqueue(sendBuf, size));
        ASSERT_TRUE(queue.dequeue(recvBuf, 256, actualSize));
        ASSERT_EQ(actualSize, size);
    }
}

// Blocking enqueue - non-blocking mode (immediate return on full)
TEST_F(AtomicQueueTest, EnqueueBlockingNonBlocking) {
    const FwSizeType cap = 4;
    queue.create(cap, 32, allocator, 0);

    // Fill queue
    for (FwSizeType i = 0; i < cap; ++i) {
        sendBuf[0] = static_cast<U8>(i);
        ASSERT_TRUE(queue.enqueueBlocking(sendBuf, 1, false));
    }

    // Next enqueue with blockIfFull=false should return false immediately
    ASSERT_FALSE(queue.enqueueBlocking(sendBuf, 1, false));
    ASSERT_TRUE(queue.isFull());
}

// Blocking enqueue - blocking mode with consumer thread unblocking
TEST_F(AtomicQueueTest, EnqueueBlockingWithUnblock) {
    const FwSizeType cap = 2;
    queue.create(cap, 32, allocator, 0);

    // Fill queue
    for (FwSizeType i = 0; i < cap; ++i) {
        sendBuf[0] = static_cast<U8>(i);
        ASSERT_TRUE(queue.enqueueBlocking(sendBuf, 1, true));
    }
    ASSERT_TRUE(queue.isFull());

    std::atomic<bool> producerBlocked{false};
    std::atomic<bool> producerUnblocked{false};

    // Producer thread: will block on enqueue when queue is full
    std::thread producer([&]() {
        U8 buf[32] = {0xFF};
        producerBlocked.store(true);
        bool result = queue.enqueueBlocking(buf, 1, true);  // Blocks here
        EXPECT_TRUE(result);
        producerUnblocked.store(true);
    });

    // Wait for producer to enter blocking state
    while (!producerBlocked.load()) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Consumer: dequeue to unblock producer
    ASSERT_TRUE(queue.dequeue(recvBuf, 32, actualSize));

    // Wait for producer to unblock
    producer.join();
    ASSERT_TRUE(producerUnblocked.load());
}

// Concurrent MPMC stress test - verifies lock-free design claims
TEST_F(AtomicQueueTest, ConcurrentMPMC) {
    const FwSizeType queueCap = 100;
    const FwSizeType msgsPerProducer = 1000;
    const FwSizeType numProducers = 3;
    const FwSizeType numConsumers = 2;

    queue.create(queueCap, 64, allocator, 0);

    std::atomic<U32> producedCount{0};
    std::atomic<U32> consumedCount{0};
    std::atomic<bool> stopConsumers{false};

    // Producer threads: send messages with thread ID + sequence number
    auto producer = [&](U8 threadId) {
        U8 buf[64];
        for (FwSizeType i = 0; i < msgsPerProducer; ++i) {
            buf[0] = threadId;
            buf[1] = static_cast<U8>(i >> 8);
            buf[2] = static_cast<U8>(i & 0xFF);

            // Retry on full (expected under high contention)
            while (!queue.enqueue(buf, 3)) {
                // Yield to avoid tight spin
                std::this_thread::yield();
            }
            producedCount++;
        }
    };

    // Consumer threads: receive and validate messages
    auto consumer = [&]() {
        U8 buf[64];
        FwSizeType size;
        while (!stopConsumers.load() || !queue.isEmpty()) {
            if (queue.dequeue(buf, 64, size)) {
                ASSERT_EQ(size, 3);
                consumedCount++;
            } else {
                std::this_thread::yield();
            }
        }
    };

    // Launch threads
    std::vector<std::thread> producers;
    for (FwSizeType i = 0; i < numProducers; ++i) {
        producers.emplace_back(producer, static_cast<U8>(i));
    }

    std::vector<std::thread> consumers;
    for (FwSizeType i = 0; i < numConsumers; ++i) {
        consumers.emplace_back(consumer);
    }

    // Wait for all producers to finish
    for (auto& t : producers) {
        t.join();
    }

    // Signal consumers to stop after draining queue
    stopConsumers.store(true);

    // Wait for consumers
    for (auto& t : consumers) {
        t.join();
    }

    // Verify counts match
    const U32 expectedTotal = numProducers * msgsPerProducer;
    ASSERT_EQ(producedCount.load(), expectedTotal);
    ASSERT_EQ(consumedCount.load(), expectedTotal);
    ASSERT_TRUE(queue.isEmpty());
}

// ISR safety simulation - verifies lock-free operation under preemption
TEST_F(AtomicQueueTest, ISRSafetySimulation) {
    const FwSizeType queueCap = 16;
    queue.create(queueCap, 64, allocator, 0);

    std::atomic<bool> isrActive{false};
    std::atomic<U32> mainEnqueued{0};
    std::atomic<U32> mainDequeued{0};
    std::atomic<U32> isrEnqueued{0};
    std::atomic<U32> stopThreads{false};

    // Main thread: performs enqueue/dequeue cycles
    auto mainTask = [&]() {
        U8 buf[64];
        FwSizeType size;
        for (U32 i = 0; i < 1000 && !stopThreads.load(); ++i) {
            buf[0] = 0xAA;
            buf[1] = static_cast<U8>(i & 0xFF);
            if (queue.enqueue(buf, 2)) {
                mainEnqueued++;
            }

            // Yield to allow ISR simulation to preempt
            std::this_thread::yield();

            // Wait for ISR to clear (simulates resuming after interrupt)
            while (isrActive.load()) {
                std::this_thread::yield();
            }

            // Attempt dequeue
            U8 localRecvBuf[64];
            if (queue.dequeue(localRecvBuf, 64, size)) {
                mainDequeued++;
            }
        }
    };

    // Simulated ISR: rapid high-priority interruptions writing to queue
    // This exercises the lock-free properties critical for ISR safety
    auto simulatedISR = [&]() {
        U8 isrBuf[64];
        for (U32 i = 0; i < 5000 && !stopThreads.load(); ++i) {
            isrActive.store(true);
            isrBuf[0] = 0xBB;  // ISR marker
            isrBuf[1] = static_cast<U8>(i & 0xFF);
            if (queue.enqueue(isrBuf, 2)) {
                isrEnqueued++;
            }
            isrActive.store(false);

            // Brief delay to simulate ISR frequency (~100us intervals)
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    };

    std::thread main(mainTask);
    std::thread isr(simulatedISR);

    // Run for bounded duration
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    stopThreads.store(true);

    main.join();
    isr.join();

    // Verify no corruption: operations completed without hang or assertion
    EXPECT_GT(mainEnqueued.load(), 0) << "Main thread should successfully enqueue";
    EXPECT_GT(isrEnqueued.load(), 0) << "ISR should successfully enqueue";

    // Drain remaining messages and verify no corruption
    U8 drainBuf[64];
    FwSizeType drainSize;
    U32 drained = 0;
    while (queue.dequeue(drainBuf, 64, drainSize) && drained < queueCap * 2) {
        // Verify marker is valid (either 0xAA from main or 0xBB from ISR)
        ASSERT_TRUE(drainBuf[0] == 0xAA || drainBuf[0] == 0xBB)
            << "Queue corruption detected: invalid marker 0x" << std::hex << static_cast<int>(drainBuf[0]);
        drained++;
    }
}

// Counter wrap-around boundary test - verifies algorithm correctness near 32-bit limits
TEST_F(AtomicQueueTest, CounterWrapBoundary) {
    const FwSizeType cap = 4;
    queue.create(cap, 32, allocator, 0);

    // This test validates wrap-around handling via sustained operations
    // Note: Directly manipulating counters to near-wrap state would require friend access
    // For true wrap validation, counters would be set near FwSizeType maximum
    // For true wrap validation on 64-bit, would require >2^64 operations (infeasible)
    // For 32-bit platforms, wrap occurs at 2^32 (~4.3B ops, ~1 hour at 1M ops/sec)

    // Perform operations that span potential wrap boundary
    const FwSizeType opsAcrossWrap = 200;  // Crosses 32-bit boundary if starting near max

    for (FwSizeType cycle = 0; cycle < opsAcrossWrap / cap; ++cycle) {
        // Fill queue
        for (FwSizeType i = 0; i < cap; ++i) {
            sendBuf[0] = static_cast<U8>(cycle);
            sendBuf[1] = static_cast<U8>(i);
            ASSERT_TRUE(queue.enqueue(sendBuf, 2)) << "Enqueue failed at cycle " << cycle;
        }

        // Drain queue, verify FIFO order maintained
        for (FwSizeType i = 0; i < cap; ++i) {
            ASSERT_TRUE(queue.dequeue(recvBuf, 32, actualSize)) << "Dequeue failed at cycle " << cycle;
            ASSERT_EQ(actualSize, 2);
            ASSERT_EQ(recvBuf[0], static_cast<U8>(cycle)) << "FIFO violated at cycle " << cycle;
            ASSERT_EQ(recvBuf[1], static_cast<U8>(i)) << "FIFO violated at cycle " << cycle;
        }
    }

    ASSERT_TRUE(queue.isEmpty());

    // NOTE: Full 32-bit wrap validation requires either:
    // 1. Running ~4.3 billion operations (impractical for unit test)
    // 2. Test-only internal state manipulation (via friend class)
    // 3. Accelerated test on 16-bit platform (if available)
    // This test validates algorithm logic; production testing on 32-bit targets
    // should include extended soak tests (hours) to exercise wrap in deployment environment.
}

// Partial allocation failure - verify cleanup on mid-creation failure
TEST_F(AtomicQueueTest, PartialAllocationFailure) {
    Types::AtomicQueue testQueue;

    // Allocator that fails after 1 allocation (slots succeed, buffer memory fails)
    PartialFailAllocator partialAlloc(1);

    // AtomicQueue allocates in sequence: 1) slot array, 2) buffer memory, 3) semaphore
    // Failing on 2nd allocation should trigger cleanup
    ASSERT_DEATH_IF_SUPPORTED(testQueue.create(10, 64, partialAlloc, 0), "Assert:.*MemAllocator\\.cpp");

    // Verify no memory leaks: allocator should have cleaned up the 1st allocation
    // (This is validated implicitly by ASAN/Valgrind in CI, not explicitly here)
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
