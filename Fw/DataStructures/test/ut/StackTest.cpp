// ======================================================================
// \title  StackTest.cpp
// \author bocchino
// \brief  cpp file for Stack tests
// ======================================================================

#include "Fw/DataStructures/Stack.hpp"
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

using TestStack = Stack<State::ItemType, State::capacity>;
using StackTester = StackTester<State::ItemType, State::capacity>;

TEST(Stack, ZeroArgConstructor) {
    TestStack stack;
    ASSERT_EQ(stack.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(Stack, CopyConstructor) {
    // Construct s1
    TestStack s1;
    // Push an item
    const auto item = State::getRandomItem();
    const auto status = s1.push(item);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(s1.getSize(), 1);
    // Use the copy constructor to construct s2
    TestStack s2(s1);
    ASSERT_EQ(s2.getSize(), 1);
}

TEST(Stack, PushOK) {
    const FwSizeType size = STest::Pick::lowerUpper(1, State::capacity);
    TestStack stack;
    ASSERT_EQ(stack.getCapacity(), FwSizeType(State::capacity));
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
    TestStack stack;
    // Fill up the stack
    for (FwSizeType i = 0; i < State::capacity; i++) {
        const auto status = stack.push(0);
        ASSERT_EQ(status, Success::SUCCESS);
    }
    // Now try to push another element
    const auto item = State::getRandomItem();
    const auto status = stack.push(item);
    // Push should fail
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(Stack, CopyAssignmentOperator) {
    // Call the constructor providing backing storage
    TestStack s1;
    // Push an item
    const auto item = State::getRandomItem();
    const auto status = s1.push(item);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    TestStack s2;
    ASSERT_EQ(s2.getSize(), 0);
    // Call the copy assignment operator
    s2 = s1;
    ASSERT_EQ(s2.getSize(), 1);
}

TEST(Stack, PopOK) {
    const FwSizeType size = STest::Pick::lowerUpper(1, State::capacity);
    State::ItemType items[State::capacity];
    TestStack stack;
    ASSERT_EQ(stack.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(stack.getSize(), 0);
    for (FwSizeType i = 0; i < size; i++) {
        // Pick a value
        const auto item = State::getRandomItem();
        // Push it
        const auto status = stack.push(item);
        items[i] = item;
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(stack.getSize(), i + 1);
    }
    for (FwSizeType i = 0; i < size; i++) {
        const FwSizeType stackIndex = size - 1 - i;
        State::ItemType item = 0;
        // Peek
        auto status = stack.peek(item);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(item, items[stackIndex]);
        // Pop it
        status = stack.pop(item);
        ASSERT_EQ(status, Success::SUCCESS);
        ASSERT_EQ(item, items[stackIndex]);
        ASSERT_EQ(stack.getSize(), stackIndex);
    }
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(Stack, PopEmpty) {
    TestStack stack;
    State::ItemType item = 0;
    const auto status = stack.pop(item);
    ASSERT_EQ(status, Success::FAILURE);
}

TEST(Stack, CopyDataFrom) {
    constexpr FwSizeType maxSize = State::capacity;
    constexpr FwSizeType smallSize = maxSize / 2;
    TestStack s1;
    // size1 < capacity2
    {
        TestStack s2;
        State::testCopyDataFrom(s1, smallSize, s2);
    }
    // size1 == capacity2
    {
        TestStack s2;
        State::testCopyDataFrom(s1, maxSize, s2);
    }
    // size1 > capacity2
    {
        Stack<State::ItemType, smallSize> s2;
        State::testCopyDataFrom(s1, maxSize, s2);
    }
}

TEST(StackScenarios, At) {
    TestStack stack;
    State state(stack);
    Scenarios::at(state);
}

TEST(StackScenarios, Clear) {
    TestStack stack;
    State state(stack);
    Scenarios::clear(state);
}

TEST(StackScenarios, Peek) {
    TestStack stack;
    State state(stack);
    Scenarios::peek(state);
}

TEST(StackScenarios, PopEmpty) {
    TestStack stack;
    State state(stack);
    Scenarios::popEmpty(state);
}

TEST(StackScenarios, PopOK) {
    TestStack stack;
    State state(stack);
    Scenarios::popOK(state);
}

TEST(StackScenarios, PushFull) {
    TestStack stack;
    State state(stack);
    Scenarios::pushFull(state);
}

TEST(StackScenarios, PushOK) {
    TestStack stack;
    State state(stack);
    Scenarios::pushOK(state);
}

TEST(StackScenarios, Random) {
    TestStack stack;
    State state(stack);
    Scenarios::random(Fw::String("StackRandom"), state, 1000);
}

}  // namespace StackTest

}  // namespace Fw
