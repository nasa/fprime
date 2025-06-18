// ======================================================================
// \title  ExternalStackTest.cpp
// \author bocchino
// \brief  cpp file for ExternalStack tests
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/StackTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/StackTestScenarios.hpp"

namespace Fw {

template <typename T>
class ExternalStackTester {
  public:
    ExternalStackTester<T>(const ExternalStack<T>& stack) : m_stack(stack) {}

    const ExternalArray<T> getItems() const { return this->m_stack.m_items; }

  private:
    const ExternalStack<T>& m_stack;
};

namespace StackTest {

#if 0
TEST(ExternalStack, ZeroArgConstructor) {
    ExternalStack<U32> stack;
    ASSERT_EQ(stack.getCapacity(), 0);
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, TypedStorageConstructor) {
    constexpr FwSizeType capacity = 10;
    U32 items[capacity];
    ExternalStack<U32> stack(items, capacity);
    ExternalStackTester<U32> tester(stack);
    ASSERT_EQ(tester.getItems().getElements(), items);
    ASSERT_EQ(stack.getCapacity(), capacity);
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, UntypedStorageConstructor) {
    constexpr FwSizeType capacity = 10;
    constexpr U8 alignment = ExternalStack<U32>::getByteArrayAlignment();
    constexpr FwSizeType byteArraySize = ExternalStack<U32>::getByteArraySize(capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    ExternalStack<U32> stack(ByteArray(&bytes[0], sizeof bytes), capacity);
    ExternalStackTester<U32> tester(stack);
    ASSERT_EQ(tester.getItems().getElements(), reinterpret_cast<U32*>(bytes));
    ASSERT_EQ(stack.getCapacity(), capacity);
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, EnstackOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    U32 elts[capacity];
    ExternalStack<U32> stack(elts, capacity);
    ASSERT_EQ(stack.getCapacity(), capacity);
    ASSERT_EQ(stack.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const U32 val = STest::Pick::any();
        // Enstack it
        auto status = stack.enstack(val);
        ASSERT_EQ(status, Success::SUCCESS);
        // Peek it
        U32 val1 = 0;
        status = stack.peek(val1, i);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val1, val);
        // Check the size
        ASSERT_EQ(stack.getSize(), i + 1);
    }
    stack.clear();
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, EnstackFull) {
    constexpr const FwSizeType capacity = 1000;
    U32 elts[capacity];
    ExternalStack<U32> stack(elts, capacity);
    // Fill up the FIFO
    for (FwSizeType i = 0; i < capacity; i++) {
        const auto status = stack.enstack(0);
        ASSERT_EQ(status, Success::SUCCESS);
    }
    // Now try to push another element
    const U32 val = STest::Pick::any();
    const auto status = stack.enstack(val);
    // Push should fail
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(ExternalStack, CopyConstructor) {
    constexpr FwSizeType capacity = 3;
    U32 items[capacity];
    // Call the constructor providing backing storage
    ExternalStack<U32> q1(items, capacity);
    // Enstack an item
    U32 value = 42;
    (void)q1.enstack(value);
    // Call the copy constructor
    ExternalStack<U32> q2(q1);
    ExternalStackTester<U32> tester1(q1);
    ExternalStackTester<U32> tester2(q2);
    ASSERT_EQ(tester2.getItems().getElements(), items);
    ASSERT_EQ(tester2.getItems().getSize(), capacity);
    ASSERT_EQ(tester2.getEnstackIndex().getValue(), 1);
    ASSERT_EQ(tester2.getDestackIndex().getValue(), 0);
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(ExternalStack, CopyAssignmentOperator) {
    constexpr FwSizeType capacity = 3;
    U32 items[capacity];
    // Call the constructor providing backing storage
    ExternalStack<U32> q1(items, capacity);
    // Enstack an item
    U32 value = 42;
    (void)q1.enstack(value);
    // Call the default constructor
    ExternalStack<U32> q2;
    ASSERT_EQ(q2.getSize(), 0);
    // Call the copy assignment operator
    q2 = q1;
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(ExternalStack, DestackOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    U32 items[capacity];
    ExternalStack<U32> stack(items, capacity);
    ASSERT_EQ(stack.getCapacity(), capacity);
    ASSERT_EQ(stack.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const U32 val = STest::Pick::any();
        // Enstack it
        const auto status = stack.enstack(val);
        ASSERT_EQ(val, items[i]);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(stack.getSize(), i + 1);
    }
    for (FwSizeType i = 0; i < size; i++) {
        U32 val = 0;
        // Peek
        auto status = stack.peek(val);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val, items[i]);
        // Destack it
        status = stack.destack(val);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val, items[i]);
        ASSERT_EQ(stack.getSize(), size - i - 1);
    }
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, DestackEmpty) {
    constexpr const FwSizeType capacity = 1000;
    U32 items[capacity];
    ExternalStack<U32> stack(items, capacity);
    U32 val = 0;
    const auto status = stack.destack(val);
    ASSERT_EQ(status, Success::FAILURE);
}

namespace {

void testCopyDataFrom(StackBase<U32>& q1, FwSizeType size1, StackBase<U32>& q2) {
    q1.clear();
    for (FwSizeType i = 0; i < size1; i++) {
        const auto status = q1.enstack(static_cast<U32>(i));
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

TEST(ExternalStack, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    U32 items1[maxSize];
    U32 items2[maxSize];
    ExternalStack<U32> q1(items1, maxSize);
    // size1 < capacity2
    {
        ExternalStack<U32> q2(items2, maxSize);
        testCopyDataFrom(q1, smallSize, q2);
    }
    // size1 == size2
    {
        ExternalStack<U32> q2(items2, maxSize);
        testCopyDataFrom(q1, maxSize, q2);
    }
    // size1 > size2
    {
        ExternalStack<U32> q2(items2, smallSize);
        testCopyDataFrom(q1, maxSize, q2);
    }
}

TEST(ExternalStackRules, EnstackOK) {
    U32 items[State::capacity];
    State::ExternalQueue stack(items, State::capacity);
    State state(stack);
    Rules::enstackOK.apply(state);
}

TEST(ExternalStackRules, EnstackFull) {
    U32 items[State::capacity];
    State::ExternalQueue stack(items, State::capacity);
    State state(stack);
    for (FwSizeType i = 0; i < State::capacity; i++) {
        Rules::enstackOK.apply(state);
    }
    Rules::enstackFull.apply(state);
}

TEST(ExternalStackRules, At) {
    U32 items[State::capacity];
    State::ExternalQueue stack(items, State::capacity);
    State state(stack);
    Rules::enstackOK.apply(state);
    Rules::at.apply(state);
}

TEST(ExternalStackRules, DestackOK) {
    U32 items[State::capacity];
    State::ExternalQueue stack(items, State::capacity);
    State state(stack);
    Rules::enstackOK.apply(state);
    Rules::destackOK.apply(state);
}

TEST(ExternalStackRules, DestackEmpty) {
    U32 items[State::capacity];
    State::ExternalQueue stack(items, State::capacity);
    State state(stack);
    Rules::destackEmpty.apply(state);
}

TEST(ExternalStackRules, Clear) {
    U32 items[State::capacity];
    State::ExternalQueue stack(items, State::capacity);
    State state(stack);
    Rules::enstackOK.apply(state);
    ASSERT_EQ(state.stack.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.stack.getSize(), 0);
}

TEST(ExternalStackScenarios, Random) {
    U32 items[State::capacity];
    State::ExternalQueue stack(items, State::capacity);
    State state(stack);
    Scenarios::random(Fw::String("ExternalStackRandom"), state, 1000);
}
#endif

}  // namespace StackTest
}  // namespace Fw
