// ======================================================================
// \title  ExternalFifoQueueTest.cpp
// \author bocchino
// \brief  cpp file for ExternalFifoQueue tests
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/FifoQueueTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/FifoQueueTestScenarios.hpp"

namespace Fw {

template <typename T>
class ExternalFifoQueueTester {
  public:
    ExternalFifoQueueTester<T>(const ExternalFifoQueue<T>& queue) : m_queue(queue) {}

    const ExternalArray<T>& getItems() const { return this->m_queue.m_items; }

    const CircularIndex& getEnqueueIndex() const { return this->m_queue.m_enqueueIndex; }

    const CircularIndex& getDequeueIndex() const { return this->m_queue.m_dequeueIndex; }

  private:
    const ExternalFifoQueue<T>& m_queue;
};

namespace FifoQueueTest {

using Queue = ExternalFifoQueue<State::ItemType>;
using QueueTester = ExternalFifoQueueTester<State::ItemType>;

TEST(ExternalFifoQueue, ZeroArgConstructor) {
    Queue queue;
    ASSERT_EQ(queue.getCapacity(), 0);
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(ExternalFifoQueue, TypedStorageConstructor) {
    State::ItemType items[State::capacity];
    Queue queue(items, State::capacity);
    QueueTester tester(queue);
    ASSERT_EQ(tester.getItems().getElements(), items);
    ASSERT_EQ(queue.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(ExternalFifoQueue, UntypedStorageConstructor) {
    constexpr U8 alignment = Queue::getByteArrayAlignment();
    constexpr FwSizeType byteArraySize = Queue::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    Queue queue(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    QueueTester tester(queue);
    ASSERT_EQ(tester.getItems().getElements(), reinterpret_cast<State::ItemType*>(bytes));
    ASSERT_EQ(queue.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(ExternalFifoQueue, CopyConstructor) {
    State::ItemType items[State::capacity];
    // Call the constructor providing backing storage
    Queue q1(items, State::capacity);
    // Enqueue an item
    State::ItemType item = State::getRandomItem();
    const auto status = q1.enqueue(item);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    Queue q2(q1);
    QueueTester tester1(q1);
    QueueTester tester2(q2);
    ASSERT_EQ(tester2.getItems().getElements(), items);
    ASSERT_EQ(tester2.getItems().getSize(), FwSizeType(State::capacity));
    ASSERT_EQ(tester2.getEnqueueIndex().getValue(), 1);
    ASSERT_EQ(tester2.getDequeueIndex().getValue(), 0);
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(ExternalFifoQueue, CopyAssignmentOperator) {
    State::ItemType items[State::capacity];
    // Call the constructor providing backing storage
    Queue q1(items, State::capacity);
    // Enqueue an item
    const State::ItemType item = State::getRandomItem();
    const auto status = q1.enqueue(item);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    Queue q2;
    ASSERT_EQ(q2.getSize(), 0);
    // Call the copy assignment operator
    q2 = q1;
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(ExternalFifoQueue, CopyDataFrom) {
    constexpr FwSizeType maxSize = State::capacity;
    constexpr FwSizeType smallSize = maxSize / 2;
    State::ItemType items1[maxSize];
    State::ItemType items2[maxSize];
    Queue q1(items1, maxSize);
    // size1 < capacity2
    {
        Queue q2(items2, maxSize);
        State::testCopyDataFrom(q1, smallSize, q2);
    }
    // size1 == capacity2
    {
        Queue q2(items2, maxSize);
        State::testCopyDataFrom(q1, maxSize, q2);
    }
    // size1 > capacity2
    {
        Queue q2(items2, smallSize);
        State::testCopyDataFrom(q1, maxSize, q2);
    }
}

TEST(ExternalFifoQueueScenarios, At) {
    State::ItemType items[State::capacity];
    State::ExternalQueue queue(items, State::capacity);
    State state(queue);
    Scenarios::enqueueOK(state);
}

TEST(ExternalFifoQueueScenarios, Clear) {
    State::ItemType items[State::capacity];
    State::ExternalQueue queue(items, State::capacity);
    State state(queue);
    Scenarios::clear(state);
}

TEST(ExternalFifoQueueScenarios, DequeueEmpty) {
    State::ItemType items[State::capacity];
    State::ExternalQueue queue(items, State::capacity);
    State state(queue);
    Scenarios::dequeueEmpty(state);
}

TEST(ExternalFifoQueueScenarios, DequeueOK) {
    State::ItemType items[State::capacity];
    State::ExternalQueue queue(items, State::capacity);
    State state(queue);
    Scenarios::dequeueOK(state);
}

TEST(ExternalFifoQueueScenarios, EnqueueFull) {
    State::ItemType items[State::capacity];
    State::ExternalQueue queue(items, State::capacity);
    State state(queue);
    Scenarios::enqueueFull(state);
}

TEST(ExternalFifoQueueScenarios, EnqueueOK) {
    State::ItemType items[State::capacity];
    State::ExternalQueue queue(items, State::capacity);
    State state(queue);
    Rules::enqueueOK.apply(state);
}

TEST(ExternalFifoQueueTestScenarios, Peek) {
    State::ItemType items[State::capacity];
    State::ExternalQueue queue(items, State::capacity);
    State state(queue);
    Scenarios::enqueueOK(state);
    Scenarios::enqueueOK(state);
    Scenarios::peek(state);
}

TEST(ExternalFifoQueueScenarios, Random) {
    State::ItemType items[State::capacity];
    State::ExternalQueue queue(items, State::capacity);
    State state(queue);
    Scenarios::random(Fw::String("ExternalFifoQueueRandom"), state, 1000);
}

}  // namespace FifoQueueTest
}  // namespace Fw
