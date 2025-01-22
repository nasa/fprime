// ======================================================================
// \title Os/Queue.cpp
// \brief common function implementation for Os::Queue
// ======================================================================
#include "Os/Queue.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/Serializable.hpp"

namespace Os {

FwSizeType Queue::s_queueCount = 0;
QueueRegistry* Queue::s_queueRegistry = nullptr;

Queue::Queue() : m_name(""), m_depth(0), m_size(0), m_delegate(*QueueInterface::getDelegate(m_handle_storage)) {}

Queue::~Queue() {
    m_delegate.~QueueInterface();
}

QueueInterface::Status Queue ::create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<QueueInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(depth > 0);
    FW_ASSERT(messageSize > 0);
    // Check for previous creation call
    if (this->m_depth > 0 || this->m_size > 0) {
        return QueueInterface::Status::ALREADY_CREATED;
    }
    QueueInterface::Status status = this->m_delegate.create(name, depth, messageSize);
    if (status == QueueInterface::Status::OP_OK) {
        this->m_name = name;
        this->m_depth = depth;
        this->m_size = messageSize;
        ScopeLock lock(Queue::getStaticMutex());
        Queue::s_queueCount++;
        if (Queue::s_queueRegistry != nullptr) {
            Queue::s_queueRegistry->registerQueue(this);
        }
    }
    return status;
}

QueueInterface::Status Queue::send(const U8* buffer,
                                   FwSizeType size,
                                   PlatformIntType priority,
                                   QueueInterface::BlockingType blockType) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<QueueInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(buffer != nullptr);
    // Check if initialized
    if (this->m_depth == 0 || this->m_size == 0) {
        return QueueInterface::Status::UNINITIALIZED;
    }
    // Check size before proceeding
    else if (size > this->getMessageSize()) {
        return QueueInterface::Status::SIZE_MISMATCH;
    }
    return this->m_delegate.send(buffer, size, priority, blockType);
}

QueueInterface::Status Queue::receive(U8* destination,
                                      FwSizeType capacity,
                                      QueueInterface::BlockingType blockType,
                                      FwSizeType& actualSize,
                                      PlatformIntType& priority) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<QueueInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(destination != nullptr);
    // Check if initialized
    if (this->m_depth == 0 || this->m_size == 0) {
        return QueueInterface::Status::UNINITIALIZED;
    }
    // Check capacity before proceeding
    else if (capacity < this->getMessageSize()) {
        return QueueInterface::Status::SIZE_MISMATCH;
    }
    return this->m_delegate.receive(destination, capacity, blockType, actualSize, priority);
}

FwSizeType Queue::getMessagesAvailable() const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const QueueInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getMessagesAvailable();
}

FwSizeType Queue::getMessageHighWaterMark() const {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const QueueInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getMessageHighWaterMark();
}

QueueHandle* Queue::getHandle(){
    FW_ASSERT(&this->m_delegate == reinterpret_cast<const QueueInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

QueueInterface::Status Queue::send(const Fw::SerializeBufferBase& message,
                                   PlatformIntType priority,
                                   QueueInterface::BlockingType blockType) {
    return this->send(message.getBuffAddr(), message.getBuffLength(), priority, blockType);
}

QueueInterface::Status Queue::receive(Fw::SerializeBufferBase& destination,
                                      QueueInterface::BlockingType blockType,
                                      PlatformIntType& priority) {
    FwSizeType actualSize = 0;
    destination.resetSer();  // Reset the buffer
    QueueInterface::Status status =
        this->receive(destination.getBuffAddrSer(), destination.getBuffCapacity(), blockType, actualSize, priority);
    if (status == QueueInterface::Status::OP_OK) {
        Fw::SerializeStatus serializeStatus =
            destination.setBuffLen(static_cast<Fw::Serializable::SizeType>(actualSize));
        if (serializeStatus != Fw::SerializeStatus::FW_SERIALIZE_OK) {
            status = QueueInterface::Status::SIZE_MISMATCH;
        }
    }
    return status;
}

FwSizeType Queue::getDepth() const {
    return this->m_depth;
}

FwSizeType Queue::getMessageSize() const {
    return this->m_size;
}

const QueueString& Queue::getName() const {
    return this->m_name;
}

FwSizeType Queue::getNumQueues() {
    ScopeLock lock(Queue::getStaticMutex());
    return Queue::s_queueCount;
}

Os::Mutex& Queue::getStaticMutex() {
    static Os::Mutex s_mutex;
    return s_mutex;
}

}  // namespace Os
