// ======================================================================
// \title  MapTestState.hpp
// \author bocchino
// \brief  hpp file for map test state
// ======================================================================

#ifndef MapTestState_HPP
#define MapTestState_HPP

#include <gtest/gtest.h>
#include <map>

#include "Fw/DataStructures/MapBase.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

namespace MapTest {

struct State {
    //! The key type
    using KeyType = U16;
    //! The value type
    using ValueType = U32;
    //! The map capacity
    static constexpr FwSizeType capacity = 1024;
    //! The MapBase type
    using MapBaseType = MapBase<KeyType, ValueType>;
    //! Constructor
    State(MapBaseType& a_map) : map(a_map) {}
    //! The map under test
    MapBaseType& map;
    //! The map for modeling correct behavior
    std::map<KeyType, ValueType> modelMap;
    //! Whether to use the stored key
    bool useStoredKey = false;
    //! The stored key
    KeyType storedKey = 0;
    //! Whether to use the stored value
    bool useStoredValue = false;
    //! The stored value
    ValueType storedValue = 0;
    //! Get a key
    KeyType getKey() const { return useStoredKey ? storedKey : static_cast<KeyType>(STest::Pick::any()); }
    //! Get a value
    ValueType getValue() const { return useStoredValue ? storedValue : static_cast<ValueType>(STest::Pick::any()); }
    //! Check whether the model map contains the specified key
    bool modelMapContains(KeyType key) const { return modelMap.count(key) != 0; }
    //! Test copy data from
    static void testCopyDataFrom(MapBaseType& m1, FwSizeType size1, MapBaseType& m2) {
        m1.clear();
        for (FwSizeType i = 0; i < size1; i++) {
            const auto status = m1.insert(static_cast<State::KeyType>(i), static_cast<State::ValueType>(i));
            ASSERT_EQ(status, Success::SUCCESS);
        }
        m2.copyDataFrom(m1);
        const auto capacity2 = m2.getCapacity();
        const FwSizeType size = FW_MIN(size1, capacity2);
        for (FwSizeType i = 0; i < size; i++) {
            State::KeyType key = static_cast<State::KeyType>(i);
            U32 val = 0;
            const auto status = m2.find(key, val);
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_EQ(val, static_cast<State::ValueType>(i));
        }
    }
};

}  // namespace MapTest

}  // namespace Fw

#endif
