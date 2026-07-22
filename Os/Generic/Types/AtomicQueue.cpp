// ======================================================================
// \title  AtomicQueue.cpp
// \author B. Duckett
// \brief  Lock-free MPMC circular buffer with embedded buffer storage
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Fw/Types/Assert.hpp>
#include <Os/Generic/Types/AtomicQueue.hpp>
#include <cstdio>
#include <cstring>
#include <new>

namespace Types {

U32 AtomicQueue::computeChecksum(const U8* buffer, FwSizeType size) {
    FW_ASSERT(buffer != nullptr);
    U32 sum = 0;
    for (FwSizeType i = 0; i < size; ++i) {
        sum += buffer[i];
        sum = (sum << 1) | (sum >> 31);  // Rotate left
    }
    return sum;
}

AtomicQueue::AtomicQueue()
    : m_slots(nullptr),
      m_bufferMemory(nullptr),
      m_capacity(0),
      m_bufferSize(0),
      m_mask(0),
      m_enqueuePos(0),
      m_dequeuePos(0),
      m_allocator(nullptr),
      m_allocatorId(0),
      m_notFullSem(nullptr) {}

AtomicQueue::~AtomicQueue() {
    this->teardown();
}

void AtomicQueue::create(FwSizeType numBuffers,
                         FwSizeType bufferSize,
                         Fw::MemAllocator& allocator,
                         FwEnumStoreType allocatorId) {
    FW_ASSERT(numBuffers > 0, static_cast<FwAssertArgType>(numBuffers));
    FW_ASSERT(bufferSize > 0, static_cast<FwAssertArgType>(bufferSize));

    this->m_capacity = numBuffers;
    this->m_bufferSize = bufferSize;
    this->m_allocator = &allocator;
    this->m_allocatorId = allocatorId;

    // Optimization: use bitwise AND for power-of-2, otherwise modulo
    bool isPowerOf2 = (numBuffers & (numBuffers - 1)) == 0;
    this->m_mask = isPowerOf2 ? (numBuffers - 1) : 0;

    // Allocate slot array (with overflow check)
    FW_ASSERT(numBuffers <= std::numeric_limits<FwSizeType>::max() / sizeof(Slot),
              static_cast<FwAssertArgType>(numBuffers), static_cast<FwAssertArgType>(sizeof(Slot)));
    FwSizeType slotsSize = numBuffers * sizeof(Slot);
    void* slotMem = allocator.checkedAllocate(allocatorId, slotsSize, alignof(Slot));
    FW_ASSERT(slotMem != nullptr, static_cast<FwAssertArgType>(numBuffers), static_cast<FwAssertArgType>(bufferSize));
    this->m_slots = static_cast<Slot*>(slotMem);

    // Allocate contiguous buffer memory for all slots (with overflow check)
    FW_ASSERT(numBuffers <= std::numeric_limits<FwSizeType>::max() / bufferSize,
              static_cast<FwAssertArgType>(numBuffers), static_cast<FwAssertArgType>(bufferSize));
    FwSizeType totalBufferSize = numBuffers * bufferSize;
    void* bufferMem = allocator.checkedAllocate(allocatorId, totalBufferSize, 64);
    FW_ASSERT(bufferMem != nullptr, static_cast<FwAssertArgType>(numBuffers), static_cast<FwAssertArgType>(bufferSize));
    this->m_bufferMemory = static_cast<U8*>(bufferMem);

    // Initialize all slots with placement new and assign buffer pointers
    for (FwSizeType i = 0; i < numBuffers; ++i) {
        Slot* slot = new (&this->m_slots[i]) Slot();

        // Assign buffer from contiguous memory block
        slot->buffer = this->m_bufferMemory + (i * bufferSize);
        slot->size = 0;
        slot->sequence.store(i, std::memory_order_relaxed);

        // Runtime verification that sequence atomics are lock-free
        // This is critical for ISR safety and lock-free guarantee
        FW_ASSERT(slot->sequence.is_lock_free(), static_cast<FwAssertArgType>(i),
                  static_cast<FwAssertArgType>(numBuffers));
    }

    // Create semaphore for blocking enqueue support (all platforms)
    // Allocate semaphore using provided allocator
    FwSizeType semSize = sizeof(Os::CountingSemaphore);
    void* semMem = allocator.checkedAllocate(allocatorId, semSize, alignof(Os::CountingSemaphore));
    FW_ASSERT(semMem != nullptr, static_cast<FwAssertArgType>(numBuffers));

    // Use placement new to construct semaphore with initial count = numBuffers (all slots available)
    this->m_notFullSem = new (semMem) Os::CountingSemaphore(static_cast<U32>(numBuffers));
    FW_ASSERT(this->m_notFullSem != nullptr, static_cast<FwAssertArgType>(numBuffers));

    this->m_enqueuePos.store(0, std::memory_order_relaxed);
    this->m_dequeuePos.store(0, std::memory_order_relaxed);
}

void AtomicQueue::teardown() {
    // Destroy and deallocate semaphore
    if (this->m_notFullSem != nullptr) {
        FW_ASSERT(this->m_allocator != nullptr, 0);

        // Call destructor
        this->m_notFullSem->~CountingSemaphore();

        // Deallocate memory
        this->m_allocator->deallocate(this->m_allocatorId, this->m_notFullSem);
        this->m_notFullSem = nullptr;
    }

    // Destroy slots and deallocate memory
    if (this->m_slots != nullptr) {
        FW_ASSERT(this->m_capacity > 0, static_cast<FwAssertArgType>(this->m_capacity));
        FW_ASSERT(this->m_allocator != nullptr, 0);

        // Call destructors on slots
        for (FwSizeType i = 0; i < this->m_capacity; ++i) {
            FW_ASSERT(i < this->m_capacity, static_cast<FwAssertArgType>(i),
                      static_cast<FwAssertArgType>(this->m_capacity));
            this->m_slots[i].~Slot();
        }

        // Deallocate buffer memory
        if (this->m_bufferMemory != nullptr) {
            this->m_allocator->deallocate(this->m_allocatorId, this->m_bufferMemory);
            this->m_bufferMemory = nullptr;
        }

        // Deallocate slot array
        this->m_allocator->deallocate(this->m_allocatorId, this->m_slots);
        this->m_slots = nullptr;
    }

    this->m_enqueuePos.store(0, std::memory_order_relaxed);
    this->m_dequeuePos.store(0, std::memory_order_relaxed);
    this->m_capacity = 0;
    this->m_bufferSize = 0;
    this->m_mask = 0;
    this->m_allocator = nullptr;
}

bool AtomicQueue::enqueueInternal(const U8* buffer, FwSizeType size) {
    FW_ASSERT(this->m_slots != nullptr, 0);
    FW_ASSERT(buffer != nullptr, 0);
    FW_ASSERT(size > 0, static_cast<FwAssertArgType>(size));
    FW_ASSERT(size <= this->m_bufferSize, static_cast<FwAssertArgType>(size),
              static_cast<FwAssertArgType>(this->m_bufferSize));

    for (FwSizeType retry = 0; retry < MAX_CAS_RETRIES; ++retry) {
        FW_ASSERT(retry < MAX_CAS_RETRIES, static_cast<FwAssertArgType>(retry));

        // acquire-release on slot->sequence ensures coherence, enqueuePos & dequeuePos are relaxed since strong memory
        // ordering is not required

        // Get current enqueue position
        FwSizeType pos = this->m_enqueuePos.load(std::memory_order_relaxed);

        // Check against dequeue position to prevent lapping (detect full queue)
        FwSizeType deqPos = this->m_dequeuePos.load(std::memory_order_relaxed);
        FwSignedSizeType queueDiff = static_cast<FwSignedSizeType>(pos) - static_cast<FwSignedSizeType>(deqPos);
        if (queueDiff >= static_cast<FwSignedSizeType>(this->m_capacity)) {
            return false;  // Queue is full
        }

        Slot* slot = &this->m_slots[this->getIndex(pos)];
        FwSizeType seq = slot->sequence.load(std::memory_order_acquire);

        // Check if slot is ready for write (seq == pos means available)
        FwSignedSizeType diff = static_cast<FwSignedSizeType>(seq) - static_cast<FwSignedSizeType>(pos);

        if (diff == 0) {
            // Slot available, try to claim it
            if (this->m_enqueuePos.compare_exchange_weak(pos, pos + 1, std::memory_order_release,
                                                         std::memory_order_relaxed)) {
                // Claimed the slot, copy message data
                FW_ASSERT(slot->buffer != nullptr, static_cast<FwAssertArgType>(pos));
                (void)std::memcpy(slot->buffer, buffer, size);
                slot->size = size;

                // Mark slot as ready for read
                slot->sequence.store(pos + 1, std::memory_order_release);
                return true;
            }
        } else if (diff < 0) {
            // Queue is full (wrapped around)
            return false;
        }
        // else: another producer claimed this slot, retry
    }

    return false;
}

bool AtomicQueue::enqueue(const U8* buffer, FwSizeType size) {
    bool success = this->enqueueInternal(buffer, size);

    // Decrement semaphore to track available slots (if semaphore exists)
    // This ensures blocking sends see correct availability even when
    // queue is filled via non-blocking sends.
    //
    // NOTE: tryWait() may fail due to race conditions when multiple threads
    // concurrently enqueue. This is acceptable - the semaphore is a best-effort
    // hint for blocking operations. The lock-free atomics in enqueueInternal()
    // are the authoritative source of queue state.
    if (success && this->m_notFullSem != nullptr) {
        (void)this->m_notFullSem->tryWait();
    }

    return success;
}

bool AtomicQueue::enqueueBlocking(const U8* buffer, FwSizeType size, bool blockIfFull) {
    FW_ASSERT(this->m_slots != nullptr, 0);
    FW_ASSERT(buffer != nullptr, 0);

    // If not blocking or no semaphore, just use non-blocking enqueue
    if (!blockIfFull || this->m_notFullSem == nullptr) {
        return this->enqueue(buffer, size);
    }

    // Wait-first pattern: reserve slot via semaphore, then enqueue
    // This prevents semaphore count drift under contention
    for (FwSizeType attempt = 0; attempt < MAX_CAS_RETRIES; ++attempt) {
        FW_ASSERT(attempt < MAX_CAS_RETRIES, static_cast<FwAssertArgType>(attempt));

        // Reserve a slot by waiting on semaphore (blocks until space available)
        Os::CountingSemaphoreInterface::Status status = this->m_notFullSem->wait();
        FW_ASSERT(status == Os::CountingSemaphoreInterface::Status::OP_OK, static_cast<FwAssertArgType>(status));

        // Try to enqueue (should succeed since we reserved a slot)
        // Use internal method to avoid double-decrementing semaphore
        if (this->enqueueInternal(buffer, size)) {
            return true;  // Success
        }

        // Extremely rare: slot was stolen between wait() and enqueue()
        // Return the semaphore permit and retry
        status = this->m_notFullSem->post();
        FW_ASSERT(status == Os::CountingSemaphoreInterface::Status::OP_OK, static_cast<FwAssertArgType>(status));
    }

    // Exceeded retry limit (should never happen in practice)
    return false;
}

bool AtomicQueue::dequeue(U8* buffer, FwSizeType capacity, FwSizeType& actualSize) {
    FW_ASSERT(this->m_slots != nullptr, 0);
    FW_ASSERT(buffer != nullptr, 0);
    FW_ASSERT(capacity > 0, static_cast<FwAssertArgType>(capacity));

    for (FwSizeType retry = 0; retry < MAX_CAS_RETRIES; ++retry) {
        FW_ASSERT(retry < MAX_CAS_RETRIES, static_cast<FwAssertArgType>(retry));

        // acquire-release on slot->sequence ensures coherence, enqueuePos & dequeuePos are relaxed since strong memory
        // ordering is not required

        // Get current dequeue & enqueue positions
        FwSizeType pos = this->m_dequeuePos.load(std::memory_order_relaxed);
        Slot* slot = &this->m_slots[this->getIndex(pos)];
        FwSizeType seq = slot->sequence.load(std::memory_order_acquire);

        // Check if slot is ready for read (seq == pos + 1 means data available)
        FwSignedSizeType diff = static_cast<FwSignedSizeType>(seq) - static_cast<FwSignedSizeType>(pos + 1);

        if (diff == 0) {
            // Slot has data, try to claim it
            if (this->m_dequeuePos.compare_exchange_weak(pos, pos + 1, std::memory_order_release,
                                                         std::memory_order_relaxed)) {
                // Claimed the slot, check size and copy data
                // Note: sequence.load(acquire) at 10 lines above already synchronizes with
                // enqueue's sequence.store(release), ensuring size & buffer coherency
                FW_ASSERT(slot->buffer != nullptr, static_cast<FwAssertArgType>(pos));
                FW_ASSERT(slot->size > 0, static_cast<FwAssertArgType>(slot->size));
                FW_ASSERT(slot->size <= capacity, static_cast<FwAssertArgType>(slot->size),
                          static_cast<FwAssertArgType>(capacity));

                actualSize = slot->size;
                (void)std::memcpy(buffer, slot->buffer, actualSize);

                // Mark slot as available for next cycle (pos + capacity)
                slot->sequence.store(pos + this->m_capacity, std::memory_order_release);

                // Post semaphore to wake up blocked enqueue threads (if semaphore exists)
                // ISR-SAFETY NOTE: Calling from ISR depends on platform semaphore implementation.
                // See header comments for details.
                if (this->m_notFullSem != nullptr) {
                    Os::CountingSemaphoreInterface::Status status = this->m_notFullSem->post();
                    FW_ASSERT(status == Os::CountingSemaphoreInterface::Status::OP_OK,
                              static_cast<FwAssertArgType>(status));
                }

                return true;
            }
        } else if (diff < 0) {
            // Queue is empty (no data written yet)
            return false;
        }
        // else: another consumer claimed this slot, retry
    }

    return false;
}

bool AtomicQueue::isFull() const {
    // Queue is full if enqueue is exactly capacity ahead of dequeue
    // Return false for uninitialized queue
    if (this->m_capacity == 0) {
        return false;
    }
    return this->getSize() >= this->m_capacity;
}

bool AtomicQueue::isEmpty() const {
    return this->getSize() == 0;
}

FwSizeType AtomicQueue::getSize() const {
    // Safe to call on uninitialized queue
    if (this->m_capacity == 0) {
        return 0;
    }

    FwSizeType enq = this->m_enqueuePos.load(std::memory_order_relaxed);
    FwSizeType deq = this->m_dequeuePos.load(std::memory_order_relaxed);
    FwSignedSizeType diff = static_cast<FwSignedSizeType>(enq) - static_cast<FwSignedSizeType>(deq);

    // Two independent relaxed loads provide no cross-variable consistency guarantee.
    // Restrict to [0, capacity] rather than asserting — diff can be transiently negative
    // or > capacity on concurrent access even though no real program state has that.
    if (diff < 0) {
        return 0;
    }
    if (static_cast<FwSizeType>(diff) > this->m_capacity) {
        return this->m_capacity;
    }
    return static_cast<FwSizeType>(diff);
}

FwSizeType AtomicQueue::getCapacity() const {
    // Safe to call on uninitialized queue - returns 0 if not created
    return this->m_capacity;
}

FwSizeType AtomicQueue::getBufferSize() const {
    // Safe to call on uninitialized queue - returns 0 if not created
    return this->m_bufferSize;
}

}  // namespace Types
