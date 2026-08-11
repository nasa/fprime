// ======================================================================
// \title  QueueSerializableTest.cpp
// \brief  Unit tests for the serializable enqueue/dequeue/popFront overloads
//
// Tests the Fw::Serializable and Fw::LinearBufferBase overloads of
// Types::Queue, which store messages via F Prime serialization into
// fixed-size, linear (non-wrapping) circular buffer slots.
// ======================================================================

#include <gtest/gtest.h>
#include <Fw/Buffer/Buffer.hpp>
#include <Fw/Com/ComBuffer.hpp>
#include <Fw/Types/Assert.hpp>
#include <Utils/Types/Queue.hpp>

namespace {

constexpr FwSizeType BUFFER_MSG_SIZE = Fw::Buffer::SERIALIZED_SIZE;
constexpr FwSizeType COM_MSG_SIZE = Fw::ComBuffer::SERIALIZED_SIZE;
constexpr FwSizeType QUEUE_DEPTH = 5;

U8 dummyRegion[1024];

Fw::Buffer makeBuffer(U32 seed) {
    return Fw::Buffer(&dummyRegion[seed % 512], 100u + seed, 200u + seed);
}

void expectBufferEq(const Fw::Buffer& expected, const Fw::Buffer& actual) {
    EXPECT_EQ(expected.getData(), actual.getData());
    EXPECT_EQ(expected.getSize(), actual.getSize());
    EXPECT_EQ(expected.getContext(), actual.getContext());
}

Fw::ComBuffer makeComBuffer(U32 seed, FwSizeType length) {
    Fw::ComBuffer com;
    for (FwSizeType i = 0; i < length; i++) {
        EXPECT_EQ(Fw::FW_SERIALIZE_OK, com.serializeFrom(static_cast<U8>((seed + i) & 0xFF)));
    }
    return com;
}

}  // namespace

// Round-trip an Fw::Serializable (Fw::Buffer) through the queue in FIFO order
TEST(QueueSerializable, BufferFifoRoundTrip) {
    U8 storage[BUFFER_MSG_SIZE * QUEUE_DEPTH];
    Types::Queue queue;
    queue.setup(storage, sizeof storage, QUEUE_DEPTH, BUFFER_MSG_SIZE);

    for (U32 i = 0; i < QUEUE_DEPTH; i++) {
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeBuffer(i)));
    }
    for (U32 i = 0; i < QUEUE_DEPTH; i++) {
        Fw::Buffer out;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.dequeue(out));
        expectBufferEq(makeBuffer(i), out);
    }
    Fw::Buffer out;
    ASSERT_EQ(Fw::FW_DESERIALIZE_BUFFER_EMPTY, queue.dequeue(out));
}

// Round-trip an Fw::LinearBufferBase (Fw::ComBuffer) through the queue, verifying
// contents and length are preserved
TEST(QueueSerializable, ComBufferFifoRoundTrip) {
    U8 storage[COM_MSG_SIZE * QUEUE_DEPTH];
    Types::Queue queue;
    queue.setup(storage, sizeof storage, QUEUE_DEPTH, COM_MSG_SIZE);

    for (U32 i = 0; i < QUEUE_DEPTH; i++) {
        Fw::ComBuffer com = makeComBuffer(i, 10u + i);
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(com));
    }
    for (U32 i = 0; i < QUEUE_DEPTH; i++) {
        Fw::ComBuffer expected = makeComBuffer(i, 10u + i);
        Fw::ComBuffer out;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.dequeue(out));
        ASSERT_EQ(expected.getSize(), out.getSize());
        EXPECT_EQ(0, memcmp(expected.getBuffAddr(), out.getBuffAddr(), static_cast<size_t>(expected.getSize())));
    }
}

// LIFO mode returns the newest message first
TEST(QueueSerializable, BufferLifo) {
    U8 storage[BUFFER_MSG_SIZE * QUEUE_DEPTH];
    Types::Queue queue;
    queue.setup(storage, sizeof storage, QUEUE_DEPTH, BUFFER_MSG_SIZE, Types::QUEUE_LIFO);

    for (U32 i = 0; i < QUEUE_DEPTH; i++) {
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeBuffer(i)));
    }
    for (U32 i = QUEUE_DEPTH; i > 0; i--) {
        Fw::Buffer out;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.dequeue(out));
        expectBufferEq(makeBuffer(i - 1), out);
    }
}

// DROP_NEWEST rejects an enqueue on a full queue without modifying contents
TEST(QueueSerializable, DropNewest) {
    U8 storage[BUFFER_MSG_SIZE * QUEUE_DEPTH];
    Types::Queue queue;
    queue.setup(storage, sizeof storage, QUEUE_DEPTH, BUFFER_MSG_SIZE, Types::QUEUE_FIFO, Types::QUEUE_DROP_NEWEST);

    for (U32 i = 0; i < QUEUE_DEPTH; i++) {
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeBuffer(i)));
    }
    ASSERT_EQ(Fw::FW_SERIALIZE_NO_ROOM_LEFT, queue.enqueue(makeBuffer(99)));
    Fw::Buffer out;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.dequeue(out));
    expectBufferEq(makeBuffer(0), out);
}

