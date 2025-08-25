// ======================================================================
// \title  ExternalArraySetTest.cpp
// \author bocchino
// \brief  cpp file for ExternalArraySet tests
// ======================================================================

#include "Fw/DataStructures/ExternalArraySet.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/ExternalArraySet.hpp"
#include "Fw/DataStructures/test/ut/ArraySetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestScenarios.hpp"

namespace Fw {

template <typename T>
class ExternalArraySetTester {
  public:
    ExternalArraySetTester<T>(const ExternalArraySet<T>& set) : m_set(set) {}

    const ArraySetOrMapImpl<T, Nil>& getImpl() const { return this->m_set.m_impl; }

  private:
    const ExternalArraySet<T>& m_set;
};

namespace SetTest {

using Entry = SetOrMapImplEntry<State::ElementType, Nil>;
using Set = ExternalArraySet<State::ElementType>;
using SetTester = ExternalArraySetTester<State::ElementType>;
using ImplTester = ArraySetOrMapImplTester<State::ElementType, Nil>;

TEST(ExternalArraySet, ZeroArgConstructor) {
    Set set;
    ASSERT_EQ(set.getCapacity(), 0);
    ASSERT_EQ(set.getSize(), 0);
}

TEST(ExternalArraySet, TypedStorageConstructor) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    SetTester setTester(set);
    ImplTester implTester(setTester.getImpl());
    ASSERT_EQ(implTester.getEntries().getElements(), entries);
    ASSERT_EQ(set.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(set.getSize(), 0);
}

TEST(ExternalArraySet, UntypedStorageConstructor) {
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

TEST(ExternalArraySet, CopyConstructor) {
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

TEST(ExternalArraySet, CopyAssignmentOperator) {
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

TEST(ExternalArraySet, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    Entry entries1[maxSize];
    Entry entries2[maxSize];
    Set s1(entries1, maxSize);
    // size1 < capacity2
    {
        Set s2(entries2, maxSize);
        State::testCopyDataFrom(s1, smallSize, s2);
    }
    // size1 == size2
    {
        Set s2(entries2, maxSize);
        State::testCopyDataFrom(s1, maxSize, s2);
    }
    // size1 > size2
    {
        Set s2(entries2, smallSize);
        State::testCopyDataFrom(s1, maxSize, s2);
    }
}

TEST(ExternalArraySetScenarios, Clear) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::clear(state);
}

TEST(ExternalArraySetScenarios, Find) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::find(state);
}

TEST(ExternalArraySetScenarios, FindExisting) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::findExisting(state);
}

TEST(ExternalArraySetScenarios, InsertExisting) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::insertExisting(state);
}

TEST(ExternalArraySetScenarios, InsertFull) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::insertFull(state);
}

TEST(ExternalArraySetScenarios, InsertNotFull) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::insertNotFull(state);
}

TEST(ExternalArraySetScenarios, Remove) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::remove(state);
}

TEST(ExternalArraySetScenarios, RemoveExisting) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::removeExisting(state);
}

TEST(ExternalArraySetScenarios, Random) {
    Entry entries[State::capacity];
    Set set(entries, State::capacity);
    State state(set);
    Scenarios::random(Fw::String("ExternalArraySetRandom"), state, 1000);
}

}  // namespace SetTest
}  // namespace Fw
