// ======================================================================
// \title Os/Generic/LocklessPriorityQueue.cpp
// \brief lockless ISR-safe priority queue implementation for Os::Queue
// ======================================================================
#include "Os/Generic/LocklessPriorityQueue.hpp"
#include <atomic>
#include <cstring>
#include <limits>
#include "Fw/LanguageHelpers.hpp"
#include "Fw/Time/TimeInterval.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/ByteArray.hpp"
#include "Fw/Types/MemAllocator.hpp"
#include "Os/Task.hpp"
#include "config/MemoryAllocatorTypeEnumAc.hpp"

namespace Os {
namespace Generic {

namespace {

//! Number of retry passes used by producers when updating the high-water mark via CAS.
constexpr U32 HIGH_MARK_CAS_BOUND = 16;

//! Sleep duration used by the BLOCKING send/receive paths when an entire bounded scan made no
//! progress. The value is short enough that latency-sensitive consumers see negligible delay
//! and long enough that an idle thread relinquishes its CPU instead of busy-spinning.
constexpr U32 LOCKLESS_BLOCKING_BACKOFF_US = 100;

//! Extract the state portion of a packed state-tag word.
constexpr U32 stateOf(U32 packed) {
    return packed & LocklessSlot::STATE_MASK;
}

//! Extract the tag portion of a packed state-tag word.
constexpr U32 tagOf(U32 packed) {
    return packed >> LocklessSlot::STATE_BITS;
}

//! Pack a (state, tag) pair into a single word.
constexpr U32 packStateTag(U32 state, U32 tag) {
    return (tag << LocklessSlot::STATE_BITS) | (state & LocklessSlot::STATE_MASK);
}

//! Decide whether a candidate (priority, sequence) is preferred over the current best.
//!
//! Highest priority wins; on a tie, the smallest sequence wins. Sequence comparison uses unsigned
//! modular subtraction so that any plausible wrap of the global sequence counter is still
//! ordered correctly within the queue's active window.
bool isCandidatePreferred(FwQueuePriorityType candidatePriority,
                          U32 candidateSequence,
                          FwQueuePriorityType bestPriority,
                          U32 bestSequence) {
    bool preferred = false;
    if (candidatePriority > bestPriority) {
        preferred = true;
    } else if (candidatePriority == bestPriority) {
        // Modular subtraction: if (candidate - best) interpreted unsigned has its top bit set,
        // candidate is "older" (smaller in the wrap-aware ordering) than best.
        const U32 difference = candidateSequence - bestSequence;
        const U32 topBit = static_cast<U32>(1) << (std::numeric_limits<U32>::digits - 1);
        preferred = (difference & topBit) != 0;
    }
    return preferred;
}

}  // namespace

LocklessSlot::LocklessSlot()
    : m_stateTag(packStateTag(LOCKLESS_SLOT_FREE, 0)), m_sequence(0), m_size(0), m_priority() {}

LocklessPriorityQueueHandle::LocklessPriorityQueueHandle()
    : QueueHandle(),
      m_slots(nullptr),
      m_data(nullptr),
      m_depth(0),
      m_messageSize(0),
      m_sequence(0),
      m_count(0),
      m_highMark(0),
      m_id(0) {}

LocklessPriorityQueue::~LocklessPriorityQueue() {
    // The destructor intentionally does *not* free resources. Resource cleanup is the
    // responsibility of `teardown()`, which the owner must call explicitly before the queue
    // (or its hosting `Os::Queue`) is destroyed. Two reasons:
    //
    // 1. Static destruction order: a `LocklessPriorityQueue` may live in a global / topology
    //    component that is destroyed at process exit. `teardownInternal()` calls
    //    `Fw::MemAllocatorRegistry::getInstance()`, which is itself a function-local static
    //    of unspecified destruction order with respect to other globals. A virtual
    //    `MemAllocator::deallocate` call after the registry has been destroyed manifests as
    //    "pure virtual method called" — a fault we have observed in upstream CI's topology
    //    unit test (see PR nasa/fprime#5076).
    //
    // 2. Consistency with `Os::Generic::PriorityQueue::~PriorityQueue()`, which is also
    //    empty for the same reason.
    //
    // Owners that fail to call `teardown()` will leak the slot pool and message-data region
    // exactly as they would with the existing `Os::Generic::PriorityQueue`.
}

QueueInterface::Status LocklessPriorityQueue::create(FwEnumStoreType id,
                                                     const Fw::ConstStringBase& name,
                                                     FwSizeType depth,
                                                     FwSizeType messageSize) {
    static_cast<void>(name);

    // Ensure that the queue has not already been created. A double create would leak memory and
    // is a programming error.
    FW_ASSERT(this->m_handle.m_slots == nullptr);
    FW_ASSERT(this->m_handle.m_data == nullptr);

    // The state-tag word relies on the underlying atomic being lock-free for ISR safety. The
    // check is runtime because constexpr `is_always_lock_free` is C++17 and this code targets
    // C++14. On every supported flight target the atomic is in fact lock-free.
    std::atomic<U32> probe(0);
    FW_ASSERT(probe.is_lock_free());
    FW_ASSERT(depth > 0);
    FW_ASSERT(messageSize > 0);

    // Guard the multiplications used to compute allocation sizes against overflow.
    const FwSizeType maxSize = std::numeric_limits<FwSizeType>::max();
    FW_ASSERT(depth <= (maxSize / sizeof(LocklessSlot)));
    FW_ASSERT(depth <= (maxSize / messageSize));

    // The modular sequence comparison in isCandidatePreferred relies on the active set of
    // sequence values (at most `depth`) being far smaller than half the U32 range.
    FW_ASSERT(depth < (std::numeric_limits<U32>::max() / 2));

    Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
        Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    LocklessSlot* slots = nullptr;
    U8* data = nullptr;
    QueueInterface::Status status = QueueInterface::Status::OP_OK;

    // Allocate the slot array.
    FwSizeType slotBytesRequested = depth * sizeof(LocklessSlot);
    FwSizeType slotBytesAllocated = slotBytesRequested;
    void* slotsAllocation = allocator.allocate(id, slotBytesAllocated, alignof(LocklessSlot));
    if (slotsAllocation == nullptr) {
        status = QueueInterface::Status::ALLOCATION_FAILED;
    } else if (slotBytesAllocated < slotBytesRequested) {
        allocator.deallocate(id, slotsAllocation);
        status = QueueInterface::Status::ALLOCATION_FAILED;
    } else {
        slots = Fw::arrayPlacementNew<LocklessSlot>(
            Fw::ByteArray(static_cast<U8*>(slotsAllocation), slotBytesAllocated), depth);
    }

    // Allocate the message-data region.
    if (status == QueueInterface::Status::OP_OK) {
        FwSizeType dataBytesRequested = depth * messageSize;
        FwSizeType dataBytesAllocated = dataBytesRequested;
        void* dataAllocation = allocator.allocate(id, dataBytesAllocated, alignof(U8));
        if (dataAllocation == nullptr) {
            Fw::arrayPlacementDestruct<LocklessSlot>(slots, depth);
            allocator.deallocate(id, slots);
            status = QueueInterface::Status::ALLOCATION_FAILED;
        } else if (dataBytesAllocated < dataBytesRequested) {
            Fw::arrayPlacementDestruct<LocklessSlot>(slots, depth);
            allocator.deallocate(id, slots);
            allocator.deallocate(id, dataAllocation);
            status = QueueInterface::Status::ALLOCATION_FAILED;
        } else {
            data = static_cast<U8*>(dataAllocation);
        }
    }

    // Publish the configured handle once both allocations succeeded.
    if (status == QueueInterface::Status::OP_OK) {
        this->m_handle.m_id = id;
        this->m_handle.m_messageSize = messageSize;
        this->m_handle.m_depth = depth;
        this->m_handle.m_slots = slots;
        this->m_handle.m_data = data;
        this->m_handle.m_sequence.store(0, std::memory_order_relaxed);
        this->m_handle.m_count.store(0, std::memory_order_relaxed);
        this->m_handle.m_highMark.store(0, std::memory_order_relaxed);
    }
    return status;
}

void LocklessPriorityQueue::teardown() {
    this->teardownInternal();
}

void LocklessPriorityQueue::teardownInternal() {
    if (this->m_handle.m_slots != nullptr) {
        Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
            Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
        Fw::arrayPlacementDestruct<LocklessSlot>(this->m_handle.m_slots, this->m_handle.m_depth);
        allocator.deallocate(this->m_handle.m_id, this->m_handle.m_slots);
        if (this->m_handle.m_data != nullptr) {
            allocator.deallocate(this->m_handle.m_id, this->m_handle.m_data);
        }
        this->m_handle.m_slots = nullptr;
        this->m_handle.m_data = nullptr;
        this->m_handle.m_depth = 0;
        this->m_handle.m_messageSize = 0;
        this->m_handle.m_count.store(0, std::memory_order_relaxed);
        this->m_handle.m_highMark.store(0, std::memory_order_relaxed);
        this->m_handle.m_sequence.store(0, std::memory_order_relaxed);
    }
}

QueueInterface::Status LocklessPriorityQueue::send(const U8* buffer,
                                                   FwSizeType size,
                                                   FwQueuePriorityType priority,
                                                   QueueInterface::BlockingType blockType) {
    // Programming-error checks: queue must be created and inputs must be well-formed. These are
    // preconditions, not untrusted-input checks.
    FW_ASSERT(this->m_handle.m_slots != nullptr);
    FW_ASSERT(this->m_handle.m_data != nullptr);
    FW_ASSERT(buffer != nullptr);

    // Reject oversized messages without touching the queue.
    if (size > this->m_handle.m_messageSize) {
        return QueueInterface::Status::SIZE_MISMATCH;
    }

    const FwSizeType depth = this->m_handle.m_depth;
    const bool nonBlocking = (blockType == QueueInterface::BlockingType::NONBLOCKING);

    // Bounded for non-blocking, unbounded for blocking (explicit user contract).
    for (FwSizeType pass = 0; !nonBlocking || pass <= MAX_RETRY_PASSES; pass++) {
        for (FwSizeType i = 0; i < depth; i++) {
            LocklessSlot& slot = this->m_handle.m_slots[i];
            U32 packed = slot.m_stateTag.load(std::memory_order_acquire);
            if (stateOf(packed) != LOCKLESS_SLOT_FREE) {
                continue;
            }
            const U32 desired = packStateTag(LOCKLESS_SLOT_WRITING, tagOf(packed) + 1);
            if (slot.m_stateTag.compare_exchange_strong(packed, desired, std::memory_order_acq_rel,
                                                        std::memory_order_relaxed)) {
                if (size > 0) {
                    const FwSizeType offset = i * this->m_handle.m_messageSize;
                    static_cast<void>(::memcpy(this->m_handle.m_data + offset, buffer, static_cast<size_t>(size)));
                }
                slot.m_size = size;
                slot.m_priority.store(priority, std::memory_order_relaxed);
                slot.m_sequence.store(this->m_handle.m_sequence.fetch_add(1, std::memory_order_relaxed),
                                      std::memory_order_relaxed);

                slot.m_stateTag.store(packStateTag(LOCKLESS_SLOT_READY, tagOf(desired) + 1), std::memory_order_release);

                const U32 nextCount = this->m_handle.m_count.fetch_add(1, std::memory_order_acq_rel) + 1;
                U32 prevMark = this->m_handle.m_highMark.load(std::memory_order_relaxed);
                for (U32 markPass = 0; (markPass < HIGH_MARK_CAS_BOUND) && (nextCount > prevMark); markPass++) {
                    if (this->m_handle.m_highMark.compare_exchange_weak(prevMark, nextCount, std::memory_order_relaxed,
                                                                        std::memory_order_relaxed)) {
                        break;
                    }
                }
                return QueueInterface::Status::OP_OK;
            }
        }
        // No free slot found this pass. Blocking callers back off; non-blocking callers retry.
        if (!nonBlocking) {
            static_cast<void>(Os::Task::delay(Fw::TimeInterval(0, LOCKLESS_BLOCKING_BACKOFF_US)));
        }
    }
    return QueueInterface::Status::FULL;
}

QueueInterface::Status LocklessPriorityQueue::receive(U8* destination,
                                                      FwSizeType capacity,
                                                      QueueInterface::BlockingType blockType,
                                                      FwSizeType& actualSize,
                                                      FwQueuePriorityType& priority) {
    // Programming-error checks. These mirror the assertions in send().
    FW_ASSERT(this->m_handle.m_slots != nullptr);
    FW_ASSERT(this->m_handle.m_data != nullptr);
    FW_ASSERT(destination != nullptr);

    const FwSizeType depth = this->m_handle.m_depth;
    const bool nonBlocking = (blockType == QueueInterface::BlockingType::NONBLOCKING);

    // Bounded for non-blocking, unbounded for blocking (explicit user contract).
    for (FwSizeType pass = 0; !nonBlocking || pass <= MAX_RETRY_PASSES; pass++) {
        FwSizeType bestIndex = depth;
        FwQueuePriorityType bestPriority = FwQueuePriorityType();
        U32 bestSequence = 0;
        U32 bestPacked = 0;

        for (FwSizeType i = 0; i < depth; i++) {
            LocklessSlot& slot = this->m_handle.m_slots[i];
            U32 packed = slot.m_stateTag.load(std::memory_order_acquire);
            if (stateOf(packed) != LOCKLESS_SLOT_READY) {
                continue;
            }
            const FwQueuePriorityType candidatePriority = slot.m_priority.load(std::memory_order_relaxed);
            const U32 candidateSequence = slot.m_sequence.load(std::memory_order_relaxed);
            const U32 packedRecheck = slot.m_stateTag.load(std::memory_order_acquire);
            if (packed != packedRecheck) {
                continue;
            }
            if ((bestIndex == depth) ||
                isCandidatePreferred(candidatePriority, candidateSequence, bestPriority, bestSequence)) {
                bestIndex = i;
                bestPriority = candidatePriority;
                bestSequence = candidateSequence;
                bestPacked = packed;
            }
        }

        if (bestIndex == depth) {
            if (!nonBlocking) {
                static_cast<void>(Os::Task::delay(Fw::TimeInterval(0, LOCKLESS_BLOCKING_BACKOFF_US)));
            }
            continue;
        }

        const U32 desired = packStateTag(LOCKLESS_SLOT_READING, tagOf(bestPacked) + 1);
        if (this->m_handle.m_slots[bestIndex].m_stateTag.compare_exchange_strong(
                bestPacked, desired, std::memory_order_acq_rel, std::memory_order_relaxed)) {
            LocklessSlot& slot = this->m_handle.m_slots[bestIndex];
            const FwSizeType storedSize = slot.m_size;
            FW_ASSERT(storedSize <= capacity);
            if (storedSize > 0) {
                const FwSizeType offset = bestIndex * this->m_handle.m_messageSize;
                static_cast<void>(
                    ::memcpy(destination, this->m_handle.m_data + offset, static_cast<size_t>(storedSize)));
            }
            actualSize = storedSize;
            priority = slot.m_priority.load(std::memory_order_relaxed);
            slot.m_stateTag.store(packStateTag(LOCKLESS_SLOT_FREE, tagOf(desired) + 1), std::memory_order_release);
            static_cast<void>(this->m_handle.m_count.fetch_sub(1, std::memory_order_acq_rel));
            return QueueInterface::Status::OP_OK;
        }
        // CAS failed — another consumer claimed this slot; yield and rescan.
        if (!nonBlocking) {
            static_cast<void>(Os::Task::delay(Fw::TimeInterval(0, 0)));
        }
    }
    return QueueInterface::Status::EMPTY;
}

FwSizeType LocklessPriorityQueue::getMessagesAvailable() const {
    return static_cast<FwSizeType>(this->m_handle.m_count.load(std::memory_order_acquire));
}

FwSizeType LocklessPriorityQueue::getMessageHighWaterMark() const {
    return static_cast<FwSizeType>(this->m_handle.m_highMark.load(std::memory_order_acquire));
}

QueueHandle* LocklessPriorityQueue::getHandle() {
    return &this->m_handle;
}

}  // namespace Generic
}  // namespace Os
