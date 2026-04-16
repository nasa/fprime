// ======================================================================
// \title  QueueTest.cpp
// \author Auto-generated
// \brief  cpp file for Queue unit tests
//
// Test the FIFO/LIFO and DROP_NEWEST/DROP_OLDEST functionality
// ======================================================================

#include <gtest/gtest.h>
#include <Fw/Types/Assert.hpp>
#include <Utils/Types/Queue.hpp>

class QueueTest : public ::testing::Test {
  protected:
    static const FwSizeType MSG_SIZE = sizeof(U32);
    static const FwSizeType QUEUE_DEPTH = 5;
    static const FwSizeType BUFFER_SIZE = MSG_SIZE * QUEUE_DEPTH;

    void enqueueValue(Types::Queue& queue, U32 value) {
        Fw::SerializeStatus status = queue.enqueue(reinterpret_cast<const U8*>(&value), MSG_SIZE);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, status);
    }

    U32 dequeueValue(Types::Queue& queue) {
        U32 value;
        Fw::SerializeStatus status = queue.dequeue(reinterpret_cast<U8*>(&value), MSG_SIZE);
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, status);
        return value;
    }
};

// Test FIFO mode (default)
TEST_F(QueueTest, FIFOMode) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE);

    // Enqueue values 1, 2, 3, 4, 5
    for (U32 i = 1; i <= 5; i++) {
        enqueueValue(queue, i);
    }

    // Dequeue should return in order: 1, 2, 3, 4, 5
    for (U32 i = 1; i <= 5; i++) {
        EXPECT_EQ(i, dequeueValue(queue));
    }
}

// Test LIFO mode
TEST_F(QueueTest, LIFOMode) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE, Types::QUEUE_LIFO);

    // Enqueue values 1, 2, 3, 4, 5
    for (U32 i = 1; i <= 5; i++) {
        enqueueValue(queue, i);
    }

    // Dequeue should return in reverse order: 5, 4, 3, 2, 1
    for (U32 i = 5; i >= 1; i--) {
        EXPECT_EQ(i, dequeueValue(queue));
    }
}

// Test DROP_NEWEST mode (default) - queue full should reject new items
TEST_F(QueueTest, DropNewestMode) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE);

    // Fill the queue completely
    for (U32 i = 1; i <= 5; i++) {
        enqueueValue(queue, i);
    }

    // Try to enqueue when full - should fail
    U32 newValue = 99;
    Fw::SerializeStatus status = queue.enqueue(reinterpret_cast<const U8*>(&newValue), MSG_SIZE);
    EXPECT_EQ(Fw::FW_SERIALIZE_NO_ROOM_LEFT, status);

    // Verify original values still intact
    for (U32 i = 1; i <= 5; i++) {
        EXPECT_EQ(i, dequeueValue(queue));
    }
}

// Test DROP_OLDEST mode - queue full should drop oldest and add new
TEST_F(QueueTest, DropOldestMode) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE, Types::QUEUE_FIFO, Types::QUEUE_DROP_OLDEST);

    // Fill the queue with values 1, 2, 3, 4, 5
    for (U32 i = 1; i <= 5; i++) {
        enqueueValue(queue, i);
    }

    // Enqueue 99 when full - should succeed and drop oldest (1)
    U32 newValue = 99;
    Fw::SerializeStatus status = queue.enqueue(reinterpret_cast<const U8*>(&newValue), MSG_SIZE);
    EXPECT_EQ(Fw::FW_SERIALIZE_DISCARDED_EXISTING, status);

    // Should now have: 2, 3, 4, 5, 99
    EXPECT_EQ(2, dequeueValue(queue));
    EXPECT_EQ(3, dequeueValue(queue));
    EXPECT_EQ(4, dequeueValue(queue));
    EXPECT_EQ(5, dequeueValue(queue));
    EXPECT_EQ(99, dequeueValue(queue));
}

// Test LIFO with DROP_OLDEST
TEST_F(QueueTest, LIFOWithDropOldest) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE, Types::QUEUE_LIFO, Types::QUEUE_DROP_OLDEST);

    // Fill the queue with values 1, 2, 3, 4, 5
    for (U32 i = 1; i <= 5; i++) {
        enqueueValue(queue, i);
    }

    // Enqueue 99 when full - should succeed and drop oldest (1)
    U32 newValue = 99;
    Fw::SerializeStatus status = queue.enqueue(reinterpret_cast<const U8*>(&newValue), MSG_SIZE);
    EXPECT_EQ(Fw::FW_SERIALIZE_DISCARDED_EXISTING, status);

    // LIFO should return newest first: 99, 5, 4, 3, 2
    EXPECT_EQ(99, dequeueValue(queue));
    EXPECT_EQ(5, dequeueValue(queue));
    EXPECT_EQ(4, dequeueValue(queue));
    EXPECT_EQ(3, dequeueValue(queue));
    EXPECT_EQ(2, dequeueValue(queue));
}

// Test empty queue dequeue
TEST_F(QueueTest, DequeueEmpty) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE);

    U32 value;
    Fw::SerializeStatus status = queue.dequeue(reinterpret_cast<U8*>(&value), MSG_SIZE);
    EXPECT_EQ(Fw::FW_DESERIALIZE_BUFFER_EMPTY, status);
}

// Test queue size tracking
TEST_F(QueueTest, QueueSize) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE);

    EXPECT_EQ(0, queue.getQueueSize());

    enqueueValue(queue, 1);
    EXPECT_EQ(1, queue.getQueueSize());

    enqueueValue(queue, 2);
    enqueueValue(queue, 3);
    EXPECT_EQ(3, queue.getQueueSize());

    dequeueValue(queue);
    EXPECT_EQ(2, queue.getQueueSize());

    dequeueValue(queue);
    dequeueValue(queue);
    EXPECT_EQ(0, queue.getQueueSize());
}

// Test high water mark
TEST_F(QueueTest, HighWaterMark) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE);

    EXPECT_EQ(0, queue.get_high_water_mark());

    enqueueValue(queue, 1);
    EXPECT_EQ(1, queue.get_high_water_mark());

    enqueueValue(queue, 2);
    enqueueValue(queue, 3);
    EXPECT_EQ(3, queue.get_high_water_mark());

    // Dequeue doesn't lower high water mark
    dequeueValue(queue);
    EXPECT_EQ(3, queue.get_high_water_mark());

    // Clear and verify
    queue.clear_high_water_mark();
    EXPECT_EQ(0, queue.get_high_water_mark());
}

// Test alternating enqueue/dequeue with FIFO
TEST_F(QueueTest, AlternatingFIFO) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE);

    enqueueValue(queue, 1);
    enqueueValue(queue, 2);
    EXPECT_EQ(1, dequeueValue(queue));

    enqueueValue(queue, 3);
    EXPECT_EQ(2, dequeueValue(queue));
    EXPECT_EQ(3, dequeueValue(queue));
}

// Test alternating enqueue/dequeue with LIFO
TEST_F(QueueTest, AlternatingLIFO) {
    U8 storage[BUFFER_SIZE];
    Types::Queue queue;
    queue.setup(storage, BUFFER_SIZE, QUEUE_DEPTH, MSG_SIZE, Types::QUEUE_LIFO);

    enqueueValue(queue, 1);
    enqueueValue(queue, 2);
    EXPECT_EQ(2, dequeueValue(queue));  // LIFO returns newest

    enqueueValue(queue, 3);
    EXPECT_EQ(3, dequeueValue(queue));
    EXPECT_EQ(1, dequeueValue(queue));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
