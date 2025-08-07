// ======================================================================
// \title  ArraySetTest.cpp
// \author bocchino
// \brief  cpp file for ArraySet tests
// ======================================================================

#include "Fw/DataStructures/ArraySet.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/ArraySet.hpp"
#include "Fw/DataStructures/test/ut/ArraySetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestScenarios.hpp"

namespace Fw {

template <typename T, FwSizeType C>
class ArraySetTester {
  public:
    ArraySetTester<T, C>(const ArraySet<T, C>& set) : m_set(set) {}

    const ExternalArraySet<T> getExtSet() const { return this->m_set.extSet; }

    const typename ArraySet<T, C>::Entries& getEntries() const { return this->m_set.m_entries; }

  private:
    const ArraySet<T, C>& m_set;
};

namespace SetTest {

using Entry = SetOrMapImplEntry<State::ElementType, Nil>;
using Set = ArraySet<State::ElementType, State::capacity>;
using SetTester = ArraySetTester<State::ElementType, State::capacity>;
using ImplTester = ArraySetOrMapImplTester<State::ElementType, Nil>;

TEST(ArraySet, ZeroArgConstructor) {
    Set set;
    ASSERT_EQ(set.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(set.getSize(), 0);
}

TEST(ArraySet, CopyConstructor) {
    Set s1;
    // Insert an item
    const State::ElementType e = 42;
    const auto status = s1.insert(e);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    Set s2(s1);
    ASSERT_EQ(s2.getSize(), 1);
}

TEST(ArraySet, CopyAssignmentOperator) {
    Set s1;
    // Insert an item
    const State::ElementType e = 42;
    auto status = s1.insert(e);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    Set s2;
    ASSERT_EQ(s2.getSize(), 0);
    // Call the copy assignment operator
    s2 = s1;
    ASSERT_EQ(s2.getSize(), 1);
    status = s2.find(e);
    ASSERT_EQ(status, Success::SUCCESS);
}

TEST(ArraySet, CopyDataFrom) {
    constexpr FwSizeType maxSize = State::capacity;
    constexpr FwSizeType smallSize = maxSize / 2;
    Set s1;
    // size1 < capacity2
    {
        Set s2;
        State::testCopyDataFrom(s1, smallSize, s2);
    }
    // size1 == capacity2
    {
        Set s2;
        State::testCopyDataFrom(s1, maxSize, s2);
    }
    // size1 > capacity2
    {
        ArraySet<State::ElementType, smallSize> s2;
        State::testCopyDataFrom(s1, maxSize, s2);
    }
}

TEST(ArraySetScenarios, Clear) {
    Set set;
    State state(set);
    Scenarios::clear(state);
}

TEST(ArraySetScenarios, Find) {
    Set set;
    State state(set);
    Scenarios::find(state);
}

TEST(ArraySetScenarios, FindExisting) {
    Set set;
    State state(set);
    Scenarios::findExisting(state);
}

TEST(ArraySetScenarios, InsertExisting) {
    Set set;
    State state(set);
    Scenarios::insertExisting(state);
}

TEST(ArraySetScenarios, InsertFull) {
    Set set;
    State state(set);
    Scenarios::insertFull(state);
}

TEST(ArraySetScenarios, InsertNotFull) {
    Set set;
    State state(set);
    Scenarios::insertNotFull(state);
}

TEST(ArraySetScenarios, Remove) {
    Set set;
    State state(set);
    Scenarios::remove(state);
}

TEST(ArraySetScenarios, RemoveExisting) {
    Set set;
    State state(set);
    Scenarios::removeExisting(state);
}

TEST(ArraySetScenarios, Random) {
    Set set;
    State state(set);
    Scenarios::random(Fw::String("ArraySetRandom"), state, 1000);
}

}  // namespace SetTest
}  // namespace Fw
