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

TEST(ExternalStack, At) {
    constexpr FwSizeType capacity = 10;
    U32 items[capacity];
    ExternalStack<U32> stack(items, capacity);
    auto status = stack.push(3);
    ASSERT_EQ(status, Success::SUCCESS);
    status = stack.push(4);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(stack.at(0), 4);
    ASSERT_EQ(stack.at(1), 3);
    ASSERT_DEATH(stack.at(2), "Assert");
}

TEST(ExternalStack, PushOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    U32 elts[capacity];
    ExternalStack<U32> stack(elts, capacity);
    ASSERT_EQ(stack.getCapacity(), capacity);
    ASSERT_EQ(stack.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const U32 val = STest::Pick::any();
        // Push it
        auto status = stack.push(val);
        ASSERT_EQ(status, Success::SUCCESS);
        // Peek it
        U32 val1 = 0;
        status = stack.peek(val1);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val1, val);
        // Check the size
        ASSERT_EQ(stack.getSize(), i + 1);
    }
    stack.clear();
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, PushFull) {
    constexpr const FwSizeType capacity = 1000;
    U32 elts[capacity];
    ExternalStack<U32> stack(elts, capacity);
    // Fill up the stack
    for (FwSizeType i = 0; i < capacity; i++) {
        const auto status = stack.push(0);
        ASSERT_EQ(status, Success::SUCCESS);
    }
    // Now try to push another element
    const U32 val = STest::Pick::any();
    const auto status = stack.push(val);
    // Push should fail
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(ExternalStack, CopyConstructor) {
    constexpr FwSizeType capacity = 3;
    U32 items[capacity];
    // Call the constructor providing backing storage
    ExternalStack<U32> s1(items, capacity);
    // Push an item
    U32 value = 42;
    const auto status = s1.push(value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    ExternalStack<U32> s2(s1);
    ExternalStackTester<U32> tester1(s1);
    ExternalStackTester<U32> tester2(s2);
    ASSERT_EQ(tester2.getItems().getElements(), items);
    ASSERT_EQ(tester2.getItems().getSize(), capacity);
    ASSERT_EQ(s2.getSize(), 1);
}

TEST(ExternalStack, CopyAssignmentOperator) {
    constexpr FwSizeType capacity = 3;
    U32 items[capacity];
    // Call the constructor providing backing storage
    ExternalStack<U32> s1(items, capacity);
    // Push an item
    U32 value = 42;
    const auto status = s1.push(value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    ExternalStack<U32> s2;
    ASSERT_EQ(s2.getSize(), 0);
    // Call the copy assignment operator
    s2 = s1;
    ASSERT_EQ(s2.getSize(), 1);
}

TEST(ExternalStack, PopOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    U32 items[capacity];
    ExternalStack<U32> stack(items, capacity);
    ASSERT_EQ(stack.getCapacity(), capacity);
    ASSERT_EQ(stack.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const U32 val = STest::Pick::any();
        // Push it
        const auto status = stack.push(val);
        ASSERT_EQ(val, items[i]);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(stack.getSize(), i + 1);
    }
    for (FwSizeType i = 0; i < size; i++) {
        const FwSizeType stackIndex = size - 1 - i;
        U32 val = 0;
        // Peek
        auto status = stack.peek(val);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val, items[stackIndex]);
        // Pop it
        status = stack.pop(val);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(val, items[stackIndex]);
        ASSERT_EQ(stack.getSize(), stackIndex);
    }
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, PopEmpty) {
    constexpr const FwSizeType capacity = 1000;
    U32 items[capacity];
    ExternalStack<U32> stack(items, capacity);
    U32 val = 0;
    const auto status = stack.pop(val);
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(ExternalStack, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    U32 items1[maxSize];
    U32 items2[maxSize];
    ExternalStack<U32> s1(items1, maxSize);
    // size1 < capacity2
    {
        ExternalStack<U32> s2(items2, maxSize);
        State::testCopyDataFrom(s1, smallSize, s2);
    }
    // size1 == size2
    {
        ExternalStack<U32> s2(items2, maxSize);
        State::testCopyDataFrom(s1, maxSize, s2);
    }
    // size1 > size2
    {
        ExternalStack<U32> s2(items2, smallSize);
        State::testCopyDataFrom(s1, maxSize, s2);
    }
}

TEST(ExternalStackScenarios, PushOK) {
    U32 items[State::capacity];
    State::ExternalStack stack(items, State::capacity);
    State state(stack);
    Rules::pushOK.apply(state);
}

TEST(ExternalStackScenarios, PushFull) {
    U32 items[State::capacity];
    State::ExternalStack stack(items, State::capacity);
    State state(stack);
    for (FwSizeType i = 0; i < State::capacity; i++) {
        Rules::pushOK.apply(state);
    }
    Rules::pushFull.apply(state);
}

TEST(ExternalStackScenarios, At) {
    U32 items[State::capacity];
    State::ExternalStack stack(items, State::capacity);
    State state(stack);
    for (FwSizeType i = 0; i < State::capacity; i++) {
        Rules::pushOK.apply(state);
    }
    Rules::at.apply(state);
}

TEST(ExternalStackScenarios, PopOK) {
    U32 items[State::capacity];
    State::ExternalStack stack(items, State::capacity);
    State state(stack);
    Rules::pushOK.apply(state);
    Rules::pushOK.apply(state);
    Rules::popOK.apply(state);
    Rules::popOK.apply(state);
}

TEST(ExternalStackScenarios, PopEmpty) {
    U32 items[State::capacity];
    State::ExternalStack stack(items, State::capacity);
    State state(stack);
    Rules::popEmpty.apply(state);
}

TEST(ExternalStackScenarios, Clear) {
    U32 items[State::capacity];
    State::ExternalStack stack(items, State::capacity);
    State state(stack);
    Rules::pushOK.apply(state);
    Rules::clear.apply(state);
}

TEST(ExternalStackScenarios, Random) {
    U32 items[State::capacity];
    State::ExternalStack stack(items, State::capacity);
    State state(stack);
    Scenarios::random(Fw::String("ExternalStackRandom"), state, 1000);
}

}  // namespace StackTest
}  // namespace Fw
