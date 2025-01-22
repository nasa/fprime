//
// Created by Michael Starch on 8/27/24.
//
#include <cstring>
#include "Queue.hpp"
#include "Fw/Types/Assert.hpp"

namespace Os {
namespace Stub {
namespace Queue {
namespace Test {

StaticData StaticData::data;

InjectableStlQueueHandle::InjectableStlQueueHandle() :
      // Creates the necessary handle on the heap to keep the handle size small
      m_storage(*new std::priority_queue<Message, std::deque<Message>, Message::LessMessage>),
      m_high_water(0),
      m_max_depth(0) {}

InjectableStlQueueHandle::~InjectableStlQueueHandle() {
    // Clean-up heap
    delete &m_storage;
}

InjectableStlQueue::InjectableStlQueue() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

InjectableStlQueue::~InjectableStlQueue() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

QueueInterface::Status InjectableStlQueue::create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) {
    StaticData::data.lastCalled = StaticData::LastFn::CREATE_FN;
    // This must be the case or this test queue will not work
    if (StaticData::data.sendStatus != QueueInterface::Status::OP_OK) {
        FW_ASSERT(messageSize <= sizeof InjectableStlQueueHandle::Message::data);
    }
    StaticData::data.name = name;
    StaticData::data.depth = depth;
    StaticData::data.size = messageSize;
    this->m_handle.m_max_depth = depth;
    return StaticData::data.createStatus;
}


QueueInterface::Status InjectableStlQueue::send(const U8* buffer, FwSizeType size, FwQueuePriorityType priority, QueueInterface::BlockingType blockType) {
    StaticData::data.lastCalled = StaticData::LastFn::SEND_FN;
    StaticData::data.buffer = const_cast<U8*>(buffer);
    StaticData::data.size = size;
    StaticData::data.priority = priority;
    StaticData::data.blockType = blockType;
    if (StaticData::data.sendStatus != QueueInterface::Status::OP_OK) {
        return StaticData::data.sendStatus;
    } else if (size > sizeof InjectableStlQueueHandle::Message::data) {
        return QueueInterface::Status::SIZE_MISMATCH;
    } else if (this->m_handle.m_storage.size() >= this->m_handle.m_max_depth) {
        return QueueInterface::Status::FULL;
    }

    InjectableStlQueueHandle::Message message;
    (void) std::memcpy(message.data, buffer, size);
    message.priority = priority;
    message.size = size;
    this->m_handle.m_storage.push(message);
    this->m_handle.m_high_water = FW_MAX(this->m_handle.m_high_water, this->m_handle.m_storage.size());
    return QueueInterface::Status::OP_OK;
}

QueueInterface::Status InjectableStlQueue::receive(U8* destination,
               FwSizeType capacity,
               QueueInterface::BlockingType blockType,
               FwSizeType& actualSize,
               FwQueuePriorityType& priority) {
    StaticData::data.lastCalled = StaticData::LastFn::RECEIVE_FN;
    StaticData::data.buffer = const_cast<U8*>(destination);
    StaticData::data.capacity = capacity;
    StaticData::data.blockType = blockType;
    if (StaticData::data.receiveStatus != QueueInterface::Status::OP_OK) {
        actualSize = StaticData::data.size;
        priority = StaticData::data.priority;
        return StaticData::data.receiveStatus;
    }
    if (this->m_handle.m_storage.empty()) {
        return Status::EMPTY;
    }
    InjectableStlQueueHandle::Message message = this->m_handle.m_storage.top();
    // Fail with size miss-match when the destination cannot store message
    if (message.size > capacity) {
        return QueueInterface::Status::SIZE_MISMATCH;
    }
    std::memcpy(destination, message.data, message.size);
    priority = message.priority;
    actualSize = message.size;
    this->m_handle.m_storage.pop();
    return QueueInterface::Status::OP_OK;
}

FwSizeType InjectableStlQueue::getMessagesAvailable() const {
    StaticData::data.lastCalled = StaticData::LastFn::MESSAGES_FN;
    // Injection detected
    if (StaticData::data.messages != -1) {
        return StaticData::data.messages;
    }
    return this->m_handle.m_storage.size();
}

FwSizeType InjectableStlQueue::getMessageHighWaterMark() const {
    StaticData::data.lastCalled = StaticData::LastFn::HIGH_WATER_FN;
    // Injection detected
    if (StaticData::data.highWaterMark != -1) {
        return StaticData::data.highWaterMark;
    }
    return this->m_handle.m_high_water;
}

QueueHandle* InjectableStlQueue::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::HANDLE_FN;
    StaticData::data.handle = &this->m_handle;
    return &this->m_handle;
}


}  // namespace Test
}  // namespace Queue
}  // namespace Stub
}  // namespace Os
