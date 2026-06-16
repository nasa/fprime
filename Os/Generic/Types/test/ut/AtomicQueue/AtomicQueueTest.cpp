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
#include <numeric>
#include <vector>
#include "Os/Generic/Types/AtomicQueue.hpp"
#include "Fw/Types/MemAllocator.hpp"

// Test allocator for queue memory
class TestAllocator : public Fw::MemAllocator {
  public:
    TestAllocator() : m_bytesAllocated(0), m_allocations(0) {}
    
    void* allocate(const FwEnumStoreType identifier, FwSizeType& size, bool& recoverable, FwSizeType alignment) override {
        (void)identifier;
        recoverable = true;
        void* mem = (alignment > 0) ? nullptr : ::malloc(size);
        if (alignment > 0) {
            int result = posix_memalign(&mem, alignment, size);
            if (result != 0) mem = nullptr;
        }
        if (mem) { m_bytesAllocated += size; m_allocations++; }
        return mem;
    }
    
    void deallocate(const FwEnumStoreType identifier, void* ptr) override {
        (void)identifier;
        if (ptr) { ::free(ptr); m_allocations--; }
    }
    
    FwSizeType getBytesAllocated() const { return m_bytesAllocated; }
    FwSizeType getAllocationCount() const { return m_allocations; }
    
  private:
    FwSizeType m_bytesAllocated, m_allocations;
};

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
        for (FwSizeType i = 0; i < size; ++i) sendBuf[i] = static_cast<U8>(offset + i);
    }
    
    // Helper: verify recv matches send
    void verifyData(FwSizeType size) {
        for (FwSizeType i = 0; i < size; ++i) ASSERT_EQ(recvBuf[i], sendBuf[i]);
    }
};

// Parameterized test: consolidates Creation + basic capacity tests
struct QueueParams { FwSizeType capacity, bufSize; };
class AtomicQueueParamTest : public AtomicQueueTest, 
                             public ::testing::WithParamInterface<QueueParams> {};

TEST_P(AtomicQueueParamTest, Creation) {
    QueueParams params = GetParam();
    queue.create(params.capacity, params.bufSize, allocator, 0);
    ASSERT_EQ(queue.getCapacity(), params.capacity);
    ASSERT_EQ(queue.getBufferSize(), params.bufSize);
    ASSERT_TRUE(queue.isEmpty());
    ASSERT_FALSE(queue.isFull());
}

INSTANTIATE_TEST_SUITE_P(VariousCapacities, AtomicQueueParamTest, ::testing::Values(
    QueueParams{8, 64},      // Small power-of-2
    QueueParams{16, 128},    // Medium power-of-2
    QueueParams{100, 256}    // Non-power-of-2
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
    ASSERT_FALSE(queue.enqueue(sendBuf, bufSz)); // Reject when full
    
    // Drain: verify FIFO order and patterns
    for (FwSizeType i = 0; i < cap; ++i) {
        fillPattern(bufSz, i * 10); // Regenerate expected pattern
        ASSERT_TRUE(queue.dequeue(recvBuf, bufSz, actualSize));
        ASSERT_EQ(actualSize, bufSz);
        verifyData(bufSz);
    }
    ASSERT_TRUE(queue.isEmpty());
    ASSERT_FALSE(queue.dequeue(recvBuf, bufSz, actualSize)); // Reject when empty
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

// Edge case: empty queue rejects dequeue
TEST_F(AtomicQueueTest, EmptyDequeue) {
    queue.create(4, 32, allocator, 0);
    for (FwSizeType i = 0; i < 3; ++i) 
        ASSERT_FALSE(queue.dequeue(recvBuf, 32, actualSize));
}

// Edge case: full queue rejects enqueue
TEST_F(AtomicQueueTest, FullEnqueue) {
    const FwSizeType cap = 4;
    queue.create(cap, 16, allocator, 0);
    sendBuf[0] = 0xFF;
    for (FwSizeType i = 0; i < cap; ++i) 
        ASSERT_TRUE(queue.enqueue(sendBuf, 1));
    for (FwSizeType i = 0; i < 3; ++i) 
        ASSERT_FALSE(queue.enqueue(sendBuf, 1));
}

// Edge case: capacity=1 queue
TEST_F(AtomicQueueTest, MinimalCapacity) {
    queue.create(1, 16, allocator, 0);
    sendBuf[0] = 0xAA;
    ASSERT_TRUE(queue.enqueue(sendBuf, 1));
    ASSERT_TRUE(queue.isFull());
    ASSERT_FALSE(queue.enqueue(sendBuf, 1));
    ASSERT_TRUE(queue.dequeue(recvBuf, 16, actualSize));
    ASSERT_EQ(recvBuf[0], 0xAA);
    ASSERT_TRUE(queue.isEmpty());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
