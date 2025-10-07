// ======================================================================
// \title  RedBlackTreeSetTest.cpp
// \author bocchino
// \brief  cpp file for RedBlackTreeSet tests
// ======================================================================

#include "Fw/DataStructures/RedBlackTreeSet.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/RedBlackTreeSet.hpp"
#include "Fw/DataStructures/test/ut/RedBlackTreeSetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestScenarios.hpp"

namespace Fw {

template <typename T, FwSizeType C>
class RedBlackTreeSetTester {
  public:
    using Nodes = typename RedBlackTreeSet<T, C>::Node;

    using FreeNodes = typename RedBlackTreeSet<T, C>::FreeNodes;

    RedBlackTreeSetTester<T, C>(RedBlackTreeSet<T, C>& map) : m_map(map) {}

    const ExternalRedBlackTreeSet<T>& getExtMap() const { return this->m_map.m_extMap; }

    ExternalRedBlackTreeSet<T>& getExtMap() { return this->m_map.m_extMap; }

    const Nodes& getNodes() const { return this->m_map.m_nodes; }

    const FreeNodes& getFreeNodes() const { return this->m_map.m_freeNodes; }

  private:
    RedBlackTreeSet<T, C>& m_map;
};

namespace SetTest {

using Entry = SetOrMapImplEntry<State::ElementType, Nil>;
using ImplTester = RedBlackTreeSetOrMapImplTester<State::ElementType, Nil>;
using Set = RedBlackTreeSet<State::ElementType, State::capacity>;
using SetTester = RedBlackTreeSetTester<State::ElementType, State::capacity>;

TEST(RedBlackTreeSet, ZeroArgConstructor) {
    Set set;
    ASSERT_EQ(set.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(set.getSize(), 0);
}

TEST(RedBlackTreeSet, CopyConstructor) {
    Set s1;
    // Insert an item
    const State::ElementType e = 42;
    const auto status = s1.insert(e);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    Set s2(s1);
    ASSERT_EQ(s2.getSize(), 1);
}

TEST(RedBlackTreeSet, CopyAssignmentOperator) {
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

TEST(RedBlackTreeSet, CopyDataFrom) {
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
        RedBlackTreeSet<State::ElementType, smallSize> s2;
        State::testCopyDataFrom(s1, maxSize, s2);
    }
}

TEST(RedBlackTreeSetScenarios, Clear) {
    Set set;
    State state(set);
    Scenarios::clear(state);
}

TEST(RedBlackTreeSetScenarios, Find) {
    Set set;
    State state(set);
    Scenarios::find(state);
}

TEST(RedBlackTreeSetScenarios, FindExisting) {
    Set set;
    State state(set);
    Scenarios::findExisting(state);
}

TEST(RedBlackTreeSetScenarios, InsertExisting) {
    Set set;
    State state(set);
    Scenarios::insertExisting(state);
}

TEST(RedBlackTreeSetScenarios, InsertFull) {
    Set set;
    State state(set);
    Scenarios::insertFull(state);
}

TEST(RedBlackTreeSetScenarios, InsertNotFull) {
    Set set;
    State state(set);
    Scenarios::insertNotFull(state);
}

TEST(RedBlackTreeSetScenarios, Remove) {
    Set set;
    State state(set);
    Scenarios::remove(state);
}

TEST(RedBlackTreeSetScenarios, RemoveExisting) {
    Set set;
    State state(set);
    Scenarios::removeExisting(state);
}

TEST(RedBlackTreeSetScenarios, Random) {
    Set set;
    State state(set);
    Scenarios::random(Fw::String("RedBlackTreeSetRandom"), state, 1000);
}

}  // namespace SetTest
}  // namespace Fw
