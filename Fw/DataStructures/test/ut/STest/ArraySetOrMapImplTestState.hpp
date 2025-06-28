// ======================================================================
// \title  ArraySetOrMapImplTestState.hpp
// \author bocchino
// \brief  hpp file for ArraySetOrMapImpl test state
// ======================================================================

#ifndef ArraySetOrMapImplTestState_HPP
#define ArraySetOrMapImplTestState_HPP

#include <map>

#include "Fw/DataStructures/ArraySetOrMapImpl.hpp"
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
  //! The ArraySetOrMapImpl type
  using ArraySetOrMapImpl = ArraySetOrMapImpl<KeyType, ValueType>;
  //! Constructor
  State(ArraySetOrMapImpl& a_impl) : impl(a_impl) {}
  //! The array set or map under test
  ArraySetOrMapImpl& impl;
  //! The map for modeling correct behavior
  std::map<KeyType, ValueType> modelMap;
  //! Get a random key
  static KeyType getRandomKey() { return static_cast<KeyType>(STest::Pick::any()); }
  //! Get a random value
  static ValueType getRandomValue() { return static_cast<ValueType>(STest::Pick::any()); }
};

}

}

#endif
