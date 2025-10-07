// ======================================================================
// \title  ExternalRedBlackTreeMapTest.cpp
// \author bocchino
// \brief  cpp file for ExternalRedBlackTreeMap tests
// ======================================================================

#include "Fw/DataStructures/ExternalRedBlackTreeMap.hpp"
#include "Fw/DataStructures/test/ut/ExternalStackTester.hpp"
#include "Fw/DataStructures/test/ut/RedBlackTreeSetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestScenarios.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

template <typename K, typename V>
class ExternalRedBlackTreeMapTester {
  public:
    ExternalRedBlackTreeMapTester<K, V>(const ExternalRedBlackTreeMap<K, V>& map) : m_map(map) {}

    const RedBlackTreeSetOrMapImpl<K, V>& getImpl() const { return this->m_map.m_impl; }

  private:
    const ExternalRedBlackTreeMap<K, V>& m_map;
};

namespace MapTest {

using Impl = RedBlackTreeSetOrMapImpl<State::KeyType, State::ValueType>;
using ImplTester = RedBlackTreeSetOrMapImplTester<State::KeyType, State::ValueType>;
using Map = ExternalRedBlackTreeMap<State::KeyType, State::ValueType>;
using MapTester = ExternalRedBlackTreeMapTester<State::KeyType, State::ValueType>;
using StackTester = ExternalStackTester<ImplTester::Index>;

TEST(ExternalRedBlackTreeMap, ZeroArgConstructor) {
    Map map;
    ASSERT_EQ(map.getCapacity(), 0);
    ASSERT_EQ(map.getSize(), 0);
}

TEST(ExternalRedBlackTreeMap, TypedStorageConstructor) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    MapTester mapTester(map);
    ImplTester implTester(mapTester.getImpl());
    ASSERT_EQ(implTester.getNodes().getElements(), nodes);
    StackTester stackTester(implTester.getFreeNodes());
    ASSERT_EQ(stackTester.getItems().getElements(), freeNodes);
    ASSERT_EQ(map.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(map.getSize(), 0);
}

TEST(ExternalRedBlackTreeMap, UntypedStorageConstructor) {
    constexpr auto alignment = Map::getByteArrayAlignment();
    constexpr auto byteArraySize = Map::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    Map map(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    MapTester mapTester(map);
    ImplTester implTester(mapTester.getImpl());
    ASSERT_EQ(implTester.getNodes().getElements(), reinterpret_cast<Impl::Node*>(bytes));
    ASSERT_EQ(map.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(map.getSize(), 0);
}

TEST(ExternalRedBlackTreeMap, CopyConstructor) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    // Call the constructor providing backing storage
    Map map1(nodes, freeNodes, State::capacity);
    // Insert an item
    const State::KeyType key = 0;
    const State::ValueType value = 42;
    const auto status = map1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    Map map2(map1);
    MapTester mapTester1(map1);
    MapTester mapTester2(map2);
    ImplTester implTester1(mapTester1.getImpl());
    ImplTester implTester2(mapTester2.getImpl());
    ASSERT_EQ(implTester2.getNodes().getElements(), nodes);
    ASSERT_EQ(implTester2.getNodes().getSize(), FwSizeType(State::capacity));
    StackTester stackTester(implTester2.getFreeNodes());
    ASSERT_EQ(stackTester.getItems().getElements(), freeNodes);
    ASSERT_EQ(map2.getSize(), 1);
}

TEST(ExternalRedBlackTreeMap, CopyAssignmentOperator) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    // Call the constructor providing backing storage
    Map map1(nodes, freeNodes, State::capacity);
    // Insert an item
    const State::KeyType key = 0;
    const State::ValueType value = 42;
    const auto status = map1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the default constructor
    Map map2;
    ASSERT_EQ(map2.getSize(), 0);
    // Call the copy assignment operator
    map2 = map1;
    ASSERT_EQ(map2.getSize(), 1);
}

TEST(ExternalRedBlackTreeMap, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    ImplTester::Node nodes1[maxSize];
    ImplTester::Node nodes2[maxSize];
    ImplTester::Index freeNodes1[maxSize];
    ImplTester::Index freeNodes2[maxSize];
    Map m1(nodes1, freeNodes1, maxSize);
    // size1 < capacity2
    {
        Map m2(nodes2, freeNodes2, maxSize);
        State::testCopyDataFrom(m1, smallSize, m2);
    }
    // size1 == capacity2
    {
        Map m2(nodes2, freeNodes2, maxSize);
        State::testCopyDataFrom(m1, maxSize, m2);
    }
    // size1 > capacity2
    {
        Map m2(nodes2, freeNodes2, smallSize);
        State::testCopyDataFrom(m1, maxSize, m2);
    }
}

TEST(ExternalRedBlackTreeMapScenarios, Clear) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    State state(map);
    Scenarios::clear(state);
}

TEST(ExternalRedBlackTreeMapScenarios, Find) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    State state(map);
    Scenarios::find(state);
}

TEST(ExternalRedBlackTreeMapScenarios, FindExisting) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    State state(map);
    Scenarios::findExisting(state);
}

TEST(ExternalRedBlackTreeMapScenarios, InsertExisting) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    State state(map);
    Scenarios::insertExisting(state);
}

TEST(ExternalRedBlackTreeMapScenarios, InsertFull) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    State state(map);
    Scenarios::insertFull(state);
}

TEST(ExternalRedBlackTreeMapScenarios, InsertNotFull) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    State state(map);
    Scenarios::insertNotFull(state);
}

TEST(ExternalRedBlackTreeMapScenarios, Remove) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    State state(map);
    Scenarios::remove(state);
}

TEST(ExternalRedBlackTreeMapScenarios, RemoveExisting) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    State state(map);
    Scenarios::removeExisting(state);
}

TEST(ExternalRedBlackTreeMapScenarios, Random) {
    ImplTester::Node nodes[State::capacity];
    ImplTester::Index freeNodes[State::capacity];
    Map map(nodes, freeNodes, State::capacity);
    State state(map);
    Scenarios::random(Fw::String("ExternalRedBlackTreeMapRandom"), state, 1000);
}

}  // namespace MapTest
}  // namespace Fw
