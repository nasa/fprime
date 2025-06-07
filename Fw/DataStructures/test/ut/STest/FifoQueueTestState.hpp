#ifndef FifoQueueTestState_HPP
#define FifoQueueTestState_HPP

#include <vector>

#include "Fw/DataStructures/FifoQueue.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

namespace FifoQueueTest {

struct State {
  //! The queue item type
  using ItemType = U32;
  //! The queue capacity
  static constexpr FwSizeType capacity = 1024;
  //! The queue under test
  FifoQueue<ItemType, capacity> queue;
  //! The queue for modeling correct behavior
  std::vector<ItemType> modelQueue;
  //! Get a random item
  static ItemType getRandomItem() { return STest::Pick::any(); }
};

}

}

#endif
