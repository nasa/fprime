// ======================================================================
// \title  FifoQueueTestState.hpp
// \author bocchino
// \brief  hpp file for FIFO queue test state
// ======================================================================

#ifndef FifoQueueTestState_HPP
#define FifoQueueTestState_HPP

#include <gtest/gtest.h>
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
    //! The QueueBase type
    using QueueBase = FifoQueueBase<ItemType>;
    //! Constructor
    State(QueueBase& a_queue) : queue(a_queue) {}
    //! The queue under test
    QueueBase& queue;
    //! The queue for modeling correct behavior
    std::deque<ItemType> modelQueue;
    //! Get a random item
    static ItemType getRandomItem() { return STest::Pick::any(); }
    //! Test copy data from
    static void testCopyDataFrom(QueueBase& q1, FwSizeType size1, QueueBase& q2) {
        q1.clear();
        for (FwSizeType i = 0; i < size1; i++) {
            const auto status = q1.enqueue(static_cast<U32>(i));
            ASSERT_EQ(status, Success::SUCCESS);
        }
        q2.copyDataFrom(q1);
        const auto capacity2 = q2.getCapacity();
        const FwSizeType size = FW_MIN(size1, capacity2);
        for (FwSizeType i = 0; i < size; i++) {
            U32 val1 = 0;
            auto status = q1.peek(val1, i);
            ASSERT_EQ(status, Success::SUCCESS);
            U32 val2 = 1;
            status = q2.peek(val2, i);
            ASSERT_EQ(status, Success::SUCCESS);
            ASSERT_EQ(val1, val2);
        }
    }
};

}  // namespace FifoQueueTest

}  // namespace Fw
#endif
