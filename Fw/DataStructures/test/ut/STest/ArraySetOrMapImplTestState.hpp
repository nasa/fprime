// ======================================================================
// \title  ArraySetOrMapImplTestState.hpp
// \author bocchino
// \brief  hpp file for ArraySetOrMapImpl test state
// ======================================================================

#ifndef ArraySetOrMapImplTestState_HPP
#define ArraySetOrMapImplTestState_HPP

#include <map>

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
#include "Fw/DataStructures/test/ut/ArraySetOrMapImplTester.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

namespace ArraySetOrMapImplTest {

struct State {
    //! The key type
    using KeyType = U16;
    //! The value type
    using ValueType = U32;
    //! The array set or map capacity
    static constexpr FwSizeType capacity = 1024;
    //! The Impl type
    using Impl = ArraySetOrMapImpl<KeyType, ValueType>;
    //! The Tester type
    using Tester = ArraySetOrMapImplTester<KeyType, ValueType>;
    //! The entry type
    using Entry = SetOrMapImplEntry<U16, U32>;
    //! Constructor
    State(Impl& a_impl) : impl(a_impl), tester(a_impl) {}
    //! The array set or map under test
    Impl& impl;
    //! The tester
    Tester tester;
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

}  // namespace ArraySetOrMapImplTest

}  // namespace Fw

#endif
