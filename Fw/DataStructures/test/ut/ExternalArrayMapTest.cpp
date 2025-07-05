// ======================================================================
// \title  ExternalArrayMapTest.cpp
// \author bocchino
// \brief  cpp file for ExternalArrayMap tests
// ======================================================================

#include "Fw/DataStructures/ExternalArrayMap.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/ExternalArrayMap.hpp"
#include "Fw/DataStructures/test/ut/ArraySetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/ArrayMapTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/ArrayMapTestScenarios.hpp"

namespace Fw {

template <typename K, typename V>
class ExternalArrayMapTester {
  public:
    ExternalArrayMapTester<K, V>(const ExternalArrayMap<K, V>& map) : m_map(map) {}

    const ArraySetOrMapImpl<K, V>& getImpl() const { return this->m_map.m_impl; }

  private:
    const ExternalArrayMap<K, V>& m_map;
};

namespace ArrayMapTest {

using Entry = SetOrMapIterator<State::KeyType, State::ValueType>;
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

TEST(ExternalFifoQueue, CopyDataFrom) {
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
    // size1 == size2
    {
        Map m2(entries2, maxSize);
        State::testCopyDataFrom(m1, maxSize, m2);
    }
    // size1 > size2
    {
        Map m2(entries2, smallSize);
        State::testCopyDataFrom(m1, maxSize, m2);
    }
}

#if 0
TEST(ExternalArrayMapRules, At) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::insertNotFull.apply(state);
    state.storedKey = 1;
    Rules::insertNotFull.apply(state);
    Rules::at.apply(state);
}
#endif

TEST(ExternalArrayMapRules, Clear) {
  Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Rules::insertNotFull.apply(state);
    ASSERT_EQ(state.map.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.map.getSize(), 0);
}

#if 0
TEST(ExternalArrayMapRules, FindExisting) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Rules::insertNotFull.apply(state);
    Rules::findExisting.apply(state);
}
#endif

TEST(ExternalArrayMapRules, InsertFull) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < State::capacity; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    state.useStoredKey = false;
    Rules::insertFull.apply(state);
}

TEST(ExternalArrayMapRules, InsertNotFull) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Rules::insertNotFull.apply(state);
}

#if 0
TEST(ExternalArrayMapRules, Remove) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::remove.apply(state);
    Rules::remove.apply(state);
}

TEST(ExternalArrayMapRules, RemoveExisting) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Rules::insertNotFull.apply(state);
    Rules::removeExisting.apply(state);
}
#endif

TEST(ExternalArrayMapScenarios, Random) {
    Entry entries[State::capacity];
    Map map(entries, State::capacity);
    State state(map);
    Scenarios::random(Fw::String("ExternalArrayMapRandom"), state, 1000);
}

}  // namespace ArrayMapTest
}  // namespace Fw
