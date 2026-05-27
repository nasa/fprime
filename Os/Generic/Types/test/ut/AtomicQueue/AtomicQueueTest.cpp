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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Fw/Types/MemAllocator.hpp"
#include "Os/Generic/Types/AtomicQueue.hpp"

// Simple heap allocator for testing
class TestAllocator : public Fw::MemAllocator {
  public:
    TestAllocator() : m_bytesAllocated(0), m_allocations(0) {}

    void* allocate(const FwEnumStoreType identifier,
                   FwSizeType& size,
                   bool& recoverable,
                   FwSizeType alignment) override {
        (void)identifier;
        recoverable = true;
        // Use posix_memalign for aligned allocation
        void* mem = nullptr;
        if (alignment > 0) {
            int result = posix_memalign(&mem, alignment, size);
            if (result != 0) {
                mem = nullptr;
            }
        } else {
            mem = ::malloc(size);
        }
        if (mem != nullptr) {
            m_bytesAllocated += size;
            m_allocations++;
        }
        return mem;
    }

    void deallocate(const FwEnumStoreType identifier, void* ptr) override {
        (void)identifier;
        if (ptr != nullptr) {
            ::free(ptr);
            m_allocations--;
        }
    }

    FwSizeType getBytesAllocated() const { return m_bytesAllocated; }
    FwSizeType getAllocationCount() const { return m_allocations; }

  private:
    FwSizeType m_bytesAllocated;
    FwSizeType m_allocations;
};

TEST(Nominal, Creation) {
    TestAllocator allocator;

    // Test various capacities
    {
        Types::AtomicQueue queue;
        queue.create(8, 64, allocator, 0);
        ASSERT_EQ(queue.getCapacity(), 8);
        ASSERT_EQ(queue.getBufferSize(), 64);
        ASSERT_TRUE(queue.isEmpty());
        ASSERT_FALSE(queue.isFull());
        queue.teardown();
    }

    // Test power-of-2 capacity
    {
        Types::AtomicQueue queue;
        queue.create(16, 128, allocator, 0);
        ASSERT_EQ(queue.getCapacity(), 16);
        ASSERT_EQ(queue.getBufferSize(), 128);
        queue.teardown();
    }

    // Test non-power-of-2 capacity
    {
        Types::AtomicQueue queue;
        queue.create(100, 256, allocator, 0);
        ASSERT_EQ(queue.getCapacity(), 100);
        ASSERT_EQ(queue.getBufferSize(), 256);
        queue.teardown();
    }
}

TEST(Nominal, SingleEnqueueDequeue) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    queue.create(8, 64, allocator, 0);

    U8 sendBuf[64];
    U8 recvBuf[64];
    FwSizeType actualSize;

    // Prepare test data
    for (FwSizeType i = 0; i < 64; ++i) {
        sendBuf[i] = static_cast<U8>(i);
    }

    // Enqueue message
    ASSERT_TRUE(queue.enqueue(sendBuf, 64));
    ASSERT_FALSE(queue.isEmpty());
    ASSERT_EQ(queue.getSize(), 1);

    // Dequeue message
    ASSERT_TRUE(queue.dequeue(recvBuf, 64, actualSize));
    ASSERT_EQ(actualSize, 64);
    ASSERT_TRUE(queue.isEmpty());
    ASSERT_EQ(queue.getSize(), 0);

    // Verify data
    for (FwSizeType i = 0; i < 64; ++i) {
        ASSERT_EQ(recvBuf[i], sendBuf[i]);
    }

    queue.teardown();
}

TEST(Nominal, FillAndDrain) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    const FwSizeType capacity = 8;
    const FwSizeType bufSize = 32;
    queue.create(capacity, bufSize, allocator, 0);

    U8 sendBuf[32];
    U8 recvBuf[32];
    FwSizeType actualSize;

    // Fill queue
    for (FwSizeType i = 0; i < capacity; ++i) {
        // Create unique message
        for (FwSizeType j = 0; j < bufSize; ++j) {
            sendBuf[j] = static_cast<U8>(i * 10 + j);
        }

        ASSERT_TRUE(queue.enqueue(sendBuf, bufSize));
        ASSERT_EQ(queue.getSize(), i + 1);
    }

    ASSERT_TRUE(queue.isFull());
    ASSERT_EQ(queue.getSize(), capacity);

    // Queue should reject when full
    ASSERT_FALSE(queue.enqueue(sendBuf, bufSize));

    // Drain queue
    for (FwSizeType i = 0; i < capacity; ++i) {
        ASSERT_TRUE(queue.dequeue(recvBuf, bufSize, actualSize));
        ASSERT_EQ(actualSize, bufSize);
        ASSERT_EQ(queue.getSize(), capacity - i - 1);

        // Verify data
        for (FwSizeType j = 0; j < bufSize; ++j) {
            ASSERT_EQ(recvBuf[j], static_cast<U8>(i * 10 + j));
        }
    }

    ASSERT_TRUE(queue.isEmpty());

    // Queue should return false when empty
    ASSERT_FALSE(queue.dequeue(recvBuf, bufSize, actualSize));

    queue.teardown();
}

