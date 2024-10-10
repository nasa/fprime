// ======================================================================
// \title Os/Stub/test/ut/StubFileTests.cpp
// \brief tests using stub implementation for Os::File interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Queue.hpp"
#include "Os/Stub/test/Queue.hpp"
#include "STest/Random/Random.hpp"


void resetInjections() {
    Os::Stub::Queue::Test::StaticData::data.createStatus = Os::QueueInterface::Status::OP_OK;
    Os::Stub::Queue::Test::StaticData::data.sendStatus = Os::QueueInterface::Status::OP_OK;
    Os::Stub::Queue::Test::StaticData::data.receiveStatus = Os::QueueInterface::Status::OP_OK;
    Os::Stub::Queue::Test::StaticData::data.messages = -1;
    Os::Stub::Queue::Test::StaticData::data.highWaterMark = -1;
}

// Construction test
TEST(Interface, Construction) {
    Os::Queue queue;
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.lastCalled, Os::Stub::Queue::Test::StaticData::CONSTRUCT_FN);
    resetInjections();
}

// Destruct test
TEST(Interface, Destruction) {
    delete (new Os::Queue);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.lastCalled, Os::Stub::Queue::Test::StaticData::DESTRUCT_FN);
    resetInjections();
}

// Create test
TEST(Interface, Create) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType depth = STest::Random::lowerUpper(
        std::numeric_limits<FwSizeType>::min(),
        FW_MIN(std::numeric_limits<FwSizeType>::max(), std::numeric_limits<U32>::max())
    );
    const FwSizeType messageSize = STest::Random::lowerUpper(
        std::numeric_limits<FwSizeType>::min(),
        FW_MIN(std::numeric_limits<FwSizeType>::max(), std::numeric_limits<U32>::max())
    );
    Os::Stub::Queue::Test::StaticData::data.createStatus = Os::QueueInterface::Status::INVALID_PRIORITY;
    Os::QueueInterface::Status status = queue.create(name, depth, messageSize);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.lastCalled, Os::Stub::Queue::Test::StaticData::CREATE_FN);
    ASSERT_EQ(Os::QueueInterface::Status::INVALID_PRIORITY, status);
    ASSERT_STREQ(name.toChar(), Os::Stub::Queue::Test::StaticData::data.name.toChar());
    ASSERT_EQ(depth, Os::Stub::Queue::Test::StaticData::data.depth);
    ASSERT_EQ(messageSize, Os::Stub::Queue::Test::StaticData::data.size);
    resetInjections();
}

// Send test
TEST(Interface, SendPointer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType depth = STest::Random::lowerUpper(
        std::numeric_limits<FwSizeType>::min(),
        FW_MIN(std::numeric_limits<FwSizeType>::max(), std::numeric_limits<U32>::max())
    );
    const FwSizeType messageSize = sizeof Os::Stub::Queue::Test::InjectableStlQueueHandle::Message::data;
    const FwQueuePriorityType priority =  STest::Random::lowerUpper(
        0,
        FW_MIN(std::numeric_limits<FwQueuePriorityType>::max(), std::numeric_limits<U32>::max())
    );
    U8 buffer[messageSize];
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(name, depth, messageSize));
    ASSERT_STREQ(name.toChar(), queue.getName().toChar());
    Os::Stub::Queue::Test::StaticData::data.sendStatus = Os::QueueInterface::Status::EMPTY;
    Os::QueueInterface::Status status =
        queue.send(buffer, sizeof buffer, priority, Os::QueueInterface::BlockingType::BLOCKING);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.lastCalled, Os::Stub::Queue::Test::StaticData::SEND_FN);
    ASSERT_EQ(Os::QueueInterface::Status::EMPTY, status);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.buffer, &buffer[0]);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.size, messageSize);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.priority, priority);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.blockType, Os::QueueInterface::BlockingType::BLOCKING);
    ASSERT_EQ(depth, queue.getDepth());
    ASSERT_EQ(messageSize, queue.getMessageSize());
    resetInjections();
}

// Send test
TEST(Interface, SendBuffer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType depth = STest::Random::lowerUpper(
        std::numeric_limits<FwSizeType>::min(),
        FW_MIN(std::numeric_limits<FwSizeType>::max(), std::numeric_limits<U32>::max())
    );
    const FwSizeType messageSize = sizeof Os::Stub::Queue::Test::InjectableStlQueueHandle::Message::data;
    const FwQueuePriorityType priority =  STest::Random::lowerUpper(
        0,
        FW_MIN(std::numeric_limits<FwQueuePriorityType>::max(), std::numeric_limits<U32>::max())
    );
    U8 storage[messageSize];
    Fw::ExternalSerializeBuffer buffer(storage, sizeof storage);
    Fw::String message = "hello";
    buffer.serialize(message);

    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(name, depth, messageSize));
    Os::Stub::Queue::Test::StaticData::data.sendStatus = Os::QueueInterface::Status::UNKNOWN_ERROR;
    Os::QueueInterface::Status status = queue.send(buffer, priority, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.lastCalled, Os::Stub::Queue::Test::StaticData::SEND_FN);
    ASSERT_EQ(Os::QueueInterface::Status::UNKNOWN_ERROR, status);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.buffer, storage);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.size, 7);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.priority, priority);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.blockType, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(depth, queue.getDepth());
    ASSERT_EQ(messageSize, queue.getMessageSize());

    resetInjections();
}

