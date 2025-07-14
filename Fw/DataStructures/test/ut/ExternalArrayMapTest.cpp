// ======================================================================
// \title  ExternalArrayMapTest.cpp
// \author bocchino
// \brief  cpp file for ExternalArrayMap tests
// ======================================================================

#include "Fw/DataStructures/ExternalArrayMap.hpp"
#include "Fw/DataStructures/test/ut/ArraySetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestScenarios.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

template <typename K, typename V>
class ExternalArrayMapTester {
  public:
    ExternalArrayMapTester<K, V>(const ExternalArrayMap<K, V>& map) : m_map(map) {}

    const ArraySetOrMapImpl<K, V>& getImpl() const { return this->m_map.m_impl; }

  private:
    const ExternalArrayMap<K, V>& m_map;
};

namespace MapTest {

using Entry = SetOrMapImplEntry<State::KeyType, State::ValueType>;
using Map = ExternalArrayMap<State::KeyType, State::ValueType>;
using MapTester = ExternalArrayMapTester<State::KeyType, State::ValueType>;
using ImplTester = ArraySetOrMapImplTester<State::KeyType, State::ValueType>;

TEST(ExternalArrayMap, ZeroArgConstructor) {
    Map map;
    ASSERT_EQ(map.getCapacity(), 0);
    ASSERT_EQ(map.getSize(), 0);
}

TEST(ExternalArrayMap, TypedStorageConstructor) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    MapTester mapTester(map);
    ImplTester implTester(mapTester.getImpl());
    ASSERT_EQ(implTester.getEntries().getElements(), entries);
    ASSERT_EQ(map.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(map.getSize(), 0);
}

TEST(ExternalArrayMap, UntypedStorageConstructor) {
    constexpr auto alignment = Map::getByteArrayAlignment();
    constexpr auto byteArraySize = Map::getByteArraySize(State::capacity);
    alignas(alignment) U8 bytes[byteArraySize];
    Map map(ByteArray(&bytes[0], sizeof bytes), State::capacity);
    MapTester mapTester(map);
    ImplTester implTester(mapTester.getImpl());
    ASSERT_EQ(implTester.getEntries().getElements(), reinterpret_cast<Entry*>(bytes));
    ASSERT_EQ(map.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(map.getSize(), 0);
}

TEST(ExternalArrayMap, CopyConstructor) {
    Entry entries[State::capacity];
    // Call the constructor providing backing storage
    Map map1(entries, State::capacity);
    // Insert an item
    const State::KeyType key = 0;
    const State::ValueType value = 42;
    const auto status = map1.insert(key, value);
    ASSERT_EQ(status, Success::SUCCESS);
    // Call the copy constructor
    Map map2(map1);
    MapTester mapTester1(map1);
    ImplTester implTester1(mapTester1.getImpl());
    MapTester mapTester2(map2);
    ImplTester implTester2(mapTester2.getImpl());
    ASSERT_EQ(implTester2.getEntries().getElements(), entries);
    ASSERT_EQ(implTester2.getEntries().getSize(), FwSizeType(State::capacity));
    ASSERT_EQ(map2.getSize(), 1);
}

TEST(ExternalArrayMap, CopyAssignmentOperator) {
    Entry entries[State::capacity];
    // Call the constructor providing backing storage
    Map map1(entries, State::capacity);
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

TEST(ExternalArrayMap, CopyDataFrom) {
    constexpr FwSizeType maxSize = 10;
    constexpr FwSizeType smallSize = maxSize / 2;
    Entry entries1[maxSize];
    Entry entries2[maxSize];
    Map m1(entries1, maxSize);
    // size1 < capacity2
    {
        Map m2(entries2, maxSize);
        State::testCopyDataFrom(m1, smallSize, m2);
    }
    // size1 == capacity2
    {
        Map m2(entries2, maxSize);
        State::testCopyDataFrom(m1, maxSize, m2);
    }
    // size1 > capacity2
    {
        Map m2(entries2, smallSize);
        State::testCopyDataFrom(m1, maxSize, m2);
    }
}

TEST(ExternalArrayMapScenarios, Clear) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::clear(state);
}

TEST(ExternalArrayMapScenarios, Find) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::find(state);
}

TEST(ExternalArrayMapScenarios, FindExisting) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::findExisting(state);
}

TEST(ExternalArrayMapScenarios, InsertExisting) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::insertExisting(state);
}

TEST(ExternalArrayMapScenarios, InsertFull) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::insertFull(state);
}

TEST(ExternalArrayMapScenarios, InsertNotFull) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::insertNotFull(state);
}

TEST(ExternalArrayMapScenarios, Remove) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::remove(state);
}

TEST(ExternalArrayMapScenarios, RemoveExisting) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::removeExisting(state);
}

TEST(ExternalArrayMapScenarios, Random) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::random(Fw::String("ExternalArrayMapRandom"), state, 1000);
}

}  // namespace MapTest
}  // namespace Fw
