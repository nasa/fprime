// ======================================================================
// \title  ArrayMapTestState.hpp
// \author bocchino
// \brief  hpp file for FIFO map test state
// ======================================================================

#ifndef ArrayMapTestState_HPP
#define ArrayMapTestState_HPP

#include <map>

#include "Fw/DataStructures/MapBase.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

namespace ArrayMapTest {

struct State {
    //! The key type
    using KeyType = U16;
    //! The value type
    using ValueType = U32;
    //! The map capacity
    static constexpr FwSizeType capacity = 1024;
    //! THe MapBase type
    using MapBase = MapBase<KeyType, ValueType>;
    //! The iterator type
    using Iterator = MapIterator<KeyType, ValueType>;
    //! Constructor
    State(MapBase& a_map) : map(a_map) {}
    //! The map under test
    MapBase& map;
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
};

}  // namespace ArrayMapTest

}  // namespace Fw

#endif
