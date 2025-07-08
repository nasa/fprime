// ======================================================================
// \title  FifoQueueTest.cpp
// \author bocchino
// \brief  cpp file for FifoQueue tests
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/FifoQueueTestRules.hpp"
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

using Queue = FifoQueue<State::ItemType, State::capacity>;
using QueueTester = FifoQueueTester<State::ItemType, State::capacity>;

TEST(FifoQueue, ZeroArgConstructor) {
    Queue queue;
    ASSERT_EQ(queue.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(FifoQueue, CopyConstructor) {
    // Construct q1
    Queue q1;
    // Enqueue an item
    const auto item = State::getRandomItem();
    const auto status = q1.enqueue(item);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(q1.getSize(), 1);
    // Use the copy constructor to construct q2
    Queue q2(q1);
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(FifoQueue, EnqueueOK) {
    const FwSizeType size = STest::Pick::lowerUpper(1, State::capacity);
    Queue queue;
    ASSERT_EQ(queue.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(queue.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const auto item = State::getRandomItem();
        // Enqueue it
        auto status = queue.enqueue(item);
        ASSERT_EQ(status, Success::SUCCESS);
        // Peek it
        State::ItemType item1 = 0;
        status = queue.peek(item1, i);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(item1, item);
        // Check the size
        ASSERT_EQ(queue.getSize(), i + 1);
    }
    queue.clear();
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(FifoQueue, EnqueueFull) {
    Queue queue;
    // Fill up the FIFO
    for (FwSizeType i = 0; i < State::capacity; i++) {
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
    // Call the constructor providing backing storage
    Queue q1;
    // Enqueue an item
    const auto item = State::getRandomItem();
    (void)q1.enqueue(item);
    // Call the default constructor
    Queue q2;
    ASSERT_EQ(q2.getSize(), 0);
    // Call the copy assignment operator
    q2 = q1;
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(FifoQueue, DequeueOK) {
    const FwSizeType size = STest::Pick::lowerUpper(1, State::capacity);
    State::ItemType items[State::capacity];
    Queue queue;
    ASSERT_EQ(queue.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(queue.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const auto item = State::getRandomItem();
        // Enqueue it
        const auto status = queue.enqueue(item);
        items[i] = item;
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(queue.getSize(), i + 1);
    }
    for (FwSizeType i = 0; i < size; i++) {
        State::ItemType item = 0;
        // Peek
        auto status = queue.peek(item);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(item, items[i]);
        // Dequeue it
        status = queue.dequeue(item);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(item, items[i]);
        ASSERT_EQ(queue.getSize(), size - i - 1);
    }
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(FifoQueue, DequeueEmpty) {
    Queue queue;
    State::ItemType item = 0;
    const auto status = queue.dequeue(item);
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(FifoQueue, CopyDataFrom) {
    constexpr FwSizeType maxSize = State::capacity;
    constexpr FwSizeType smallSize = maxSize / 2;
    Queue q1;
    // size1 < capacity2
    {
        Queue q2;
        State::testCopyDataFrom(q1, smallSize, q2);
    }
    // size1 == capacity2
    {
        Queue q2;
        State::testCopyDataFrom(q1, maxSize, q2);
    }
    // size1 > capacity2
    {
        FifoQueue<State::ItemType, smallSize> q2;
        State::testCopyDataFrom(q1, maxSize, q2);
    }
}

TEST(FifoQueueTestScenarios, At) {
    State::Queue queue;
    State state(queue);
    Scenarios::at(state);
}

TEST(FifoQueueTestScenarios, Clear) {
    State::Queue queue;
    State state(queue);
    Scenarios::clear(state);
}

TEST(FifoQueueTestScenarios, DequeueEmpty) {
    State::Queue queue;
    State state(queue);
    Scenarios::dequeueEmpty(state);
}

TEST(FifoQueueTestScenarios, DequeueOK) {
    State::Queue queue;
    State state(queue);
    Scenarios::dequeueOK(state);
}

TEST(FifoQueueTestScenarios, EnqueueFull) {
    State::Queue queue;
    State state(queue);
    Scenarios::enqueueFull(state);
}

TEST(FifoQueueTestScenarios, EnqueueOK) {
    State::Queue queue;
    State state(queue);
    Scenarios::enqueueOK(state);
}

TEST(FifoQueueTestScenarios, Peek) {
    State::Queue queue;
    State state(queue);
    Scenarios::enqueueOK(state);
    Scenarios::enqueueOK(state);
    Scenarios::peek(state);
}

TEST(FifoQueueScenarios, Random) {
    State::Queue queue;
    State state(queue);
    Scenarios::random(Fw::String("FifoQueueRandom"), state, 1000);
}

}  // namespace FifoQueueTest

}  // namespace Fw
