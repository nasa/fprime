// ======================================================================
// \title  ExternalFifoQueueTest.cpp
// \author bocchino
// \brief  cpp file for ExternalFifoQueue tests
// ======================================================================

#include <gtest/gtest.h>

#include "Fw/DataStructures/ExternalFifoQueue.hpp"

namespace Fw {

TEST(ExternalFifoQueue, ZeroArgConstructor) {
    ExternalFifoQueue<U32> queue;
    ASSERT_EQ(queue.getCapacity(), 0);
    ASSERT_EQ(queue.getSize(), 0);
}

TEST(ExternalFifoQueue, StorageConstructor) {
    constexpr FwSizeType capacity = 10;
    U32 items[capacity];
    ExternalFifoQueue<U32> queue(items, capacity);
    ASSERT_EQ(queue.getCapacity(), capacity);
    ASSERT_EQ(queue.getSize(), 0);
}

}  // namespace Fw
