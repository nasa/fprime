// ======================================================================
// \title Os/Generic/PriorityQueue.cpp
// \brief priority queue implementation for Os::Queue
// ======================================================================

#include "PriorityQueue.hpp"
#include <Fw/Types/Assert.hpp>
#include <cstring>
#include <new>

namespace Os {
namespace Generic {

FwSizeType PriorityQueueHandle ::find_index() {
    FwSizeType index = this->m_indices[this->m_startIndex % this->m_depth];
    this->m_startIndex = (this->m_startIndex + 1) % this->m_depth;
    return index;
}

void PriorityQueueHandle ::return_index(FwSizeType index) {
    this->m_indices[this->m_stopIndex % this->m_depth] = index;
    this->m_stopIndex = (this->m_stopIndex + 1) % this->m_depth;
}

void PriorityQueueHandle ::store_data(FwSizeType index, const U8* data, FwSizeType size) {
    FW_ASSERT(size <= this->m_maxSize);
    FW_ASSERT(index < this->m_depth);

    FwSizeType offset = this->m_maxSize * index;
    ::memcpy(this->m_data + offset, data, size);
    this->m_sizes[index] = size;
}

void PriorityQueueHandle ::load_data(FwSizeType index, U8* destination, FwSizeType size) {
    FW_ASSERT(size <= this->m_maxSize);
    FW_ASSERT(index < this->m_depth);
    FwSizeType offset = this->m_maxSize * index;
    ::memcpy(destination, this->m_data + offset, size);
}

PriorityQueue::~PriorityQueue() {
    delete[] this->m_handle.m_data;
    delete[] this->m_handle.m_indices;
    delete[] this->m_handle.m_sizes;
}

QueueInterface::Status PriorityQueue::create(const Fw::StringBase& name, FwSizeType depth, FwSizeType messageSize) {
    // Ensure we are created exactly once
    FW_ASSERT(this->m_handle.m_indices == nullptr);
    FW_ASSERT(this->m_handle.m_sizes == nullptr);
    FW_ASSERT(this->m_handle.m_data == nullptr);

    // Allocate indices list
    FwSizeType* indices = new (std::nothrow) FwSizeType[depth];
    if (indices == nullptr) {
        return QueueInterface::Status::UNKNOWN_ERROR;
    }
    // Allocate sizes list or clean-up
    FwSizeType* sizes = new (std::nothrow) FwSizeType[depth];
    if (sizes == nullptr) {
        delete[] indices;
        return QueueInterface::Status::UNKNOWN_ERROR;
    }
    // Allocate sizes list or clean-up
    U8* data = new (std::nothrow) U8[depth * messageSize];
    if (data == nullptr) {
        delete[] indices;
        delete[] sizes;
        return QueueInterface::Status::UNKNOWN_ERROR;
    }
    // Allocate max heap or clean-up
    bool created = this->m_handle.m_heap.create(depth);
    if (not created) {
        delete[] indices;
        delete[] sizes;
        delete[] data;
        return QueueInterface::Status::UNKNOWN_ERROR;
    }
    // Assign initial indices and sizes
    for (FwSizeType i = 0; i < depth; i++) {
        indices[i] = i;
        sizes[i] = 0;
    }
    // Set local tracking variables
    this->m_handle.m_maxSize = messageSize;
    this->m_handle.m_indices = indices;
    this->m_handle.m_data = data;
    this->m_handle.m_sizes = sizes;
    this->m_handle.m_startIndex = 0;
    this->m_handle.m_stopIndex = 0;
    this->m_handle.m_depth = depth;
    this->m_handle.m_highMark = 0;

    return QueueInterface::Status::OP_OK;
}

QueueInterface::Status PriorityQueue::send(const U8* buffer,
                                           FwSizeType size,
                                           FwQueuePriorityType priority,
                                           QueueInterface::BlockingType blockType) {
    // Check for sizing problem before locking
    if (size > this->m_handle.m_maxSize) {
        return QueueInterface::Status::SIZE_MISMATCH;
    }
    // Artificial block scope for scope lock ensuring an unlock in all cases and ensuring an unlock before notify
    {
        Os::ScopeLock lock(this->m_handle.m_data_lock);
        if (this->m_handle.m_heap.isFull() and blockType == BlockingType::NONBLOCKING) {
            return QueueInterface::Status::FULL;
        }
        // Will loop and block until full is false
        while (this->m_handle.m_heap.isFull()) {
            this->m_handle.m_full.wait(this->m_handle.m_data_lock);
        }
        FwSizeType index = this->m_handle.find_index();

        // Space must exist, push must work
        FW_ASSERT(this->m_handle.m_heap.push(priority, index));
        this->m_handle.store_data(index, buffer, size);
        this->m_handle.m_sizes[index] = size;
        this->m_handle.m_highMark = FW_MAX(this->m_handle.m_highMark, this->getMessagesAvailable());
    }
    this->m_handle.m_empty.notify();
    return QueueInterface::Status::OP_OK;
}

QueueInterface::Status PriorityQueue::receive(U8* destination,
                                              FwSizeType capacity,
                                              QueueInterface::BlockingType blockType,
                                              FwSizeType& actualSize,
                                              FwQueuePriorityType& priority) {
    {
        Os::ScopeLock lock(this->m_handle.m_data_lock);
        if (this->m_handle.m_heap.isEmpty() and blockType == BlockingType::NONBLOCKING) {
            return QueueInterface::Status::EMPTY;
        }
        // Loop and lock while empty
        while (this->m_handle.m_heap.isEmpty()) {
            this->m_handle.m_empty.wait(this->m_handle.m_data_lock);
        }

        FwSizeType index;
        // Message must exist, so pop must pass and size must be valid
        FW_ASSERT(this->m_handle.m_heap.pop(priority, index));
        actualSize = this->m_handle.m_sizes[index];
        FW_ASSERT(actualSize <= capacity);
        this->m_handle.load_data(index, destination, actualSize);
        this->m_handle.return_index(index);
    }
    this->m_handle.m_full.notify();
    return QueueInterface::Status::OP_OK;
}

FwSizeType PriorityQueue::getMessagesAvailable() const {
    return this->m_handle.m_heap.getSize();
}

FwSizeType PriorityQueue::getMessageHighWaterMark() const {
    // Safe to cast away const in this context because scope lock will restore unlocked state on return
    Os::ScopeLock lock(const_cast<Mutex&>(this->m_handle.m_data_lock));
    return this->m_handle.m_highMark;
}

QueueHandle* PriorityQueue::getHandle() {
    return &this->m_handle;
}

}  // namespace Generic
}  // namespace Os
