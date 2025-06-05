#ifndef FifoQueueTestState_HPP
#define FifoQueueTestState_HPP

#include <vector>

#include "Fw/DataStructures/FifoQueue.hpp"

namespace Fw {

template<typename T, FwSizeType C> struct FifoQueueTestState {
  //! The queue under test
  FifoQueue<T, C> queue;
  //! The queue for modeling correct behavior
  std::vector<T> modelQueue;
};

}

#endif
