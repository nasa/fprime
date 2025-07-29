// ======================================================================
// \title  RedBlackTreeSetOrMapImplTest.cpp
// \author bocchino
// \brief  cpp file for RedBlackTreeSetOrMapImpl tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/DataStructures/RedBlackTreeSetOrMapImpl.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/test/ut/RedBlackTreeSetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/RedBlackTreeSetOrMapImplTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/RedBlackTreeSetOrMapImplTestScenarios.hpp"

namespace Fw {

template <typename T>
class ExternalStackTester {
  public:
    ExternalStackTester<T>(const ExternalStack<T>& stack) : m_stack(stack) {}

    const ExternalArray<T> getItems() const { return this->m_stack.m_items; }

  private:
    const ExternalStack<T>& m_stack;
};

namespace RedBlackTreeSetOrMapImplTest {

using ImplTester = RedBlackTreeSetOrMapImplTester<State::KeyType, State::ValueType>;

TEST(RedBlackTreeSetOrMapImpl, ZeroArgConstructor) {
    State::Impl impl;
    ASSERT_EQ(impl.getCapacity(), 0U);
    ASSERT_EQ(impl.getSize(), 0U);
}

TEST(RedBlackTreeSetOrMapImpl, TypedStorageConstructor) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    ASSERT_EQ(state.tester.getNodes().getElements(), nodes);
    ExternalStackTester<ImplTester::Index> stackTester(state.tester.getFreeNodes());
    ASSERT_EQ(stackTester.getItems().getElements(), freeNodes);
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0U);
}

TEST(RedBlackTreeSetOrMapImpl, UntypedStorageConstructor) {
    constexpr auto alignment = State::Impl::getByteArrayAlignment();
    constexpr auto byteArraySize = State::Impl::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    State::Impl impl(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    State::Tester tester(impl);
    ASSERT_EQ(tester.getNodes().getElements(), reinterpret_cast<ImplTester::Node*>(bytes));
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0U);
}

#if 0
TEST(RedBlackTreeSetOrMapImpl, CopyConstructor) {
    State::Entry entries[State::capacity];
    // Call the constructor providing backing storage
    State::Impl impl1(entries, State::capacity);
    // Insert an item
    const State::KeyType key = 0;
    const State::ValueType value = 42;
    const auto status = impl1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    State::Impl impl2(impl1);
    State::Tester tester1(impl1);
    State::Tester tester2(impl2);
    ASSERT_EQ(tester2.getEntries().getElements(), entries);
    ASSERT_EQ(tester2.getEntries().getSize(), FwSizeType(State::capacity));
    ASSERT_EQ(impl2.getSize(), 1);
}

TEST(RedBlackTreeSetOrMapImpl, CopyAssignmentOperator) {
    State::Entry entries[State::capacity];
    // Call the constructor providing backing storage
    State::Impl impl1(entries, State::capacity);
    // Insert an item
    const State::KeyType key = 0;
    const State::ValueType value = 42;
    const auto status = impl1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    State::Impl impl2;
    ASSERT_EQ(impl2.getSize(), 0);
    // Call the copy assignment operator
    impl2 = impl1;
    ASSERT_EQ(impl2.getSize(), 1);
}
#endif

TEST(ArraySetOrMapImpl, IteratorConstruction) {
    State::Impl impl;
    State::Impl::ConstIterator it(impl);
}

#if 0
TEST(RedBlackTreeSetOrMapImpl, IteratorComparison) {
  // Test comparison in default case
  State::Impl::ConstIterator it1;
  State::Impl::ConstIterator it2;
  ASSERT_TRUE(it1.compareEqual(it2));
}

TEST(RedBlackTreeSetOrMapImplScenarios, Clear) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    ASSERT_EQ(state.impl.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.impl.getSize(), 0);
}
#endif

TEST(RedBlackTreeSetOrMapImplScenarios, Find) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    Rules::find.apply(state);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::find.apply(state);
}

TEST(RedBlackTreeSetOrMapImplScenarios, FindExisting) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::findExisting.apply(state);
}

TEST(RedBlackTreeSetOrMapImplScenarios, InsertExisting) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::insertExisting.apply(state);
}

#if 0
TEST(RedBlackTreeSetOrMapImplScenarios, InsertFull) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < State::capacity; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    state.useStoredKey = false;
    Rules::insertFull.apply(state);
}
#endif

TEST(RedBlackTreeSetOrMapImplScenarios, InsertNotFull) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    const FwSizeType n = 30;
    for (FwSizeType i = 0; i < n; i++) {
        Rules::insertNotFull.apply(state);
    }
    state.tester.printTree();
    std::cout << "size is " << impl.getSize() << "\n";
    state.tester.printBlackHeight();
}

#if 0
TEST(RedBlackTreeSetOrMapImplScenarios, Remove) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::remove.apply(state);
    Rules::remove.apply(state);
}

TEST(RedBlackTreeSetOrMapImplScenarios, RemoveExisting) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::removeExisting.apply(state);
}
#endif

TEST(RedBlackTreeSetOrMapImplScenarios, Random) {
    State::Tester::Node nodes[State::capacity];
    State::Tester::Index freeNodes[State::capacity];
    State::Impl impl(nodes, freeNodes, State::capacity);
    State state(impl);
    Scenarios::random(Fw::String("RedBlackTreeSetOrMapImplRandom"), state, 1000);
}

}  // namespace RedBlackTreeSetOrMapImplTest
}  // namespace Fw
