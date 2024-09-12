//
// Created by Michael Starch on 8/27/24.
//
#include "Queue.hpp"

namespace Os {
namespace Stub {
namespace Queue {

QueueInterface::Status StubQueue::create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) {
    return QueueInterface::Status::UNKNOWN_ERROR;
}

QueueInterface::Status StubQueue::send(const U8* buffer,
                                       FwSizeType size,
                                       FwQueuePriorityType priority,
                                       QueueInterface::BlockingType blockType) {
    return QueueInterface::Status::UNINITIALIZED;
}

QueueInterface::Status StubQueue::receive(U8* destination,
                                          FwSizeType capacity,
                                          QueueInterface::BlockingType blockType,
                                          FwSizeType& actualSize,
                                          FwQueuePriorityType& priority) {
    return QueueInterface::Status::UNINITIALIZED;
}

FwSizeType StubQueue::getMessagesAvailable() const {
    return 0;
}

FwSizeType StubQueue::getMessageHighWaterMark() const {
    return 0;
}

}  // namespace Queue
}  // namespace Stub
}  // namespace Os