// DROP_OLDEST discards the front message and accepts the new one
TEST(QueueSerializable, DropOldest) {
    U8 storage[BUFFER_MSG_SIZE * QUEUE_DEPTH];
    Types::Queue queue;
    queue.setup(storage, sizeof storage, QUEUE_DEPTH, BUFFER_MSG_SIZE, Types::QUEUE_FIFO, Types::QUEUE_DROP_OLDEST);

    for (U32 i = 0; i < QUEUE_DEPTH; i++) {
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeBuffer(i)));
    }
    ASSERT_EQ(Fw::FW_SERIALIZE_DISCARDED_EXISTING, queue.enqueue(makeBuffer(99)));

    // Should now contain 1, 2, 3, 4, 99
    for (U32 i = 1; i < QUEUE_DEPTH; i++) {
        Fw::Buffer out;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.dequeue(out));
        expectBufferEq(makeBuffer(i), out);
    }
    Fw::Buffer out;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.dequeue(out));
    expectBufferEq(makeBuffer(99), out);
}

// popFront always removes the oldest message, even in LIFO mode
TEST(QueueSerializable, PopFrontLifo) {
    U8 storage[BUFFER_MSG_SIZE * QUEUE_DEPTH];
    Types::Queue queue;
    queue.setup(storage, sizeof storage, QUEUE_DEPTH, BUFFER_MSG_SIZE, Types::QUEUE_LIFO);

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeBuffer(1)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeBuffer(2)));

    Fw::Buffer out;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.popFront(out));
    expectBufferEq(makeBuffer(1), out);

    // Empty pops return buffer-empty
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.popFront(out));
    ASSERT_EQ(Fw::FW_DESERIALIZE_BUFFER_EMPTY, queue.popFront(out));
}

// Exercise ring wrap-around across many alternating enqueue/dequeue cycles.
// Each slot access asserts the linear (non-wrapping) slot invariant; this test
// walks the head index through every multiple of the message size in the store.
TEST(QueueSerializable, WrapAroundStress) {
    U8 storage[BUFFER_MSG_SIZE * QUEUE_DEPTH];
    Types::Queue queue;
    queue.setup(storage, sizeof storage, QUEUE_DEPTH, BUFFER_MSG_SIZE);

    // Keep the queue partially full while cycling well past several wrap points
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeBuffer(0)));
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeBuffer(1)));
    for (U32 i = 2; i < (QUEUE_DEPTH * 10); i++) {
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeBuffer(i)));
        Fw::Buffer out;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.dequeue(out));
        expectBufferEq(makeBuffer(i - 2), out);
    }
}

// Same wrap-around stress with ComBuffer (LinearBufferBase) messages
TEST(QueueSerializable, ComBufferWrapAroundStress) {
    U8 storage[COM_MSG_SIZE * QUEUE_DEPTH];
    Types::Queue queue;
    queue.setup(storage, sizeof storage, QUEUE_DEPTH, COM_MSG_SIZE);

    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(makeComBuffer(0, 5)));
    for (U32 i = 1; i < (QUEUE_DEPTH * 10); i++) {
        Fw::ComBuffer com = makeComBuffer(i, 5u + (i % 20));
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(com));
        Fw::ComBuffer expected = makeComBuffer(i - 1, 5u + ((i - 1) % 20));
        Fw::ComBuffer out;
        ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.dequeue(out));
        ASSERT_EQ(expected.getSize(), out.getSize());
        EXPECT_EQ(0, memcmp(expected.getBuffAddr(), out.getBuffAddr(), static_cast<size_t>(expected.getSize())));
    }
}

// A full-capacity ComBuffer round-trips (serialized size exactly equals the slot size)
TEST(QueueSerializable, ComBufferFullCapacity) {
    U8 storage[COM_MSG_SIZE * QUEUE_DEPTH];
    Types::Queue queue;
    queue.setup(storage, sizeof storage, QUEUE_DEPTH, COM_MSG_SIZE);

    Fw::ComBuffer com = makeComBuffer(7, FW_COM_BUFFER_MAX_SIZE);
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.enqueue(com));
    Fw::ComBuffer out;
    ASSERT_EQ(Fw::FW_SERIALIZE_OK, queue.dequeue(out));
    ASSERT_EQ(com.getSize(), out.getSize());
    EXPECT_EQ(0, memcmp(com.getBuffAddr(), out.getBuffAddr(), static_cast<size_t>(com.getSize())));
}
