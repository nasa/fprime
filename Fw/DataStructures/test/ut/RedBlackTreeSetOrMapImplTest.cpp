// ======================================================================
// \title  RedBlackTreeSetOrMapImplTest.cpp
// \author bocchino
// \brief  cpp file for RedBlackTreeSetOrMapImpl tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/DataStructures/RedBlackTreeSetOrMapImpl.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/test/ut/RedBlackTreeSetOrMapImplTester.hpp"
#if 0
#include "Fw/DataStructures/test/ut/STest/RedBlackTreeSetOrMapImplTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/RedBlackTreeSetOrMapImplTestScenarios.hpp"
#endif

namespace Fw {

namespace RedBlackTreeSetOrMapImplTest {

RedBlackTreeSetOrMapImpl<U16, U32> impl;

#if 0
State::Impl impl_;
State::Impl::ConstIterator it(impl_);

TEST(RedBlackTreeSetOrMapImpl, ZeroArgConstructor) {
    State::Impl impl;
    ASSERT_EQ(impl.getCapacity(), 0);
    ASSERT_EQ(impl.getSize(), 0);
}

TEST(RedBlackTreeSetOrMapImpl, TypedStorageConstructor) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State::Tester tester(impl);
    ASSERT_EQ(tester.getEntries().getElements(), entries);
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0);
}

TEST(RedBlackTreeSetOrMapImpl, UntypedStorageConstructor) {
    constexpr auto alignment = State::Impl::getByteArrayAlignment();
    constexpr auto byteArraySize = State::Impl::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    State::Impl impl(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    State::Tester tester(impl);
    ASSERT_EQ(tester.getEntries().getElements(), reinterpret_cast<State::Entry*>(bytes));
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0);
}

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

TEST(RedBlackTreeSetOrMapImplScenarios, Find) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::find.apply(state);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::find.apply(state);
}

TEST(RedBlackTreeSetOrMapImplScenarios, FindExisting) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::findExisting.apply(state);
}

TEST(RedBlackTreeSetOrMapImplScenarios, InsertExisting) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::insertExisting.apply(state);
}

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

TEST(RedBlackTreeSetOrMapImplScenarios, InsertNotFull) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
}

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

TEST(RedBlackTreeSetOrMapImplScenarios, Random) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Scenarios::random(Fw::String("RedBlackTreeSetOrMapImplRandom"), state, 1000);
}
#endif

}  // namespace RedBlackTreeSetOrMapImplTest
}  // namespace Fw
