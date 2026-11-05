// ======================================================================
// \title Os/Generic/test/ut/PriorityQueueTests.cpp
// \brief tests using generic priority implementation for Os::Queue interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Generic/PriorityQueue.hpp"
#include "STest/Random/Random.hpp"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}

TEST(PriorityQueue, ReceiveTooSmallReportsSizeMismatch) {
    Os::Generic::PriorityQueue queue;
    Fw::String name("receive-too-small");
    const U8 message[] = {1, 2, 3, 4};
    U8 destination[sizeof(message) - 1] = {};
    FwSizeType actualSize = 0;
    FwQueuePriorityType priority = 0;

    ASSERT_EQ(queue.create(0, name, 1, sizeof(message)), Os::QueueInterface::Status::OP_OK);
    ASSERT_EQ(queue.send(message, sizeof(message), 0, Os::QueueInterface::BlockingType::NONBLOCKING),
              Os::QueueInterface::Status::OP_OK);

    EXPECT_EQ(queue.receive(destination, sizeof(destination), Os::QueueInterface::BlockingType::NONBLOCKING, actualSize,
                            priority),
              Os::QueueInterface::Status::SIZE_MISMATCH);
    EXPECT_EQ(queue.getMessagesAvailable(), 1u);
    queue.teardown();
}
