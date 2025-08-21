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