TEST(Nominal, FifoOrdering) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    queue.create(16, 8, allocator, 0);

    U8 sendBuf[8];
    U8 recvBuf[8];
    FwSizeType actualSize;

    // Enqueue messages with unique markers
    for (FwSizeType i = 0; i < 10; ++i) {
        sendBuf[0] = static_cast<U8>(i);
        ASSERT_TRUE(queue.enqueue(sendBuf, 1));
    }

    // Dequeue and verify FIFO order
    for (FwSizeType i = 0; i < 10; ++i) {
        ASSERT_TRUE(queue.dequeue(recvBuf, 8, actualSize));
        ASSERT_EQ(actualSize, 1);
        ASSERT_EQ(recvBuf[0], static_cast<U8>(i));
    }

    queue.teardown();
}

TEST(Nominal, VariableMessageSizes) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    const FwSizeType maxSize = 128;
    queue.create(8, maxSize, allocator, 0);

    U8 sendBuf[128];
    U8 recvBuf[128];
    FwSizeType actualSize;

    // Enqueue messages of different sizes
    FwSizeType sizes[] = {8, 16, 32, 64, 128, 1, 100, 50};
    for (FwSizeType i = 0; i < 8; ++i) {
        FwSizeType msgSize = sizes[i];

        // Fill with pattern
        for (FwSizeType j = 0; j < msgSize; ++j) {
            sendBuf[j] = static_cast<U8>((i << 4) | (j & 0xF));
        }

        ASSERT_TRUE(queue.enqueue(sendBuf, msgSize));
    }

    // Dequeue and verify
    for (FwSizeType i = 0; i < 8; ++i) {
        FwSizeType expectedSize = sizes[i];

        ASSERT_TRUE(queue.dequeue(recvBuf, maxSize, actualSize));
        ASSERT_EQ(actualSize, expectedSize);

        for (FwSizeType j = 0; j < expectedSize; ++j) {
            ASSERT_EQ(recvBuf[j], static_cast<U8>((i << 4) | (j & 0xF)));
        }
    }

    queue.teardown();
}

TEST(Nominal, WrapAround) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    const FwSizeType capacity = 4;
    queue.create(capacity, 16, allocator, 0);

    U8 sendBuf[16];
    U8 recvBuf[16];
    FwSizeType actualSize;

    // Test multiple wrap-around cycles
    for (FwSizeType cycle = 0; cycle < 10; ++cycle) {
        // Fill queue
        for (FwSizeType i = 0; i < capacity; ++i) {
            sendBuf[0] = static_cast<U8>(cycle * capacity + i);
            ASSERT_TRUE(queue.enqueue(sendBuf, 1));
        }

        // Drain queue
        for (FwSizeType i = 0; i < capacity; ++i) {
            ASSERT_TRUE(queue.dequeue(recvBuf, 16, actualSize));
            ASSERT_EQ(recvBuf[0], static_cast<U8>(cycle * capacity + i));
        }
    }

    queue.teardown();
}

TEST(Nominal, NonPowerOf2Capacity) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    queue.create(100, 64, allocator, 0);  // Non-power-of-2

    U8 sendBuf[64];
    U8 recvBuf[64];
    FwSizeType actualSize;

    // Fill with 100 messages
    for (FwSizeType i = 0; i < 100; ++i) {
        sendBuf[0] = static_cast<U8>(i);
        ASSERT_TRUE(queue.enqueue(sendBuf, 1));
    }

    ASSERT_TRUE(queue.isFull());

    // Verify FIFO order
    for (FwSizeType i = 0; i < 100; ++i) {
        ASSERT_TRUE(queue.dequeue(recvBuf, 64, actualSize));
        ASSERT_EQ(recvBuf[0], static_cast<U8>(i));
    }

    ASSERT_TRUE(queue.isEmpty());
    queue.teardown();
}

TEST(EdgeCase, EmptyDequeue) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    queue.create(4, 32, allocator, 0);

    U8 recvBuf[32];
    FwSizeType actualSize;

    // Multiple dequeue attempts on empty queue
    ASSERT_FALSE(queue.dequeue(recvBuf, 32, actualSize));
    ASSERT_FALSE(queue.dequeue(recvBuf, 32, actualSize));
    ASSERT_FALSE(queue.dequeue(recvBuf, 32, actualSize));

    queue.teardown();
}

