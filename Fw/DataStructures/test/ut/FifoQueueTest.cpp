// ======================================================================
// \title  FifoQueueTest.cpp
// \author bocchino
// \brief  cpp file for FifoQueue tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/DataStructures/Array.hpp"
#include "Fw/DataStructures/FifoQueue.hpp"

namespace Fw {

template <typename T, FwSizeType C>
class FifoQueueTester {
  public:
    FifoQueueTester(const FifoQueue<T, C>& queue) : m_queue(queue) {}

    const ExternalFifoQueue<T> getExtQueue() const { return this->m_queue.extQueue; }

    const typename Array<T, C>::Elements& getItems() const { return this->m_queue.m_items; }

  private:
    const FifoQueue<T, C>& m_queue;
};

TEST(FifoQueue, ZeroArgConstructor) {
    constexpr FwSizeType C = 10;
    FifoQueue<U32, C> queue;
    ASSERT_EQ(queue.getCapacity(), C);
    ASSERT_EQ(queue.getSize(), 0);
}

#if 0
TEST(FifoQueue, TypedStorageConstructor) {
    constexpr FwSizeType capacity = 10;
    U32 items[capacity];
    FifoQueue<U32> queue(items, capacity);
    FifoQueueTester<U32> tester(queue);
    ASSERT_EQ(tester.getItems().getElements(), items);
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(FifoQueue, UntypedStorageConstructor) {
    constexpr FwSizeType capacity = 10;
    alignas(U32) U8 bytes[capacity * sizeof(U32)];
    FifoQueue<U32> queue(ByteArray(&bytes[0], sizeof bytes), capacity);
    FifoQueueTester<U32> tester(queue);
    ASSERT_EQ(tester.getItems().getElements(), reinterpret_cast<U32*>(bytes));
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(FifoQueue, EnqueueOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    U32 elts[capacity];
    FifoQueue<U32> queue(elts, capacity);
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const U32 val = STest::Pick::any();
        // Enqueue it
        auto status = queue.enqueue(val);
        ASSERT_EQ(status, Success::SUCCESS);
        // Peek it
        U32 val1 = 0;
        status = queue.peek(val1, i);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val1, val);
        // Check the size
        ASSERT_EQ(queue.getSize(), i + 1);
    }
    queue.clear();
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(ArrayFIFO, EnqueueFull) {
    constexpr const FwSizeType capacity = 1000;
    U32 elts[capacity];
    FifoQueue<U32> queue(elts, capacity);
    // Fill up the FIFO
    for (FwSizeType i = 0; i < capacity; i++) {
        queue.enqueue(0);
    }
    // Now try to push another element
    const U32 val = STest::Pick::any();
    const auto status = queue.enqueue(val);
    // Push should fail
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(FifoQueue, CopyConstructor) {
    constexpr FwSizeType capacity = 3;
    U32 items[capacity];
    // Call the constructor providing backing storage
    FifoQueue<U32> q1(items, capacity);
    // Enqueue an item
    U32 value = 42;
    (void)q1.enqueue(value);
    // Call the copy constructor
    FifoQueue<U32> q2(q1);
    FifoQueueTester<U32> tester1(q1);
    FifoQueueTester<U32> tester2(q2);
    ASSERT_EQ(tester2.getItems().getElements(), items);
    ASSERT_EQ(tester2.getItems().getSize(), capacity);
    ASSERT_EQ(tester2.getEnqueueIndex().getValue(), 1);
    ASSERT_EQ(tester2.getDequeueIndex().getValue(), 0);
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(FifoQueue, CopyAssignmentOperator) {
    constexpr FwSizeType capacity = 3;
    U32 items[capacity];
    // Call the constructor providing backing storage
    FifoQueue<U32> q1(items, capacity);
    // Enqueue an item
    U32 value = 42;
    (void)q1.enqueue(value);
    // Call the default constructor
    FifoQueue<U32> q2;
    ASSERT_EQ(q2.getSize(), 0);
    // Call the copy assignment operator
    q2 = q1;
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(FifoQueue, DequeueOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    U32 items[capacity];
    FifoQueue<U32> queue(items, capacity);
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const U32 val = STest::Pick::any();
        // Enqueue it
        const auto status = queue.enqueue(val);
        ASSERT_EQ(val, items[i]);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(queue.getSize(), i + 1);
    }
    for (FwSizeType i = 0; i < size; i++) {
        U32 val = 0;
        // Peek
        auto status = queue.peek(val);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val, items[i]);
        // Dequeue it
        status = queue.dequeue(val);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val, items[i]);
        ASSERT_EQ(queue.getSize(), size - i - 1);
    }
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(FifoQueue, DequeueEmpty) {
    constexpr const FwSizeType capacity = 1000;
    U32 items[capacity];
    FifoQueue<U32> queue(items, capacity);
    U32 val = 0;
    const auto status = queue.dequeue(val);
    ASSERT_EQ(status, Success::FAILURE);
}

namespace {

void testCopyDataFrom(FifoQueueBase<U32>& q1, FwSizeType size1, FifoQueueBase<U32>& q2) {
    q1.clear();
    for (FwSizeType i = 0; i < size1; i++) {
        const auto status = q1.enqueue(static_cast<U32>(i));
        ASSERT_EQ(status, Success::SUCCESS);
    }
    q2.copyDataFrom(q1);
    const auto capacity2 = q2.getCapacity();
    const FwSizeType size = FW_MIN(size1, capacity2);
    for (FwSizeType i = 0; i < size; i++) {
        U32 val1 = 0;
        auto status = q1.peek(val1, i);
        ASSERT_EQ(status, Success::SUCCESS);
        U32 val2 = 1;
        status = q2.peek(val2, i);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val1, val2);
    }
}

}  // namespace

TEST(FifoQueue, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    U32 items1[maxSize];
    U32 items2[maxSize];
    FifoQueue<U32> q1(items1, maxSize);
    // size1 < capacity2
    {
        FifoQueue<U32> q2(items2, maxSize);
        testCopyDataFrom(q1, smallSize, q2);
    }
    // size1 == size2
    {
        FifoQueue<U32> q2(items2, maxSize);
        testCopyDataFrom(q1, maxSize, q2);
    }
    // size1 > size2
    {
        FifoQueue<U32> q2(items2, smallSize);
        testCopyDataFrom(q1, maxSize, q2);
    }
}
#endif
}  // namespace Fw
