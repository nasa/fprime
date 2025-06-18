// ======================================================================
// \title  StackTestState.hpp
// \author bocchino
// \brief  hpp file for Stack test state
// ======================================================================

#ifndef StackTestState_HPP
#define StackTestState_HPP

#include <vector>

#include "Fw/DataStructures/Stack.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

namespace StackTest {

struct State {
  //! The stack item type
  using ItemType = U32;
  //! The stack capacity
  static constexpr FwSizeType capacity = 1024;
  //! The Stack type
  using Stack = Stack<ItemType, capacity>;
  //! The ExternalStack type
  using ExternalStack = ExternalStack<ItemType>;
  //! THe StackBase type
  using StackBase = StackBase<ItemType>;
  //! Constructor
  State(StackBase& a_stack) : stack(a_stack) {}
  //! The stack under test
  StackBase& stack;
  //! The stack for modeling correct behavior
  std::vector<ItemType> modelStack;
  //! Get a random item
  static ItemType getRandomItem() { return STest::Pick::any(); }
};

}

}

#endif
