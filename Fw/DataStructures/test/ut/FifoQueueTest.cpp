// ======================================================================
// \title  FifoQueueTest.cpp
// \author bocchino
// \brief  cpp file for FifoQueue tests
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/FifoQueueTestScenarios.hpp"

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

namespace FifoQueueTest {

TEST(FifoQueue, ZeroArgConstructor) {
    constexpr FwSizeType C = 10;
    FifoQueue<U32, C> queue;
    ASSERT_EQ(queue.getCapacity(), C);
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(FifoQueue, CopyConstructor) {
    constexpr FwSizeType capacity = 3;
    // Construct q1
    FifoQueue<U32, capacity> q1;
    // Enqueue an item
    U32 value = 42;
    (void)q1.enqueue(value);
    ASSERT_EQ(q1.getSize(), 1);
    // Use the copy constructor to construct q2
    FifoQueue<U32, capacity> q2(q1);
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(FifoQueue, EnqueueOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    FifoQueue<U32, capacity> queue;
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

TEST(FifoQueue, EnqueueFull) {
    constexpr const FwSizeType capacity = 1000;
    FifoQueue<U32, capacity> queue;
    // Fill up the FIFO
    for (FwSizeType i = 0; i < capacity; i++) {
        const auto status = queue.enqueue(0);
        ASSERT_EQ(status, Success::SUCCESS);
    }
    // Now try to push another element
    const U32 val = STest::Pick::any();
    const auto status = queue.enqueue(val);
    // Push should fail
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(FifoQueue, CopyAssignmentOperator) {
    constexpr FwSizeType capacity = 3;
    // Call the constructor providing backing storage
    FifoQueue<U32, capacity> q1;
    // Enqueue an item
    U32 value = 42;
    (void)q1.enqueue(value);
    // Call the default constructor
    FifoQueue<U32, capacity> q2;
    ASSERT_EQ(q2.getSize(), 0);
    // Call the copy assignment operator
    q2 = q1;
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(FifoQueue, DequeueOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    U32 items[capacity];
    FifoQueue<U32, capacity> queue;
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const U32 val = STest::Pick::any();
        // Enqueue it
        const auto status = queue.enqueue(val);
        items[i] = val;
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
    FifoQueue<U32, capacity> queue;
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
    FifoQueue<U32, maxSize> q1;
    // size1 < capacity2
    {
        FifoQueue<U32, maxSize> q2;
        testCopyDataFrom(q1, smallSize, q2);
    }
    // size1 == size2
    {
        FifoQueue<U32, maxSize> q2;
        testCopyDataFrom(q1, maxSize, q2);
    }
    // size1 > size2
    {
        FifoQueue<U32, smallSize> q2;
        testCopyDataFrom(q1, maxSize, q2);
    }
}

TEST(FifoQueueRules, EnqueueOK) {
    State::Queue queue;
    State state(queue);
    Rules::enqueueOK.apply(state);
}

TEST(FifoQueueRules, EnqueueFull) {
    State::Queue queue;
    State state(queue);
    for (FwSizeType i = 0; i < State::capacity; i++) {
        Rules::enqueueOK.apply(state);
    }
    Rules::enqueueFull.apply(state);
}

TEST(FifoQueueRules, At) {
    State::Queue queue;
    State state(queue);
    Rules::enqueueOK.apply(state);
    Rules::at.apply(state);
}

TEST(FifoQueueRules, DequeueOK) {
    State::Queue queue;
    State state(queue);
    Rules::enqueueOK.apply(state);
    Rules::dequeueOK.apply(state);
}

TEST(FifoQueueRules, DequeueEmpty) {
    State::Queue queue;
    State state(queue);
    Rules::dequeueEmpty.apply(state);
}

TEST(FifoQueueRules, Clear) {
    State::Queue queue;
    State state(queue);
    Rules::enqueueOK.apply(state);
    ASSERT_EQ(state.queue.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.queue.getSize(), 0);
}

TEST(FifoQueueScenarios, Random) {
    State::Queue queue;
    State state(queue);
    Scenarios::random(Fw::String("FifoQueueRandom"), state, 1000);
}

}  // namespace FifoQueueTest

}  // namespace Fw
