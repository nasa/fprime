// ======================================================================
// \title  ArrayMapTest.cpp
// \author bocchino
// \brief  cpp file for ArrayMap tests
// ======================================================================

#include "Fw/DataStructures/ArrayMap.hpp"
#include "STest/STest/Pick/Pick.hpp"

#include "Fw/DataStructures/ArrayMap.hpp"
#include "Fw/DataStructures/test/ut/ArraySetOrMapImplTester.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestScenarios.hpp"

namespace Fw {

template <typename K, typename V, FwSizeType C>
class ArrayMapTester {
  public:
    ArrayMapTester<K, V, C>(const ArrayMap<K, V, C>& map) : m_map(map) {}

    const ExternalArrayMap<K, V> getExtMap() const { return this->m_map.extMap; }

    const typename ArrayMap<K, V, C>::Entries& getEntries() const { return this->m_map.m_entries; }

  private:
    const ArrayMap<K, V, C>& m_map;
};

namespace MapTest {

using Entry = SetOrMapIterator<State::KeyType, State::ValueType>;
using Map = ArrayMap<State::KeyType, State::ValueType, State::capacity>;
using MapTester = ArrayMapTester<State::KeyType, State::ValueType, State::capacity>;
using ImplTester = ArraySetOrMapImplTester<State::KeyType, State::ValueType>;

TEST(ArrayMap, ZeroArgConstructor) {
    Map map;
    ASSERT_EQ(map.getCapacity(), FwSizeType(State::capacity));
    ASSERT_EQ(map.getSize(), 0);
}

TEST(ArrayMap, CopyConstructor) {
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

TEST(ArrayMap, CopyAssignmentOperator) {
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

TEST(ArrayMap, CopyDataFrom) {
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
        ArrayMap<State::KeyType, State::ValueType, smallSize> m2;
        State::testCopyDataFrom(m1, maxSize, m2);
    }
}

TEST(ArrayMapRules, Clear) {
    Map map;
    State state(map);
    Rules::insertNotFull.apply(state);
    ASSERT_EQ(state.map.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.map.getSize(), 0);
}

TEST(ArrayMapRules, Find) {
    Map map;
    State state(map);
    Rules::find.apply(state);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::find.apply(state);
}

TEST(ArrayMapRules, FindExisting) {
    Map map;
    State state(map);
    Rules::insertNotFull.apply(state);
    Rules::findExisting.apply(state);
}

TEST(ArrayMapRules, InsertExisting) {
    Map map;
    State state(map);
    Rules::insertNotFull.apply(state);
    Rules::insertExisting.apply(state);
}

TEST(ArrayMapRules, InsertFull) {
    Map map;
    State state(map);
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < State::capacity; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    state.useStoredKey = false;
    Rules::insertFull.apply(state);
}

TEST(ArrayMapRules, InsertNotFull) {
    Map map;
    State state(map);
    Rules::insertNotFull.apply(state);
}

TEST(ArrayMapRules, Remove) {
    Map map;
    State state(map);
    state.useStoredKey = true;
    Rules::insertNotFull.apply(state);
    Rules::remove.apply(state);
    Rules::remove.apply(state);
}

TEST(ArrayMapRules, RemoveExisting) {
    Map map;
    State state(map);
    Rules::insertNotFull.apply(state);
    Rules::removeExisting.apply(state);
}

TEST(ArrayMapScenarios, Random) {
    Map map;
    State state(map);
    Scenarios::random(Fw::String("ArrayMapRandom"), state, 1000);
}

}  // namespace MapTest
}  // namespace Fw
