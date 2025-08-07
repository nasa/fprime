// ======================================================================
// \title  ExternalRedBlackTreeSetTest.cpp
// \author bocchino
// \brief  cpp file for ExternalRedBlackTreeSet tests
// ======================================================================

#include "Fw/DataStructures/ExternalRedBlackTreeSet.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/ExternalRedBlackTreeSet.hpp"
#include "Fw/DataStructures/test/ut/RedBlackTreeSetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestScenarios.hpp"

namespace Fw {

template <typename T>
class ExternalRedBlackTreeSetTester {
  public:
    ExternalRedBlackTreeSetTester<T>(const ExternalRedBlackTreeSet<T>& set) : m_set(set) {}

    const RedBlackTreeSetOrMapImpl<T, Nil>& getImpl() const { return this->m_set.m_impl; }

  private:
    const ExternalRedBlackTreeSet<T>& m_set;
};

namespace SetTest {

using Entry = SetOrMapImplEntry<State::ElementType, Nil>;
using Set = ExternalRedBlackTreeSet<State::ElementType>;
using SetTester = ExternalRedBlackTreeSetTester<State::ElementType>;
using ImplTester = RedBlackTreeSetOrMapImplTester<State::ElementType, Nil>;

TEST(ExternalRedBlackTreeSet, ZeroArgConstructor) {
    Set set;
    ASSERT_EQ(set.getCapacity(), 0);
    ASSERT_EQ(set.getSize(), 0);
}

#if 0
TEST(ExternalRedBlackTreeSet, TypedStorageConstructor) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    SetTester setTester(set);
    ImplTester implTester(setTester.getImpl());
    ASSERT_EQ(implTester.getEntries().getElements(), entries);
    ASSERT_EQ(set.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(set.getSize(), 0);
}

TEST(ExternalRedBlackTreeSet, UntypedStorageConstructor) {
    constexpr auto alignment = Set::getByteArrayAlignment();
    constexpr auto byteArraySize = Set::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    Set set(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    SetTester setTester(set);
    ImplTester implTester(setTester.getImpl());
    ASSERT_EQ(implTester.getEntries().getElements(), reinterpret_cast<Entry*>(bytes));
    ASSERT_EQ(set.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(set.getSize(), 0);
}

TEST(ExternalRedBlackTreeSet, CopyConstructor) {
    Entry entries[State::capacity];
    // Call the constructor providing backing storage
    Set set1(entries, State::capacity);
    // Insert an item
    const State::ElementType e = 42;
    const auto status = set1.insert(e);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    Set set2(set1);
    SetTester setTester1(set1);
    ImplTester implTester1(setTester1.getImpl());
    SetTester setTester2(set2);
    ImplTester implTester2(setTester2.getImpl());
    ASSERT_EQ(implTester2.getEntries().getElements(), entries);
    ASSERT_EQ(implTester2.getEntries().getSize(), FwSizeType(State::capacity));
    ASSERT_EQ(set2.getSize(), 1);
}

TEST(ExternalRedBlackTreeSet, CopyAssignmentOperator) {
    Entry entries[State::capacity];
    // Call the constructor providing backing storage
    Set set1(entries, State::capacity);
    // Insert an item
    const State::ElementType e = 42;
    const auto status = set1.insert(e);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    Set set2;
    ASSERT_EQ(set2.getSize(), 0);
    // Call the copy assignment operator
    set2 = set1;
    ASSERT_EQ(set2.getSize(), 1);
}

TEST(ExternalRedBlackTreeSet, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    Entry entries1[maxSize];
    Entry entries2[maxSize];
    Set m1(entries1, maxSize);
    // size1 < capacity2
    {
        Set m2(entries2, maxSize);
        State::testCopyDataFrom(m1, smallSize, m2);
    }
    // size1 == size2
    {
        Set m2(entries2, maxSize);
        State::testCopyDataFrom(m1, maxSize, m2);
    }
    // size1 > size2
    {
        Set m2(entries2, smallSize);
        State::testCopyDataFrom(m1, maxSize, m2);
    }
}

TEST(ExternalRedBlackTreeSetScenarios, Clear) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::clear(state);
}

TEST(ExternalRedBlackTreeSetScenarios, Find) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::find(state);
}

TEST(ExternalRedBlackTreeSetScenarios, FindExisting) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::findExisting(state);
}

TEST(ExternalRedBlackTreeSetScenarios, InsertExisting) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::insertExisting(state);
}

TEST(ExternalRedBlackTreeSetScenarios, InsertFull) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::insertFull(state);
}

TEST(ExternalRedBlackTreeSetScenarios, InsertNotFull) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::insertNotFull(state);
}

TEST(ExternalRedBlackTreeSetScenarios, Remove) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::remove(state);
}

TEST(ExternalRedBlackTreeSetScenarios, RemoveExisting) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::removeExisting(state);
}

TEST(ExternalRedBlackTreeSetScenarios, Random) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::random(Fw::String("ExternalRedBlackTreeSetRandom"), state, 1000);
}

#endif
}  // namespace SetTest
}  // namespace Fw
