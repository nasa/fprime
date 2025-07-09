// ======================================================================
// \title  ExternalStackTest.cpp
// \author bocchino
// \brief  cpp file for ExternalStack tests
// ======================================================================

#include "Fw/DataStructures/ExternalStack.hpp"
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

using TestStack = ExternalStack<State::ItemType>;
using StackTester = ExternalStackTester<State::ItemType>;

TEST(ExternalStack, ZeroArgConstructor) {
    TestStack stack;
    ASSERT_EQ(stack.getCapacity(), 0);
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, TypedStorageConstructor) {
    State::ItemType items[State::capacity];
    TestStack stack(items, State::capacity);
    StackTester tester(stack);
    ASSERT_EQ(tester.getItems().getElements(), items);
    ASSERT_EQ(stack.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, UntypedStorageConstructor) {
    constexpr U8 alignment = TestStack::getByteArrayAlignment();
    constexpr FwSizeType byteArraySize = TestStack::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    TestStack stack(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    StackTester tester(stack);
    ASSERT_EQ(tester.getItems().getElements(), reinterpret_cast<State::ItemType*>(bytes));
    ASSERT_EQ(stack.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(stack.getSize(), 0);
}

TEST(ExternalStack, CopyConstructor) {
    State::ItemType items[State::capacity];
    // Call the constructor providing backing storage
    TestStack s1(items, State::capacity);
    // Push an item
    const auto item = State::getRandomItem();
    const auto status = s1.push(item);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    TestStack s2(s1);
    StackTester tester1(s1);
    StackTester tester2(s2);
    ASSERT_EQ(tester2.getItems().getElements(), items);
    ASSERT_EQ(tester2.getItems().getSize(), FwSizeType(State::capacity));
    ASSERT_EQ(s2.getSize(), 1);
}

TEST(ExternalStack, CopyAssignmentOperator) {
    State::ItemType items[State::capacity];
    // Call the constructor providing backing storage
    TestStack s1(items, State::capacity);
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

TEST(ExternalStack, CopyDataFrom) {
    constexpr FwSizeType maxSize = State::capacity;
    constexpr FwSizeType smallSize = maxSize / 2;
    State::ItemType items1[maxSize];
    State::ItemType items2[maxSize];
    TestStack s1(items1, maxSize);
    // size1 < capacity2
    {
        TestStack s2(items2, maxSize);
        State::testCopyDataFrom(s1, smallSize, s2);
    }
    // size1 == capacity2
    {
        TestStack s2(items2, maxSize);
        State::testCopyDataFrom(s1, maxSize, s2);
    }
    // size1 > capacity2
    {
        ExternalStack<State::ItemType> s2(items2, smallSize);
        State::testCopyDataFrom(s1, maxSize, s2);
    }
}

TEST(ExternalStackScenarios, At) {
    State::ItemType items[State::capacity];
    TestStack stack(items, State::capacity);
    State state(stack);
    Scenarios::at(state);
}

TEST(ExternalStackScenarios, Clear) {
    State::ItemType items[State::capacity];
    TestStack stack(items, State::capacity);
    State state(stack);
    Scenarios::clear(state);
}

TEST(ExternalStackScenarios, Peek) {
    State::ItemType items[State::capacity];
    TestStack stack(items, State::capacity);
    State state(stack);
    Scenarios::peek(state);
}

TEST(ExternalStackScenarios, PopEmpty) {
    State::ItemType items[State::capacity];
    TestStack stack(items, State::capacity);
    State state(stack);
    Rules::popEmpty.apply(state);
}

TEST(ExternalStackScenarios, PopOK) {
    State::ItemType items[State::capacity];
    TestStack stack(items, State::capacity);
    State state(stack);
    Scenarios::popOK(state);
}

TEST(ExternalStackScenarios, PushFull) {
    State::ItemType items[State::capacity];
    TestStack stack(items, State::capacity);
    State state(stack);
    Scenarios::pushFull(state);
}

TEST(ExternalStackScenarios, PushOK) {
    State::ItemType items[State::capacity];
    TestStack stack(items, State::capacity);
    State state(stack);
    Scenarios::pushOK(state);
}

TEST(ExternalStackScenarios, Random) {
    State::ItemType items[State::capacity];
    TestStack stack(items, State::capacity);
    State state(stack);
    Scenarios::random(Fw::String("ExternalStackRandom"), state, 1000);
}

}  // namespace StackTest
}  // namespace Fw