TEST(EdgeCase, FullEnqueue) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    const FwSizeType capacity = 4;
    queue.create(capacity, 16, allocator, 0);

    U8 sendBuf[16];
    sendBuf[0] = 0xFF;

    // Fill queue
    for (FwSizeType i = 0; i < capacity; ++i) {
        ASSERT_TRUE(queue.enqueue(sendBuf, 1));
    }

    // Multiple enqueue attempts on full queue
    ASSERT_FALSE(queue.enqueue(sendBuf, 1));
    ASSERT_FALSE(queue.enqueue(sendBuf, 1));
    ASSERT_FALSE(queue.enqueue(sendBuf, 1));

    queue.teardown();
}

TEST(EdgeCase, MinimalCapacity) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    queue.create(1, 16, allocator, 0);  // Capacity of 1

    U8 sendBuf[16];
    U8 recvBuf[16];
    FwSizeType actualSize;

    sendBuf[0] = 0xAA;

    // Single slot queue
    ASSERT_TRUE(queue.enqueue(sendBuf, 1));
    ASSERT_TRUE(queue.isFull());
    ASSERT_FALSE(queue.enqueue(sendBuf, 1));

    ASSERT_TRUE(queue.dequeue(recvBuf, 16, actualSize));
    ASSERT_EQ(recvBuf[0], 0xAA);
    ASSERT_TRUE(queue.isEmpty());

    queue.teardown();
}

TEST(EdgeCase, IsCreated) {
    TestAllocator allocator;
    Types::AtomicQueue queue;

    // Before creation
    ASSERT_FALSE(queue.isCreated());

    // After creation
    queue.create(4, 32, allocator, 0);
    ASSERT_TRUE(queue.isCreated());

    // After teardown
    queue.teardown();
    ASSERT_FALSE(queue.isCreated());
}

TEST(EdgeCase, DoubleTeardown) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    queue.create(4, 32, allocator, 0);

    // First teardown
    queue.teardown();
    ASSERT_FALSE(queue.isCreated());

    // Second teardown should be safe (no-op)
    queue.teardown();
    ASSERT_FALSE(queue.isCreated());
}

TEST(EdgeCase, GettersOnUninitializedQueue) {
    Types::AtomicQueue queue;

    // Should return 0 for uninitialized queue
    ASSERT_EQ(queue.getCapacity(), 0);
    ASSERT_EQ(queue.getBufferSize(), 0);
    ASSERT_EQ(queue.getSize(), 0);
    ASSERT_TRUE(queue.isEmpty());
    ASSERT_FALSE(queue.isFull());
    ASSERT_FALSE(queue.isCreated());
}

// ======================================================================
// Blocking Operation Tests
// ======================================================================

#include "Fw/Time/TimeInterval.hpp"
#include "Os/Mutex.hpp"
#include "Os/Task.hpp"

// Context for blocking send test
struct BlockingSendContext {
    Types::AtomicQueue* queue;
    bool messageSent;
    bool sendSucceeded;
    bool started;
    Os::Mutex mutex;

    BlockingSendContext() : queue(nullptr), messageSent(false), sendSucceeded(false), started(false) {}
};

// Thread routine for blocking send
static void blockingSendThread(void* arg) {
    BlockingSendContext* ctx = static_cast<BlockingSendContext*>(arg);

    ctx->mutex.take();
    ctx->started = true;
    ctx->mutex.release();

    U8 data[32];
    for (FwSizeType i = 0; i < sizeof(data); ++i) {
        data[i] = 0xBB;
    }

    // This should block until space is available
    bool result = ctx->queue->enqueueBlocking(data, sizeof(data), true);

    ctx->mutex.take();
    ctx->messageSent = true;
    ctx->sendSucceeded = result;
    ctx->mutex.release();
}

