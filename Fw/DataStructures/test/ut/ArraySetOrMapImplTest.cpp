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
    constexpr FwSizeType capacity = 10;
    using Entry = SetOrMapIterator<U16, U32>;
    Entry entries[capacity];
    ArraySetOrMapImpl<U16, U32> impl(entries, capacity);
    ArraySetOrMapImplTester<U16, U32> tester(impl);
    ASSERT_EQ(tester.getEntries().getElements(), entries);
    ASSERT_EQ(impl.getCapacity(), capacity);
    ASSERT_EQ(impl.getSize(), 0);
}

TEST(ArraySetOrMapImpl, UntypedStorageConstructor) {
    constexpr FwSizeType capacity = 10;
    using Entry = SetOrMapIterator<U16, U32>;
    constexpr U8 alignment = ArraySetOrMapImpl<U16, U32>::getByteArrayAlignment();
    constexpr FwSizeType byteArraySize = ArraySetOrMapImpl<U16, U32>::getByteArraySize(capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    ArraySetOrMapImpl<U16, U32> impl(ByteArray(&bytes[0], sizeof bytes), capacity);
    ArraySetOrMapImplTester<U16, U32> tester(impl);
    ASSERT_EQ(tester.getEntries().getElements(), reinterpret_cast<Entry*>(bytes));
    ASSERT_EQ(impl.getCapacity(), capacity);
    ASSERT_EQ(impl.getSize(), 0);
}

#if 0
TEST(ArraySetOrMapImpl, CopyConstructor) {
    constexpr FwSizeType capacity = 3;
    Entry entries[capacity];
    // Call the constructor providing backing storage
    ArraySetOrMapImpl<U16, U32> q1(entries, capacity);
    // Insert an item
    U32 value = 42;
    (void)q1.insert(value);
    // Call the copy constructor
    ArraySetOrMapImpl<U16, U32> q2(q1);
    ArraySetOrMapImplTester<U16, U32> tester1(q1);
    ArraySetOrMapImplTester<U16, U32> tester2(q2);
    ASSERT_EQ(tester2.getEntries().getEntries(), entries);
    ASSERT_EQ(tester2.getEntries().getSize(), capacity);
    ASSERT_EQ(tester2.getInsertIndex().getValue(), 1);
    ASSERT_EQ(tester2.getDeimplIndex().getValue(), 0);
    ASSERT_EQ(q2.getSize(), 1);
}

TEST(ArraySetOrMapImpl, CopyAssignmentOperator) {
    constexpr FwSizeType capacity = 3;
    Entry entries[capacity];
    // Call the constructor providing backing storage
    ArraySetOrMapImpl<U16, U32> q1(entries, capacity);
    // Insert an item
    U32 value = 42;
    (void)q1.insert(value);
    // Call the default constructor
    ArraySetOrMapImpl<U16, U32> q2;
    ASSERT_EQ(q2.getSize(), 0);
    // Call the copy assignment operator
    q2 = q1;
    ASSERT_EQ(q2.getSize(), 1);
}

namespace {

void testCopyDataFrom(SetOrMapBase<U16, U32>& q1, FwSizeType size1, SetOrMapBase<U16, U32>& q2) {
    q1.clear();
    for (FwSizeType i = 0; i < size1; i++) {
        const auto status = q1.insert(static_cast<U16, U32>(i));
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

TEST(ArraySetOrMapImpl, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    Entry entries1[maxSize];
    Entry entries2[maxSize];
    ArraySetOrMapImpl<U16, U32> q1(entries1, maxSize);
    // size1 < capacity2
    {
        ArraySetOrMapImpl<U16, U32> q2(entries2, maxSize);
        testCopyDataFrom(q1, smallSize, q2);
    }
    // size1 == size2
    {
        ArraySetOrMapImpl<U16, U32> q2(entries2, maxSize);
        testCopyDataFrom(q1, maxSize, q2);
    }
    // size1 > size2
    {
        ArraySetOrMapImpl<U16, U32> q2(entries2, smallSize);
        testCopyDataFrom(q1, maxSize, q2);
    }
}
#endif

TEST(ArraySetOrMapImplRules, InsertNotFull) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Rules::insertNotFull.apply(state);
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

TEST(ArraySetOrMapImplRules, At) {
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

#if 0
TEST(ArraySetOrMapImplRules, DeimplOK) {
    Entry entries[State::capacity];
    State::ExternalQueue impl(entries, State::capacity);
    State state(impl);
    Rules::insertOK.apply(state);
    Rules::deimplOK.apply(state);
}

TEST(ArraySetOrMapImplRules, DeimplEmpty) {
    Entry entries[State::capacity];
    State::ExternalQueue impl(entries, State::capacity);
    State state(impl);
    Rules::deimplEmpty.apply(state);
}

TEST(ArraySetOrMapImplRules, Clear) {
    Entry entries[State::capacity];
    State::ExternalQueue impl(entries, State::capacity);
    State state(impl);
    Rules::insertOK.apply(state);
    ASSERT_EQ(state.impl.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.impl.getSize(), 0);
}
#endif

TEST(ArraySetOrMapImplScenarios, Random) {
    State::Entry entries[State::capacity];
    State::Impl impl(entries, State::capacity);
    State state(impl);
    Scenarios::random(Fw::String("ArraySetOrMapImplRandom"), state, 1000);
}

}  // namespace ArraySetOrMapTest
}  // namespace Fw
