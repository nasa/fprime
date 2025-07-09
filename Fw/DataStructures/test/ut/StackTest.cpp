// ======================================================================
// \title  StackTest.cpp
// \author bocchino
// \brief  cpp file for Stack tests
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/StackTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/StackTestScenarios.hpp"

namespace Fw {

template <typename T, FwSizeType C>
class StackTester {
  public:
    StackTester(const Stack<T, C>& stack) : m_stack(stack) {}

    const ExternalStack<T> getExtStack() const { return this->m_stack.extStack; }

    const typename Array<T, C>::Elements& getItems() const { return this->m_stack.m_items; }

  private:
    const Stack<T, C>& m_stack;
};

namespace StackTest {

TEST(Stack, ZeroArgConstructor) {
    constexpr FwSizeType C = 10;
    Stack<U32, C> stack;
    ASSERT_EQ(stack.getCapacity(), C);
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(Stack, CopyConstructor) {
    constexpr FwSizeType capacity = 3;
    // Construct s1
    Stack<U32, capacity> s1;
    // Push an item
    U32 value = 42;
    const auto status = s1.push(value);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(s1.getSize(), 1);
    // Use the copy constructor to construct s2
    Stack<U32, capacity> s2(s1);
    ASSERT_EQ(s2.getSize(), 1);
}

TEST(Stack, PushOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    Stack<U32, capacity> stack;
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

TEST(Stack, PushFull) {
    constexpr const FwSizeType capacity = 1000;
    Stack<U32, capacity> stack;
    // Fill up the FIFO
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

TEST(Stack, CopyAssignmentOperator) {
    constexpr FwSizeType capacity = 3;
    // Call the constructor providing backing storage
    Stack<U32, capacity> s1;
    // Push an item
    U32 value = 42;
    (void)s1.push(value);
    // Call the default constructor
    Stack<U32, capacity> s2;
    ASSERT_EQ(s2.getSize(), 0);
    // Call the copy assignment operator
    s2 = s1;
    ASSERT_EQ(s2.getSize(), 1);
}

TEST(Stack, PopOK) {
    constexpr const FwSizeType capacity = 1000;
    const FwSizeType size = STest::Pick::lowerUpper(1, capacity);
    U32 items[capacity];
    Stack<U32, capacity> stack;
    ASSERT_EQ(stack.getCapacity(), capacity);
    ASSERT_EQ(stack.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const U32 val = STest::Pick::any();
        // Push it
        const auto status = stack.push(val);
        items[i] = val;
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

TEST(Stack, PopEmpty) {
    constexpr const FwSizeType capacity = 1000;
    Stack<U32, capacity> stack;
    U32 val = 0;
    const auto status = stack.pop(val);
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(Stack, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    Stack<U32, maxSize> s1;
    // size1 < capacity2
    {
        Stack<U32, maxSize> s2;
        State::testCopyDataFrom(s1, smallSize, s2);
    }
    // size1 == size2
    {
        Stack<U32, maxSize> s2;
        State::testCopyDataFrom(s1, maxSize, s2);
    }
    // size1 > size2
    {
        Stack<U32, smallSize> s2;
        State::testCopyDataFrom(s1, maxSize, s2);
    }
}

TEST(StackScenarios, PushOK) {
    State::Stack stack;
    State state(stack);
    Rules::pushOK.apply(state);
}

TEST(StackScenarios, PushFull) {
    State::Stack stack;
    State state(stack);
    for (FwSizeType i = 0; i < State::capacity; i++) {
        Rules::pushOK.apply(state);
    }
    Rules::pushFull.apply(state);
}

TEST(StackScenarios, At) {
    State::Stack stack;
    State state(stack);
    Rules::pushOK.apply(state);
    for (FwSizeType i = 0; i < State::capacity; i++) {
        Rules::at.apply(state);
    }
}

TEST(StackScenarios, Peek) {
    State::Stack stack;
    State state(stack);
    Rules::pushOK.apply(state);
    Rules::pushOK.apply(state);
    Rules::peek.apply(state);
}

TEST(StackScenarios, PopOK) {
    State::Stack stack;
    State state(stack);
    Rules::pushOK.apply(state);
    Rules::popOK.apply(state);
}

TEST(StackScenarios, PopEmpty) {
    State::Stack stack;
    State state(stack);
    Rules::popEmpty.apply(state);
}

TEST(StackScenarios, Clear) {
    State::Stack stack;
    State state(stack);
    Rules::pushOK.apply(state);
    Rules::clear.apply(state);
}

TEST(StackScenarios, Random) {
    State::Stack stack;
    State state(stack);
    Scenarios::random(Fw::String("StackRandom"), state, 1000);
}

}  // namespace StackTest

}  // namespace Fw
