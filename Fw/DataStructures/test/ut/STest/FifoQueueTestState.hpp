#ifndef FifoQueueTestState_HPP
#define FifoQueueTestState_HPP

#include <deque>

#include "Fw/DataStructures/FifoQueue.hpp"
#include "STest/STest/Pick/Pick.hpp"

namespace Fw {

namespace FifoQueueTest {

struct State {
  //! The queue item type
  using ItemType = U32;
  //! The queue capacity
  static constexpr FwSizeType capacity = 1024;
  //! The Queue type
  using Queue = FifoQueue<ItemType, capacity>;
  //! The ExternalQueue type
  using ExternalQueue = ExternalFifoQueue<ItemType>;
  //! THe QueueBase type
  using QueueBase = FifoQueueBase<ItemType>;
  //! Constructor
  State(QueueBase& a_queue) : queue(a_queue) {}
  //! The queue under test
  QueueBase& queue;
  //! The queue for modeling correct behavior
  std::deque<ItemType> modelQueue;
  //! Get a random item
  static ItemType getRandomItem() { return STest::Pick::any(); }
};

}

}

#endif
