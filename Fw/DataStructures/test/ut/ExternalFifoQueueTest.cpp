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
    constexpr FwSizeType capacity = 10;
    State::ItemType items[capacity];
    Queue queue(items, capacity);
    QueueTester tester(queue);
    ASSERT_EQ(tester.getItems().getElements(), items);
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(ExternalFifoQueue, UntypedStorageConstructor) {
    constexpr FwSizeType capacity = 10;
    constexpr U8 alignment = Queue::getByteArrayAlignment();
    constexpr FwSizeType byteArraySize = Queue::getByteArraySize(capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    Queue queue(ByteArray(&bytes[0], sizeof bytes), capacity);
    QueueTester tester(queue);
    ASSERT_EQ(tester.getItems().getElements(), reinterpret_cast<State::ItemType*>(bytes));
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(ExternalFifoQueue, EnqueueOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    State::ItemType elts[capacity];
    Queue queue(elts, capacity);
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick an item
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

TEST(ExternalFifoQueue, EnqueueFull) {
    constexpr const FwSizeType capacity = 1000;
    State::ItemType elts[capacity];
    Queue queue(elts, capacity);
    // Fill up the FIFO
    for (FwSizeType i = 0; i < capacity; i++) {
        const auto status = queue.enqueue(0);
        ASSERT_EQ(status, Success::SUCCESS);
    }
    // Now try to push another element
    const auto item = State::getRandomItem();
    const auto status = queue.enqueue(item);
    // Push should fail
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(ExternalFifoQueue, CopyConstructor) {
    constexpr FwSizeType capacity = 3;
    State::ItemType items[capacity];
    // Call the constructor providing backing storage
    Queue q1(items, capacity);
    // Enqueue an item
    State::ItemType item = State::getRandomItem();
    (void)q1.enqueue(item);
    // Call the copy constructor
    Queue q2(q1);
    QueueTester tester1(q1);
    QueueTester tester2(q2);
    ASSERT_EQ(tester2.getItems().getElements(), items);
    ASSERT_EQ(tester2.getItems().getSize(), capacity);
    ASSERT_EQ(tester2.getEnqueueIndex().getValue(), 1);
    ASSERT_EQ(tester2.getDequeueIndex().getValue(), 0);
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(ExternalFifoQueue, CopyAssignmentOperator) {
    constexpr FwSizeType capacity = 3;
    State::ItemType items[capacity];
    // Call the constructor providing backing storage
    Queue q1(items, capacity);
    // Enqueue an item
    State::ItemType item = State::getRandomItem();
    (void)q1.enqueue(item);
    // Call the default constructor
    Queue q2;
    ASSERT_EQ(q2.getSize(), 0);
    // Call the copy assignment operator
    q2 = q1;
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(ExternalFifoQueue, DequeueOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    State::ItemType items[capacity];
    Queue queue(items, capacity);
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick an item
        const auto item = State::getRandomItem();
        // Enqueue it
        const auto status = queue.enqueue(item);
        ASSERT_EQ(item, items[i]);
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

TEST(ExternalFifoQueue, DequeueEmpty) {
    constexpr const FwSizeType capacity = 1000;
    State::ItemType items[capacity];
    Queue queue(items, capacity);
    State::ItemType item = 0;
    const auto status = queue.dequeue(item);
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(ExternalFifoQueue, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    State::ItemType items1[maxSize];
    State::ItemType items2[maxSize];
    Queue q1(items1, maxSize);
    // size1 < capacity2
    {
        Queue q2(items2, maxSize);
        State::testCopyDataFrom(q1, smallSize, q2);
    }
    // size1 == size2
    {
        Queue q2(items2, maxSize);
        State::testCopyDataFrom(q1, maxSize, q2);
    }
    // size1 > size2
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

TEST(ExternalFifoQueueScenarios, Random) {
    State::ItemType items[State::capacity];
    State::ExternalQueue queue(items, State::capacity);
    State state(queue);
    Scenarios::random(Fw::String("ExternalFifoQueueRandom"), state, 1000);
}

}  // namespace FifoQueueTest
}  // namespace Fw