// Test that blocking send actually blocks when queue is full
TEST(Blocking, EnqueueBlocksWhenFullViaNonBlocking) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    const FwSizeType capacity = 2;
    queue.create(capacity, 64, allocator, 0);

    // Fill queue with NON-BLOCKING sends (this exposed the bug)
    U8 fillData[32];
    for (FwSizeType i = 0; i < capacity; ++i) {
        fillData[0] = static_cast<U8>(i);
        ASSERT_TRUE(queue.enqueue(fillData, sizeof(fillData)));
    }
    ASSERT_TRUE(queue.isFull());

    // Spawn thread that will attempt blocking send
    BlockingSendContext ctx;
    ctx.queue = &queue;

    Os::Task senderTask;
    Os::Task::Arguments args(Fw::String("BlockingSender"), blockingSendThread, &ctx, Os::Task::TASK_PRIORITY_DEFAULT,
                             Os::Task::TASK_DEFAULT);

    Os::Task::Status taskStatus = senderTask.start(args);
    ASSERT_EQ(Os::Task::Status::OP_OK, taskStatus);

    // Wait for thread to start
    for (int i = 0; i < 100 && !ctx.started; ++i) {
        Os::Task::delay(Fw::TimeInterval(0, 10000));  // 10ms
    }
    ASSERT_TRUE(ctx.started);

    // Give thread time to block
    Os::Task::delay(Fw::TimeInterval(0, 100000));  // 100ms

    // Verify message hasn't been sent yet (thread should be blocked)
    ctx.mutex.take();
    bool sentBeforeReceive = ctx.messageSent;
    ctx.mutex.release();
    ASSERT_FALSE(sentBeforeReceive) << "Message sent before space was available!";

    // Dequeue to make space - this should unblock the sender
    U8 recvData[64];
    FwSizeType actualSize;
    ASSERT_TRUE(queue.dequeue(recvData, sizeof(recvData), actualSize));

    // Wait for sender to complete
    Os::Task::Status joinStatus = senderTask.join();
    ASSERT_EQ(Os::Task::Status::OP_OK, joinStatus);

    // Verify send completed successfully
    ASSERT_TRUE(ctx.messageSent);
    ASSERT_TRUE(ctx.sendSucceeded);

    queue.teardown();
}

// Test mixed blocking and non-blocking operations maintain semaphore sync
TEST(Blocking, MixedBlockingNonBlocking) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    const FwSizeType capacity = 4;
    queue.create(capacity, 64, allocator, 0);

    U8 sendBuf[32];
    U8 recvBuf[64];
    FwSizeType actualSize;

    // Test sequence that exercises semaphore synchronization:
    // 1. Non-blocking send (should decrement semaphore)
    sendBuf[0] = 1;
    ASSERT_TRUE(queue.enqueue(sendBuf, sizeof(sendBuf)));

    // 2. Blocking send (should decrement semaphore)
    sendBuf[0] = 2;
    ASSERT_TRUE(queue.enqueueBlocking(sendBuf, sizeof(sendBuf), true));

    // 3. Non-blocking send
    sendBuf[0] = 3;
    ASSERT_TRUE(queue.enqueue(sendBuf, sizeof(sendBuf)));

    // 4. Blocking send
    sendBuf[0] = 4;
    ASSERT_TRUE(queue.enqueueBlocking(sendBuf, sizeof(sendBuf), true));

    // Queue should now be full
    ASSERT_TRUE(queue.isFull());

    // Next non-blocking send should fail
    ASSERT_FALSE(queue.enqueue(sendBuf, sizeof(sendBuf)));

    // Dequeue all (should increment semaphore)
    for (FwSizeType i = 0; i < capacity; ++i) {
        ASSERT_TRUE(queue.dequeue(recvBuf, sizeof(recvBuf), actualSize));
        ASSERT_EQ(recvBuf[0], i + 1);
    }

    // Queue should be empty
    ASSERT_TRUE(queue.isEmpty());

    // Should be able to enqueue again (semaphore properly restored)
    sendBuf[0] = 5;
    ASSERT_TRUE(queue.enqueue(sendBuf, sizeof(sendBuf)));

    queue.teardown();
}

// Test that non-blocking enqueue returns false immediately when full
TEST(Blocking, NonBlockingEnqueueReturnsFalseWhenFull) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    const FwSizeType capacity = 3;
    queue.create(capacity, 64, allocator, 0);

    U8 data[32];

    // Fill queue completely
    for (FwSizeType i = 0; i < capacity; ++i) {
        ASSERT_TRUE(queue.enqueue(data, sizeof(data)));
    }

    // Non-blocking should return false immediately (not block)
    ASSERT_FALSE(queue.enqueue(data, sizeof(data)));

    queue.teardown();
}

// Test enqueueBlocking with blockIfFull=false (non-blocking mode)
TEST(Blocking, EnqueueBlockingNonBlockingMode) {
    TestAllocator allocator;
    Types::AtomicQueue queue;
    const FwSizeType capacity = 3;
    queue.create(capacity, 64, allocator, 0);

    U8 data[32];
    data[0] = 0xAB;

    // Should succeed when queue has space (blockIfFull=false)
    ASSERT_TRUE(queue.enqueueBlocking(data, sizeof(data), false));

    // Fill remaining slots
    for (FwSizeType i = 1; i < capacity; ++i) {
        ASSERT_TRUE(queue.enqueue(data, sizeof(data)));
    }

    ASSERT_TRUE(queue.isFull());

    // Should return false immediately when full (blockIfFull=false, not blocking)
    ASSERT_FALSE(queue.enqueueBlocking(data, sizeof(data), false));

    queue.teardown();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
