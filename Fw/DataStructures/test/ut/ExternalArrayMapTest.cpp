// ======================================================================
// \title  ExternalArrayMapTest.cpp
// \author bocchino
// \brief  cpp file for ExternalArrayMap tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/DataStructures/ExternalArrayMap.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/test/ut/STest/ArrayMapTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/ArrayMapTestScenarios.hpp"

#if 0
namespace Fw {

namespace ExternalArrayMapTest {

TEST(ExternalArrayMap, ZeroArgConstructor) {
    ExternalArrayMap<U16, U32> impl;
    ASSERT_EQ(impl.getCapacity(), 0);
    ASSERT_EQ(impl.getSize(), 0);
}

TEST(ExternalArrayMap, TypedStorageConstructor) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State::Tester tester(impl);
    ASSERT_EQ(tester.getEntries().getElements(), entries);
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0);
}

TEST(ExternalArrayMap, UntypedStorageConstructor) {
    constexpr auto alignment = State::Impl::getByteArrayAlignment();
    constexpr auto byteArraySize = State::Impl::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    State::Impl impl(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    State::Tester tester(impl);
    ASSERT_EQ(tester.getEntries().getElements(), reinterpret_cast<State::Entry*>(bytes));
    ASSERT_EQ(impl.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(impl.getSize(), 0);
}

TEST(ExternalArrayMap, CopyConstructor) {
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

TEST(ExternalArrayMap, CopyAssignmentOperator) {
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

TEST(ExternalArrayMapRules, At) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::insertNotFull.apply(state);
    state.storedKey = 1;
    Rules::insertNotFull.apply(state);
    Rules::at.apply(state);
}

TEST(ExternalArrayMapRules, Clear) {
  State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    ASSERT_EQ(state.impl.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.impl.getSize(), 0);
}

TEST(ExternalArrayMapRules, FindExisting) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::findExisting.apply(state);
}

TEST(ExternalArrayMapRules, InsertFull) {
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

TEST(ExternalArrayMapRules, InsertNotFull) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
}

TEST(ExternalArrayMapRules, Remove) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::remove.apply(state);
    Rules::remove.apply(state);
}

TEST(ExternalArrayMapRules, RemoveExisting) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
    Rules::removeExisting.apply(state);
}

TEST(ExternalArrayMapScenarios, Random) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Scenarios::random(Fw::String("ExternalArrayMapRandom"), state, 1000);
}

}  // namespace ArraySetOrMapTest
}  // namespace Fw
#endif
