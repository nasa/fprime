// ======================================================================
// \title  ArraySetOrMapImplTest.cpp
// \author bocchino
// \brief  cpp file for ArraySetOrMapImpl tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/test/ut/ArraySetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/ArraySetOrMapImplTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/ArraySetOrMapImplTestScenarios.hpp"

namespace Fw {

namespace ArraySetOrMapImplTest {

TEST(ArraySetOrMapImpl, ZeroArgConstructor) {
    ArraySetOrMapImpl<U16, U32> impl;
    ASSERT_EQ(impl.getCapacity(), 0);
    ASSERT_EQ(impl.getSize(), 0);
}

TEST(ArraySetOrMapImpl, TypedStorageConstructor) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State::Tester tester(impl);
    ASSERT_EQ(tester.getEntries().getElements(), entries);
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0);
}

TEST(ArraySetOrMapImpl, UntypedStorageConstructor) {
    constexpr auto alignment = State::Impl::getByteArrayAlignment();
    constexpr auto byteArraySize = State::Impl::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    State::Impl impl(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    State::Tester tester(impl);
    ASSERT_EQ(tester.getEntries().getElements(), reinterpret_cast<State::Entry*>(bytes));
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0);
}

TEST(ArraySetOrMapImpl, CopyConstructor) {
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

TEST(ArraySetOrMapImpl, CopyAssignmentOperator) {
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

TEST(ArraySetOrMapImplRules, Clear) {
  State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    ASSERT_EQ(state.impl.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.impl.getSize(), 0);
}

TEST(ArraySetOrMapImplRules, Find) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::find.apply(state);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::find.apply(state);
}

TEST(ArraySetOrMapImplRules, FindExisting) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::findExisting.apply(state);
}

TEST(ArraySetOrMapImplRules, InsertExisting) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::insertExisting.apply(state);
}

TEST(ArraySetOrMapImplRules, InsertFull) {
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

TEST(ArraySetOrMapImplRules, InsertNotFull) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
}

TEST(ArraySetOrMapImplRules, Remove) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::remove.apply(state);
    Rules::remove.apply(state);
}

TEST(ArraySetOrMapImplRules, RemoveExisting) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::removeExisting.apply(state);
}

TEST(ArraySetOrMapImplScenarios, Random) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Scenarios::random(Fw::String("ArraySetOrMapImplRandom"), state, 1000);
}

}  // namespace ArraySetOrMapTest
}  // namespace Fw
