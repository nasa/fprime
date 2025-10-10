// ======================================================================
// \title  RedBlackTreeMapTest.cpp
// \author bocchino
// \brief  cpp file for RedBlackTreeMap tests
// ======================================================================

#include "Fw/DataStructures/RedBlackTreeMap.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/RedBlackTreeMap.hpp"
#include "Fw/DataStructures/test/ut/RedBlackTreeSetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestScenarios.hpp"

namespace Fw {

template <typename K, typename V, FwSizeType C>
class RedBlackTreeMapTester {
  public:
    using Nodes = typename RedBlackTreeMap<K, V, C>::Node;

    using FreeNodes = typename RedBlackTreeMap<K, V, C>::FreeNodes;

    RedBlackTreeMapTester<K, V, C>(RedBlackTreeMap<K, V, C>& map) : m_map(map) {}

    const ExternalRedBlackTreeMap<K, V>& getExtMap() const { return this->m_map.m_extMap; }

    ExternalRedBlackTreeMap<K, V>& getExtMap() { return this->m_map.m_extMap; }

    const Nodes& getNodes() const { return this->m_map.m_nodes; }

    const FreeNodes& getFreeNodes() const { return this->m_map.m_freeNodes; }

  private:
    RedBlackTreeMap<K, V, C>& m_map;
};

namespace MapTest {

using Entry = SetOrMapImplEntry<State::KeyType, State::ValueType>;
using ImplTester = RedBlackTreeSetOrMapImplTester<State::KeyType, State::ValueType>;
using Map = RedBlackTreeMap<State::KeyType, State::ValueType, State::capacity>;
using MapTester = RedBlackTreeMapTester<State::KeyType, State::ValueType, State::capacity>;

TEST(RedBlackTreeMap, ZeroArgConstructor) {
    Map map;
    ASSERT_EQ(map.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(map.getSize(), 0);
}

TEST(RedBlackTreeMap, CopyConstructor) {
    Map m1;
    // Insert an item
    const State::KeyType key = 0;
    const State::ValueType value = 42;
    const auto status = m1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    Map m2(m1);
    ASSERT_EQ(m2.getSize(), 1);
}

TEST(RedBlackTreeMap, CopyAssignmentOperator) {
    Map m1;
    // Insert an item
    const State::KeyType key = 0;
    State::ValueType value = 42;
    auto status = m1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    Map m2;
    ASSERT_EQ(m2.getSize(), 0);
    // Call the copy assignment operator
    m2 = m1;
    ASSERT_EQ(m2.getSize(), 1);
    value = 0;
    status = m2.find(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    ASSERT_EQ(value, 42);
}

TEST(RedBlackTreeMap, CopyDataFrom) {
    constexpr FwSizeType maxSize = State::capacity;
    constexpr FwSizeType smallSize = maxSize / 2;
    Map m1;
    // size1 < capacity2
    {
        Map m2;
        State::testCopyDataFrom(m1, smallSize, m2);
    }
    // size1 == capacity2
    {
        Map m2;
        State::testCopyDataFrom(m1, maxSize, m2);
    }
    // size1 > capacity2
    {
        RedBlackTreeMap<State::KeyType, State::ValueType, smallSize> m2;
        State::testCopyDataFrom(m1, maxSize, m2);
    }
}

TEST(RedBlackTreeMapScenarios, Clear) {
    Map map;
    State state(map);
    Scenarios::clear(state);
}

TEST(RedBlackTreeMapScenarios, Find) {
    Map map;
    State state(map);
    Scenarios::find(state);
}

TEST(RedBlackTreeMapScenarios, FindExisting) {
    Map map;
    State state(map);
    Scenarios::findExisting(state);
}

TEST(RedBlackTreeMapScenarios, InsertExisting) {
    Map map;
    State state(map);
    Scenarios::insertExisting(state);
}

TEST(RedBlackTreeMapScenarios, InsertFull) {
    Map map;
    State state(map);
    Scenarios::insertFull(state);
}

TEST(RedBlackTreeMapScenarios, InsertNotFull) {
    Map map;
    State state(map);
    Scenarios::insertNotFull(state);
}

TEST(RedBlackTreeMapScenarios, Remove) {
    Map map;
    State state(map);
    Scenarios::remove(state);
}

TEST(RedBlackTreeMapScenarios, RemoveExisting) {
    Map map;
    State state(map);
    Scenarios::removeExisting(state);
}

TEST(RedBlackTreeMapScenarios, Random) {
    Map map;
    State state(map);
    Scenarios::random(Fw::String("RedBlackTreeMapRandom"), state, 1000);
}

}  // namespace MapTest
}  // namespace Fw
