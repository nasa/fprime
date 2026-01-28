// ======================================================================
// \title Os/Generic/PriorityQueue.cpp
// \brief priority queue implementation for Os::Queue
// ======================================================================
#include "Os/Generic/PriorityQueue.hpp"
#include <cstring>
#include "Fw/LanguageHelpers.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/MemAllocator.hpp"
#include "config/MemoryAllocatorTypeEnumAc.hpp"

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
    (void)::memcpy(this->m_data + offset, data, static_cast<size_t>(size));
    this->m_sizes[index] = size;
}

void PriorityQueueHandle ::load_data(FwSizeType index, U8* destination, FwSizeType size) {
    FW_ASSERT(size <= this->m_maxSize);
    FW_ASSERT(index < this->m_depth);
    FwSizeType offset = this->m_maxSize * index;
    (void)::memcpy(destination, this->m_data + offset, static_cast<size_t>(size));
}

PriorityQueue::~PriorityQueue() {}

QueueInterface::Status PriorityQueue::create(FwEnumStoreType id,
                                             const Fw::ConstStringBase& name,
                                             FwSizeType depth,
                                             FwSizeType messageSize) {
    const FwEnumStoreType identifier = id;
    QueueInterface::Status status = Os::QueueInterface::Status::OP_OK;
    // Ensure we are created exactly once
    FW_ASSERT(this->m_handle.m_indices == nullptr);
    FW_ASSERT(this->m_handle.m_sizes == nullptr);
    FW_ASSERT(this->m_handle.m_data == nullptr);

    // Get the memory allocator configured for priority queues
    Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
        Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    // Allocate indices list
    void* allocation = nullptr;
    FwSizeType size = 0;
    FwSizeType* indices = nullptr;
    FwSizeType* sizes = nullptr;
    U8* data = nullptr;
    U8* heap_pointer = nullptr;

    // Allocate indices list and construct it when valid
    size = depth * sizeof(FwSizeType);
    allocation = allocator.allocate(identifier, size, alignof(FwSizeType));
    if (allocation == nullptr) {
        status = QueueInterface::Status::ALLOCATION_FAILED;
    } else if (size < (depth * sizeof(FwSizeType))) {
        allocator.deallocate(identifier, allocation);
        status = QueueInterface::Status::ALLOCATION_FAILED;
    } else {
        indices = Fw::arrayPlacementNew<FwSizeType>(Fw::ByteArray(static_cast<U8*>(allocation), size), depth);
    }

    // Allocate sizes list and construct it when valid
    if (status == QueueInterface::Status::OP_OK) {
        size = depth * sizeof(FwSizeType);
        allocation = allocator.allocate(identifier, size, alignof(FwSizeType));
        if (allocation == nullptr) {
            allocator.deallocate(identifier, indices);
            status = QueueInterface::Status::ALLOCATION_FAILED;
        } else if (size < (depth * sizeof(FwSizeType))) {
            allocator.deallocate(identifier, indices);
            allocator.deallocate(identifier, allocation);
            status = QueueInterface::Status::ALLOCATION_FAILED;
        } else {
            sizes = Fw::arrayPlacementNew<FwSizeType>(Fw::ByteArray(static_cast<U8*>(allocation), size), depth);
        }
    }
    // Allocate data
    if (status == QueueInterface::Status::OP_OK) {
        size = depth * messageSize;
        allocation = allocator.allocate(identifier, size, alignof(U8));
        if (allocation == nullptr) {
            allocator.deallocate(identifier, indices);
            allocator.deallocate(identifier, sizes);
            status = QueueInterface::Status::ALLOCATION_FAILED;
        } else if (size < (depth * messageSize)) {
            allocator.deallocate(identifier, indices);
            allocator.deallocate(identifier, sizes);
            allocator.deallocate(identifier, allocation);
            status = QueueInterface::Status::ALLOCATION_FAILED;
        } else {
            data = static_cast<U8*>(allocation);
        }
    }
    // Allocate data for max heap
    if (status == QueueInterface::Status::OP_OK) {
        size = Types::MaxHeap::ELEMENT_SIZE * depth;
        allocation = allocator.allocate(identifier, size, Types::MaxHeap::ALIGNMENT);
        if (allocation == nullptr) {
            allocator.deallocate(identifier, indices);
            allocator.deallocate(identifier, sizes);
            allocator.deallocate(identifier, data);
            status = QueueInterface::Status::ALLOCATION_FAILED;
        } else if (size < (Types::MaxHeap::ELEMENT_SIZE * depth)) {
            allocator.deallocate(identifier, indices);
            allocator.deallocate(identifier, sizes);
            allocator.deallocate(identifier, data);
            allocator.deallocate(identifier, allocation);
            status = QueueInterface::Status::ALLOCATION_FAILED;
        } else {
            heap_pointer = static_cast<U8*>(allocation);
            this->m_handle.m_heap.create(depth, Fw::ByteArray(static_cast<U8*>(allocation), size));
        }
    }
    // Set up structures when all allocations succeeded
    if (status == QueueInterface::Status::OP_OK) {
        // Assign initial indices and sizes
        for (FwSizeType i = 0; i < depth; i++) {
            indices[i] = i;
            sizes[i] = 0;
        }
        // Set local tracking variables
        this->m_handle.m_id = id;
        this->m_handle.m_maxSize = messageSize;
        this->m_handle.m_indices = indices;
        this->m_handle.m_data = data;
        this->m_handle.m_sizes = sizes;
        this->m_handle.m_heap_pointer = heap_pointer;
        this->m_handle.m_startIndex = 0;
        this->m_handle.m_stopIndex = 0;
        this->m_handle.m_depth = depth;
        this->m_handle.m_highMark = 0;
    }
    return status;
}

void PriorityQueue::teardown() {
    this->teardownInternal();
}

void PriorityQueue::teardownInternal() {
    if (this->m_handle.m_data != nullptr) {
        const FwEnumStoreType identifier = this->m_handle.m_id;
        Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
            Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
        allocator.deallocate(identifier, this->m_handle.m_data);
        allocator.deallocate(identifier, this->m_handle.m_indices);
        allocator.deallocate(identifier, this->m_handle.m_sizes);
        this->m_handle.m_heap.teardown();
        allocator.deallocate(identifier, this->m_handle.m_heap_pointer);

        // Set these pointers to nullptr
        this->m_handle.m_data = nullptr;
        this->m_handle.m_indices = nullptr;
        this->m_handle.m_sizes = nullptr;
    }
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
