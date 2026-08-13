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

//! Extract the state portion of a packed state-tag word.
constexpr LocklessStateTagType stateOf(LocklessStateTagType packed) {
    return packed & LocklessSlot::STATE_MASK;
}

//! Extract the tag portion of a packed state-tag word.
constexpr LocklessStateTagType tagOf(LocklessStateTagType packed) {
    return packed >> LocklessSlot::STATE_BITS;
}

//! Pack a (state, tag) pair into a single word.
constexpr LocklessStateTagType packStateTag(LocklessStateTagType state, LocklessStateTagType tag) {
    return (tag << LocklessSlot::STATE_BITS) | (state & LocklessSlot::STATE_MASK);
}

}  // namespace

bool LocklessPriorityQueue::isCandidatePreferred(FwQueuePriorityType candidatePriority,
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

LocklessSlot::LocklessSlot()
    : m_stateTag(packStateTag(LOCKLESS_SLOT_FREE, 0)), m_sequence(0), m_size(0), m_priority(0) {}

LocklessPriorityQueueHandle::LocklessPriorityQueueHandle()
    : QueueHandle(),
      m_slots(nullptr),
      m_slotsAllocation(nullptr),
      m_data(nullptr),
      m_depth(0),
      m_messageSize(0),
      m_sequence(0),
      m_count(0),
      m_available(0),
      m_highMark(0),
      m_id(0) {}

LocklessPriorityQueue::~LocklessPriorityQueue() {
    // Intentionally empty: cleanup is the responsibility of an explicit `teardown()` call.
    // Freeing here would call the `Fw::MemAllocatorRegistry` singleton, whose destruction
    // order relative to global queue objects is unspecified (matches
    // `Os::Generic::PriorityQueue::~PriorityQueue()`).
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
    // static_assert in the header covers platforms with a compile-time guarantee; this runtime
    // check is the authoritative gate (`is_always_lock_free` is C++17 and this code targets
    // C++14).
    std::atomic<LocklessStateTagType> probe(0);
    FW_ASSERT(probe.is_lock_free());
    std::atomic<FwQueuePriorityType> priorityProbe(0);
    FW_ASSERT(priorityProbe.is_lock_free());
    std::atomic<U32> counterProbe(0);
    FW_ASSERT(counterProbe.is_lock_free());
    FW_ASSERT(depth > 0);
    FW_ASSERT(messageSize > 0);

    // Guard the multiplications used to compute allocation sizes against overflow.
    const FwSizeType maxSize = std::numeric_limits<FwSizeType>::max();
    FW_ASSERT(depth <= (maxSize / sizeof(LocklessSlot)));
    FW_ASSERT(depth <= (maxSize / messageSize));

    // The modular comparison in isCandidatePreferred is exact only while queued equal-priority
    // messages span less than half the U32 sequence domain (SDD section 7); depth bounds message
    // count, not sequence spread.
    FW_ASSERT(depth < (std::numeric_limits<U32>::max() / 2));

    Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
        Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);

    LocklessSlot* slots = nullptr;
    void* slotsAllocation = nullptr;
    U8* data = nullptr;
    QueueInterface::Status status = QueueInterface::Status::OP_OK;

    // Allocate the slot array. Request padding for manual alignment: allocators are not required
    // to honor the alignment argument (e.g. Fw::MallocAllocator ignores it).
    FW_ASSERT(depth * sizeof(LocklessSlot) <= (maxSize - alignof(LocklessSlot)));
    FwSizeType slotBytesRequested = (depth * sizeof(LocklessSlot)) + alignof(LocklessSlot);
    FwSizeType slotBytesAllocated = slotBytesRequested;
    slotsAllocation = allocator.allocate(id, slotBytesAllocated, alignof(LocklessSlot));
    if (slotsAllocation == nullptr) {
        status = QueueInterface::Status::ALLOCATION_FAILED;
    } else if (slotBytesAllocated < slotBytesRequested) {
        allocator.deallocate(id, slotsAllocation);
        status = QueueInterface::Status::ALLOCATION_FAILED;
    } else {
        const PlatformPointerCastType base = reinterpret_cast<PlatformPointerCastType>(slotsAllocation);
        const PlatformPointerCastType aligned =
            (base + (alignof(LocklessSlot) - 1)) & ~static_cast<PlatformPointerCastType>(alignof(LocklessSlot) - 1);
        const FwSizeType offset = static_cast<FwSizeType>(aligned - base);
        slots = Fw::arrayPlacementNew<LocklessSlot>(
            Fw::ByteArray(static_cast<U8*>(slotsAllocation) + offset, slotBytesAllocated - offset), depth);
    }

    // Allocate the message-data region.
    if (status == QueueInterface::Status::OP_OK) {
        FwSizeType dataBytesRequested = depth * messageSize;
        FwSizeType dataBytesAllocated = dataBytesRequested;
        void* dataAllocation = allocator.allocate(id, dataBytesAllocated, alignof(U8));
        if (dataAllocation == nullptr) {
            Fw::arrayPlacementDestruct<LocklessSlot>(slots, depth);
            allocator.deallocate(id, slotsAllocation);
            status = QueueInterface::Status::ALLOCATION_FAILED;
        } else if (dataBytesAllocated < dataBytesRequested) {
            Fw::arrayPlacementDestruct<LocklessSlot>(slots, depth);
            allocator.deallocate(id, slotsAllocation);
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
        this->m_handle.m_slotsAllocation = slotsAllocation;
        this->m_handle.m_data = data;
        this->m_handle.m_sequence.store(0, std::memory_order_relaxed);
        this->m_handle.m_count.store(0, std::memory_order_relaxed);
        this->m_handle.m_available.store(0, std::memory_order_relaxed);
        this->m_handle.m_highMark.store(0, std::memory_order_relaxed);
    }
    return status;
}

void LocklessPriorityQueue::teardown() {
    if (this->m_handle.m_slots != nullptr) {
        Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
            Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
        Fw::arrayPlacementDestruct<LocklessSlot>(this->m_handle.m_slots, this->m_handle.m_depth);
        allocator.deallocate(this->m_handle.m_id, this->m_handle.m_slotsAllocation);
        if (this->m_handle.m_data != nullptr) {
            allocator.deallocate(this->m_handle.m_id, this->m_handle.m_data);
        }
        this->m_handle.m_slots = nullptr;
        this->m_handle.m_slotsAllocation = nullptr;
        this->m_handle.m_data = nullptr;
        this->m_handle.m_depth = 0;
        this->m_handle.m_messageSize = 0;
        this->m_handle.m_count.store(0, std::memory_order_relaxed);
        this->m_handle.m_available.store(0, std::memory_order_relaxed);
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
    const bool blocking = (blockType == QueueInterface::BlockingType::BLOCKING);

    // Bounded for non-blocking, unbounded for blocking (explicit user contract).
    for (FwSizeType pass = 0; blocking || (pass < MAX_RETRY_PASSES); pass++) {
        for (FwSizeType i = 0; i < depth; i++) {
            LocklessSlot& slot = this->m_handle.m_slots[i];
            LocklessStateTagType packed = slot.m_stateTag.load(std::memory_order_acquire);
            if (stateOf(packed) != LOCKLESS_SLOT_FREE) {
                continue;
            }
            const LocklessStateTagType desired = packStateTag(LOCKLESS_SLOT_WRITING, tagOf(packed) + 1);
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

                // Increment the occupancy count *before* publishing READY. This guarantees a
                // consumer's decrement (which can only follow a READY observation) never precedes
                // this increment, so m_count cannot transiently underflow.
                const U32 nextCount = this->m_handle.m_count.fetch_add(1, std::memory_order_acq_rel) + 1;

                slot.m_stateTag.store(packStateTag(LOCKLESS_SLOT_READY, tagOf(desired) + 1), std::memory_order_release);

                // Increment the receivable count only after READY is published, so a nonzero
                // getMessagesAvailable() implies at least one message has been made receivable.
                static_cast<void>(this->m_handle.m_available.fetch_add(1, std::memory_order_acq_rel));

                // Raise the high-water mark after publication so the message is never invisible
                // while the producer runs this loop. Each strong-CAS failure strictly raises
                // prevMark (mark only increases, capped at depth), so at most depth iterations run.
                U32 prevMark = this->m_handle.m_highMark.load(std::memory_order_relaxed);
                for (FwSizeType markPass = 0; (markPass < depth) && (nextCount > prevMark); markPass++) {
                    if (this->m_handle.m_highMark.compare_exchange_strong(
                            prevMark, nextCount, std::memory_order_relaxed, std::memory_order_relaxed)) {
                        break;
                    }
                }
                return QueueInterface::Status::OP_OK;
            }
        }
        // No free slot found this pass. Blocking callers back off; non-blocking callers retry.
        if (blocking) {
            static_cast<void>(Os::Task::delay(Fw::TimeInterval(0, LOCKLESS_QUEUE_BLOCKING_BACKOFF_US)));
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
    const bool blocking = (blockType == QueueInterface::BlockingType::BLOCKING);

    // Bounded for non-blocking, unbounded for blocking (explicit user contract).
    for (FwSizeType pass = 0; blocking || (pass < MAX_RETRY_PASSES); pass++) {
        // Fast path: skip the O(depth) scan while no message is receivable.
        if (this->m_handle.m_available.load(std::memory_order_acquire) == 0) {
            if (blocking) {
                static_cast<void>(Os::Task::delay(Fw::TimeInterval(0, LOCKLESS_QUEUE_BLOCKING_BACKOFF_US)));
            }
            continue;
        }
        FwSizeType bestIndex = depth;
        FwQueuePriorityType bestPriority = FwQueuePriorityType();
        U32 bestSequence = 0;
        LocklessStateTagType bestPacked = 0;

        for (FwSizeType i = 0; i < depth; i++) {
            LocklessSlot& slot = this->m_handle.m_slots[i];
            LocklessStateTagType packed = slot.m_stateTag.load(std::memory_order_acquire);
            if (stateOf(packed) != LOCKLESS_SLOT_READY) {
                continue;
            }
            const FwQueuePriorityType candidatePriority = slot.m_priority.load(std::memory_order_relaxed);
            const U32 candidateSequence = slot.m_sequence.load(std::memory_order_relaxed);
            // Recheck: if the state-tag changed, the relaxed priority/sequence reads above may
            // belong to a recycled slot; discard the candidate and keep scanning.
            const LocklessStateTagType packedRecheck = slot.m_stateTag.load(std::memory_order_acquire);
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
            if (blocking) {
                static_cast<void>(Os::Task::delay(Fw::TimeInterval(0, LOCKLESS_QUEUE_BLOCKING_BACKOFF_US)));
            }
            continue;
        }

        const LocklessStateTagType desired = packStateTag(LOCKLESS_SLOT_READING, tagOf(bestPacked) + 1);
        if (this->m_handle.m_slots[bestIndex].m_stateTag.compare_exchange_strong(
                bestPacked, desired, std::memory_order_acq_rel, std::memory_order_relaxed)) {
            // Decrement the receivable count at the successful READY->READING claim: this
            // message can no longer complete another receive.
            static_cast<void>(this->m_handle.m_available.fetch_sub(1, std::memory_order_acq_rel));
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
            // Decrement the occupancy count *before* releasing the slot to FREE. This keeps
            // m_count (and therefore the high-water mark) at or below the queue depth: a producer
            // can only re-claim and re-count this slot after observing FREE, which follows the
            // decrement.
            static_cast<void>(this->m_handle.m_count.fetch_sub(1, std::memory_order_acq_rel));
            slot.m_stateTag.store(packStateTag(LOCKLESS_SLOT_FREE, tagOf(desired) + 1), std::memory_order_release);
            return QueueInterface::Status::OP_OK;
        }
        // CAS failed — another consumer claimed this slot; yield and rescan.
        if (blocking) {
            static_cast<void>(Os::Task::delay(Fw::TimeInterval(0, 0)));
        }
    }
    return QueueInterface::Status::EMPTY;
}

FwSizeType LocklessPriorityQueue::getMessagesAvailable() const {
    return static_cast<FwSizeType>(this->m_handle.m_available.load(std::memory_order_acquire));
}

FwSizeType LocklessPriorityQueue::getMessageHighWaterMark() const {
    return static_cast<FwSizeType>(this->m_handle.m_highMark.load(std::memory_order_acquire));
}

QueueHandle* LocklessPriorityQueue::getHandle() {
    return &this->m_handle;
}

}  // namespace Generic
}  // namespace Os
