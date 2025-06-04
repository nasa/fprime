// ======================================================================
// \title  ExternalFifoQueueTest.cpp
// \author bocchino
// \brief  cpp file for ExternalFifoQueue tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/DataStructures/ExternalFifoQueue.hpp"

namespace Fw {

template<typename T> class ExternalFifoQueueTester {

  public:

    ExternalFifoQueueTester<T>(const ExternalFifoQueue<T>& queue) : m_queue(queue) {}

    const ExternalArray<T> getItems() const { return this->m_queue.m_items; }

    const CircularIndex& getEnqueueIndex() const { return this->m_queue.m_enqueueIndex; }

    const CircularIndex& getDequeueIndex() const { return this->m_queue.m_dequeueIndex; }

  private:

    const ExternalFifoQueue<T>& m_queue;

};

TEST(ExternalFifoQueue, ZeroArgConstructor) {
    ExternalFifoQueue<U32> queue;
    ASSERT_EQ(queue.getCapacity(), 0);
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(ExternalFifoQueue, StorageConstructor) {
    constexpr FwSizeType capacity = 10;
    U32 items[capacity];
    ExternalFifoQueue<U32> queue(items, capacity);
    ExternalFifoQueueTester<U32> tester(queue);
    ASSERT_EQ(tester.getItems().getElements(), items);
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(ExternalFifoQueue, CopyConstructor) {
    constexpr FwSizeType capacity = 3;
    U32 items[capacity];
    // Call the constructor providing backing storage
    ExternalFifoQueue<U32> q1(items, capacity);
    // Enqueue an item
    U32 value = 42;
    (void) q1.enqueue(value);
    // Call the copy constructor
    ExternalFifoQueue<U32> q2(q1);
    ExternalFifoQueueTester<U32> tester1(q1);
    ExternalFifoQueueTester<U32> tester2(q2);
    ASSERT_EQ(tester2.getItems().getElements(), items);
    ASSERT_EQ(tester2.getItems().getSize(), capacity);
#if 0
    ASSERT_EQ(tester2.getEnqueueIndex().getValue(), 1);
    ASSERT_EQ(tester2.getDequeueIndex().getValue(), 0);
#endif
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(ExternalFifoQueue, CopyAssignmentOperator) {
    constexpr FwSizeType capacity = 3;
    U32 items[capacity];
    // Call the constructor providing backing storage
    ExternalFifoQueue<U32> q1(items, capacity);
    // Enqueue an item
    U32 value = 42;
    (void) q1.enqueue(value);
    // Call the default constructor
    ExternalFifoQueue<U32> q2;
    ASSERT_EQ(q2.getSize(), 0);
    // Call the copy assignment operator
    q2 = q1;
    ASSERT_EQ(q2.getSize(), 1);
}

}  // namespace Fw