// Receive test
TEST(Interface, ReceivePointer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType depth = STest::Random::lowerUpper(
        std::numeric_limits<FwSizeType>::min(),
        FW_MIN(std::numeric_limits<FwSizeType>::max(), std::numeric_limits<U32>::max())
    );
    const FwSizeType sizeOut = sizeof Os::Stub::Queue::Test::InjectableStlQueueHandle::Message::data;
    const FwQueuePriorityType priorityOut =  STest::Random::lowerUpper(
        0,
        FW_MIN(std::numeric_limits<FwQueuePriorityType>::max(), std::numeric_limits<U32>::max())
    );

    FwSizeType size = sizeof Os::Stub::Queue::Test::InjectableStlQueueHandle::Message::data;
    FwQueuePriorityType priority;
    U8 storage[size];

    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(name, depth, size));
    Os::Stub::Queue::Test::StaticData::data.receiveStatus = Os::QueueInterface::Status::FULL;
    Os::Stub::Queue::Test::StaticData::data.size = sizeOut;
    Os::Stub::Queue::Test::StaticData::data.priority = priorityOut;
    Os::QueueInterface::Status status = queue.receive(storage, sizeof storage,
                                                      Os::QueueInterface::BlockingType::NONBLOCKING,
                                                      size, priority);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.lastCalled, Os::Stub::Queue::Test::StaticData::RECEIVE_FN);
    ASSERT_EQ(Os::QueueInterface::Status::FULL, status);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.buffer, &storage[0]);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.capacity, sizeof storage);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.priority, priority);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.blockType, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(size, sizeOut);
    ASSERT_EQ(priority, priorityOut);
    ASSERT_EQ(depth, queue.getDepth());
    ASSERT_EQ(size, queue.getMessageSize());

    resetInjections();
}

// Receive test
TEST(Interface, ReceiveBuffer) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType depth = STest::Random::lowerUpper(
        std::numeric_limits<FwSizeType>::min(),
        FW_MIN(std::numeric_limits<FwSizeType>::max(), std::numeric_limits<U32>::max())
    );
    const FwSizeType sizeOut = sizeof Os::Stub::Queue::Test::InjectableStlQueueHandle::Message::data;
    const FwQueuePriorityType priorityOut =  STest::Random::lowerUpper(
        0,
        FW_MIN(std::numeric_limits<FwQueuePriorityType>::max(), std::numeric_limits<U32>::max())
    );

    FwSizeType size = sizeof Os::Stub::Queue::Test::InjectableStlQueueHandle::Message::data;
    FwQueuePriorityType priority;
    U8 storage[size];
    Fw::ExternalSerializeBuffer buffer(storage, sizeof storage);

    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(name, depth, size));
    Os::Stub::Queue::Test::StaticData::data.receiveStatus = Os::QueueInterface::Status::FULL;
    Os::Stub::Queue::Test::StaticData::data.size = sizeOut;
    Os::Stub::Queue::Test::StaticData::data.priority = priorityOut;
    Os::QueueInterface::Status status = queue.receive(buffer,
                                                      Os::QueueInterface::BlockingType::NONBLOCKING,
                                                      priority);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.lastCalled, Os::Stub::Queue::Test::StaticData::RECEIVE_FN);
    ASSERT_EQ(Os::QueueInterface::Status::FULL, status);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.buffer, &storage[0]);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.capacity, sizeof storage);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.priority, priority);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.blockType, Os::QueueInterface::BlockingType::NONBLOCKING);
    ASSERT_EQ(size, sizeOut);
    ASSERT_EQ(priority, priorityOut);
    ASSERT_EQ(depth, queue.getDepth());
    ASSERT_EQ(size, queue.getMessageSize());

    resetInjections();
}

TEST(Interface, MessageCount) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType depth = 1;
    const FwSizeType messageSize = 1;
    const FwSizeType messages = STest::Random::lowerUpper(
        std::numeric_limits<FwSizeType>::min(),
        FW_MIN(std::numeric_limits<FwSizeType>::max(), std::numeric_limits<U32>::max())
    );
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(name, depth, messageSize));
    Os::Stub::Queue::Test::StaticData::data.messages = messages;
    ASSERT_EQ(queue.getMessagesAvailable(), messages);

    resetInjections();
}

TEST(Interface, MessageHighWaterMarkCount) {
    Os::Queue queue;
    Fw::String name = "My queue";
    const FwSizeType depth = 100;
    const FwSizeType messageSize = 200;
    const FwSizeType highWater = STest::Random::lowerUpper(
        std::numeric_limits<FwSizeType>::min(),
        FW_MIN(std::numeric_limits<FwSizeType>::max(), std::numeric_limits<U32>::max())
    );
    ASSERT_EQ(Os::QueueInterface::Status::OP_OK, queue.create(name, depth, messageSize));
    Os::Stub::Queue::Test::StaticData::data.highWaterMark = highWater;
    ASSERT_EQ(queue.getMessageHighWaterMark(), highWater);

    resetInjections();
}

TEST(Interface, QueueHandle) {
    Os::Queue queue;
    ASSERT_EQ(queue.getHandle(), Os::Stub::Queue::Test::StaticData::data.handle);
    ASSERT_EQ(Os::Stub::Queue::Test::StaticData::data.lastCalled, Os::Stub::Queue::Test::StaticData::HANDLE_FN);
    resetInjections();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
