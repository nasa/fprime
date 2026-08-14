// ======================================================================
// \title Os/Generic/LocklessPriorityQueue.hpp
// \brief lockless ISR-safe priority queue implementation for Os::Queue
// ======================================================================
#ifndef OS_GENERIC_LOCKLESSPRIORITYQUEUE_HPP
#define OS_GENERIC_LOCKLESSPRIORITYQUEUE_HPP

#include <atomic>
#include <limits>
#include <type_traits>
#include "Fw/FPrimeBasicTypes.hpp"
#include "Os/Queue.hpp"
#include "config/LocklessQueueCfg.hpp"

namespace Os {
namespace Generic {

static_assert(std::is_integral<LocklessStateTagType>::value && std::is_unsigned<LocklessStateTagType>::value,
              "LocklessStateTagType must be an unsigned integral type");

//! \brief compile-time lock-free possibility for an atomic of unsigned integral width WIDTH
//!
//! C++14 lacks `std::atomic<T>::is_always_lock_free` (C++17), so this is derived from the
//! standard `ATOMIC_*_LOCK_FREE` macros (0 = never, 1 = sometimes, 2 = always lock-free),
//! selected by matching the width of the corresponding builtin type so no particular ABI
//! (e.g. `sizeof(int) == 4`) is assumed. Rejects only never-lock-free (value 0) widths at
//! compile time; on sometimes-lock-free (value 1) platforms the runtime `is_lock_free()`
//! FW_ASSERT in create() is the authoritative gate.
template <FwSizeType WIDTH>
struct LocklessAtomicLockFree {
    static constexpr bool value = ((sizeof(unsigned char) == WIDTH) && (ATOMIC_CHAR_LOCK_FREE != 0)) ||
                                  ((sizeof(unsigned short) == WIDTH) && (ATOMIC_SHORT_LOCK_FREE != 0)) ||
                                  ((sizeof(unsigned int) == WIDTH) && (ATOMIC_INT_LOCK_FREE != 0)) ||
                                  ((sizeof(unsigned long) == WIDTH) && (ATOMIC_LONG_LOCK_FREE != 0)) ||
                                  ((sizeof(unsigned long long) == WIDTH) && (ATOMIC_LLONG_LOCK_FREE != 0));
};

static_assert(LocklessAtomicLockFree<sizeof(LocklessStateTagType)>::value,
              "std::atomic<LocklessStateTagType> is never lock-free on this platform; "
              "configure a narrower type in config/LocklessQueueCfg.hpp");
static_assert(LocklessAtomicLockFree<sizeof(U32)>::value, "std::atomic<U32> is never lock-free on this platform");
static_assert(LocklessAtomicLockFree<sizeof(FwQueuePriorityType)>::value,
              "std::atomic<FwQueuePriorityType> is never lock-free on this platform");

static_assert(LOCKLESS_QUEUE_MAX_RETRY_PASSES >= 1, "LOCKLESS_QUEUE_MAX_RETRY_PASSES must be at least 1");

// A zero backoff could livelock a high-priority blocking caller against a lower-priority
// thread on a strict-priority scheduler.
static_assert(LOCKLESS_QUEUE_BLOCKING_BACKOFF_US > 0, "LOCKLESS_QUEUE_BLOCKING_BACKOFF_US must be greater than 0");

static_assert((LOCKLESS_QUEUE_SLOT_ALIGNMENT & (LOCKLESS_QUEUE_SLOT_ALIGNMENT - 1)) == 0,
              "LOCKLESS_QUEUE_SLOT_ALIGNMENT must be a power of two");

//! \brief slot lifecycle states for the lockless priority queue
//!
//! Each slot in the queue moves through a four-state state machine. Producers transition slots
//! `FREE -> WRITING -> READY`. Consumers transition slots `READY -> READING -> FREE`. State values
//! occupy the low bits of a packed atomic; the remaining bits are used as an ABA tag.
enum LocklessSlotState : LocklessStateTagType {
    LOCKLESS_SLOT_FREE = 0,     //!< slot contains no data; available to a producer
    LOCKLESS_SLOT_WRITING = 1,  //!< producer has reserved the slot and is filling it
    LOCKLESS_SLOT_READY = 2,    //!< slot contains a published message available to a consumer
    LOCKLESS_SLOT_READING = 3   //!< consumer has reserved the slot and is draining it
};

//! \brief per-slot data for the lockless priority queue
//!
//! Each slot carries an atomic state-and-tag word that is the only synchronizing element of the
//! queue. Fields `m_priority` and `m_sequence` are atomic because they are read during the
//! consumer's scan phase (while the slot is `READY`) without exclusive ownership. They use
//! `relaxed` ordering; the happens-before relationship is established through the release/acquire
//! on `m_stateTag`. The non-atomic `m_size` field is only accessed under exclusive ownership
//! (`WRITING` by the producer, `READING` by the consumer).
struct alignas(LOCKLESS_QUEUE_SLOT_ALIGNMENT) LocklessSlot {
    //! Number of low bits used for the state value within `m_stateTag`.
    static constexpr U32 STATE_BITS = 2;
    //! Mask for the state portion of `m_stateTag`.
    static constexpr LocklessStateTagType STATE_MASK =
        (static_cast<LocklessStateTagType>(1) << STATE_BITS) - static_cast<LocklessStateTagType>(1);
    //! Number of bits available for the ABA epoch tag (62 with the default U64 word).
    static constexpr U32 TAG_BITS = static_cast<U32>(std::numeric_limits<LocklessStateTagType>::digits) - STATE_BITS;

    //! Packed (tag << STATE_BITS) | state word. Updated only via atomic operations. The TAG_BITS
    //! tag increments on every transition; a stale CAS is defeated unless a thread stalls
    //! between its scan and CAS across an exact multiple of 2^TAG_BITS transitions of one slot.
    //! In that case the consumer dequeues a valid message out of priority order — never
    //! corrupted, lost, or duplicated (see SDD sections 4 and 15).
    std::atomic<LocklessStateTagType> m_stateTag;
    //! Sequence number assigned at publication time. Used as a FIFO tiebreaker when priorities
    //! are equal. Atomic because consumers read it during the scan phase without ownership.
    std::atomic<U32> m_sequence;
    //! Stored message size, less than or equal to the queue's configured message size.
    //! Only accessed under exclusive ownership (WRITING or READING).
    FwSizeType m_size;
    //! Stored message priority. Atomic because consumers read it during the scan phase.
    std::atomic<FwQueuePriorityType> m_priority;

    //! Construct a slot in the FREE state with a zero tag.
    LocklessSlot();
};

//! \brief handle for the lockless priority queue
//!
//! All persistent state for the queue is contained in this handle. Memory pointed to by `m_slots`
//! and `m_data` is allocated exactly once during `LocklessPriorityQueue::create` and freed during
//! `LocklessPriorityQueue::teardown`.
struct LocklessPriorityQueueHandle : public QueueHandle {
    //! Pre-allocated array of `m_depth` slots, aligned within `m_slotsAllocation`.
    LocklessSlot* m_slots;
    //! Raw allocation backing `m_slots`; retained because allocators may ignore alignment.
    void* m_slotsAllocation;
    //! Pre-allocated array of `m_depth * m_messageSize` bytes for message payloads.
    U8* m_data;
    //! Configured queue depth in messages.
    FwSizeType m_depth;
    //! Configured maximum size of a single message.
    FwSizeType m_messageSize;
    //! Sequence assigned to messages on publication for FIFO tiebreak; may wrap (compared modularly).
    std::atomic<U32> m_sequence;
    //! Occupancy count (claimed-or-queued slots) used only for the high-water mark.
    std::atomic<U32> m_count;
    //! Receivable message count: incremented after a slot is published READY, decremented at
    //! the successful READY->READING claim. Backs getMessagesAvailable().
    std::atomic<U32> m_available;
    //! Maximum value `m_count` has ever held. Updated by producers via a bounded CAS loop.
    std::atomic<U32> m_highMark;
    //! Identifier passed to the memory allocator at create() time and reused at teardown().
    FwEnumStoreType m_id;

    //! Default-construct a handle in the uncreated state.
    LocklessPriorityQueueHandle();
};

//! \brief lockless ISR-safe priority queue implementation for Os::QueueInterface
//!
//! The lockless priority queue stores messages in a fixed pool of pre-allocated slots. Each slot
//! is governed by an atomic state machine that uses an embedded ABA tag, allowing producers and
//! consumers to manipulate the queue concurrently without taking any operating-system lock.
//!
//! \section flight_software_properties Flight-software properties
//!
//! - Memory: All memory is allocated through the configured `Fw::MemAllocator` exactly once
//!   during `create`. No allocation occurs during `send`, `receive`, `getMessagesAvailable`, or
//!   `getMessageHighWaterMark`.
//! - Loops: All non-blocking control paths are bounded by the configured queue depth multiplied
//!   by `MAX_RETRY_PASSES`. The high-water-mark CAS loop is bounded by depth because the mark
//!   only increases and never exceeds depth. Blocking paths poll with a fixed `Os::Task::delay`
//!   backoff until the requested condition is satisfied; unlike the condition-variable-based
//!   `Os::Generic::PriorityQueue`, an idle blocking caller wakes periodically rather than
//!   sleeping until signaled.
//! - Determinism: All operations execute in time bounded by queue depth; no per-message dynamic
//!   work scales with the number of producers or consumers.
//!
//! \section isr_safety ISR safety
//!
//! Non-blocking operations (`send` and `receive` with `BlockingType::NONBLOCKING`) are safe to
//! invoke from interrupt context because they use only lock-free atomic operations and bounded
//! `memcpy`. Blocking calls spin on the same atomic state and therefore must not be invoked from
//! ISR context.
//!
//! \section ordering Priority ordering
//!
//! Consumers pop the slot with the highest priority. When multiple slots share the same priority,
//! the one with the smallest sequence number (i.e. the earliest publication) is selected.
//! Sequence numbers are assigned by an atomic counter at publication time. The U32 counter may
//! wrap during a long mission; comparison uses unsigned modular subtraction so that wrap is
//! still ordered correctly within the queue's active window (create() enforces `depth < 2^31`).
//! FIFO tie-breaking assumes equal-priority messages do not remain queued across 2^31
//! intervening sends; see the SDD (`docs/sdd-lockless-queue.md` section 7) for details.
class LocklessPriorityQueue final : public Os::QueueInterface {
  public:
    //! Maximum number of retry passes through the slot array before a non-blocking operation
    //! gives up (configurable in config/LocklessQueueCfg.hpp). Each pass scans up to the
    //! configured queue depth; the worst-case work per non-blocking call is therefore
    //! `depth * MAX_RETRY_PASSES`.
    static constexpr FwSizeType MAX_RETRY_PASSES = LOCKLESS_QUEUE_MAX_RETRY_PASSES;

    //! \brief decide whether a candidate (priority, sequence) is preferred over the current best
    //!
    //! Highest priority wins; on a tie, the smallest sequence in the wrap-aware modular ordering
    //! wins. Exposed as a static member so unit tests exercise the shipped comparison.
    //!
    //! \param candidatePriority: priority of the candidate message
    //! \param candidateSequence: sequence of the candidate message
    //! \param bestPriority: priority of the current best message
    //! \param bestSequence: sequence of the current best message
    //! \return true if the candidate should be preferred over the current best
    static bool isCandidatePreferred(FwQueuePriorityType candidatePriority,
                                     U32 candidateSequence,
                                     FwQueuePriorityType bestPriority,
                                     U32 bestSequence);

    //! \brief default constructor
    LocklessPriorityQueue() = default;

    //! \brief destructor
    //!
    //! The destructor does **not** free queue resources. Owners must call `teardown()`
    //! explicitly before destroying the queue (or its hosting `Os::Queue`). This matches the
    //! `Os::Generic::PriorityQueue` contract and avoids a static-destruction-order fault
    //! where the underlying `Fw::MemAllocatorRegistry` may already have been destroyed by the
    //! time the destructor runs, which would manifest as a `pure virtual method called` abort
    //! when `MemAllocator::deallocate` is invoked through its v-table.
    ~LocklessPriorityQueue() override;

    //! \brief constructing from a base reference is forbidden
    LocklessPriorityQueue(const QueueInterface& other) = delete;

    //! \brief constructing from a pointer is forbidden
    LocklessPriorityQueue(const QueueInterface* other) = delete;

    //! \brief assignment operator is forbidden
    LocklessPriorityQueue& operator=(const QueueInterface& other) override = delete;

    //! \brief create queue storage
    //!
    //! Allocates the slot pool and message-data region through the registered memory allocator.
    //!
    //! \warning allocates memory exactly once through the memory allocator registry; subsequent
    //! `send` and `receive` calls do not allocate.
    //!
    //! \param id: identifier for the queue, used for memory allocation
    //! \param name: name of queue (unused by this implementation)
    //! \param depth: depth of queue in number of messages
    //! \param messageSize: maximum size of an individual message
    //! \return: status of the creation
    Status create(FwEnumStoreType id,
                  const Fw::ConstStringBase& name,
                  FwSizeType depth,
                  FwSizeType messageSize) override;

    //! \brief tear down the queue
    //!
    //! Returns memory acquired in `create` to the configured memory allocator. Safe to call
    //! repeatedly; only the first call returns memory.
    //!
    //! \warning not thread-safe: the caller must guarantee no concurrent `send`, `receive`,
    //! or `teardown` (including from ISR context) is in flight when this is invoked.
    void teardown() override;

    //! \brief send a message into the queue
    //!
    //! When `blockType` is `NONBLOCKING`, the operation completes in time bounded by
    //! `depth * MAX_RETRY_PASSES` and returns `FULL` if no slot can be claimed. Because slots
    //! held mid-operation by concurrent producers or consumers are not claimable, a
    //! non-blocking send may return `FULL` under contention even though fewer than `depth`
    //! messages are queued. When `blockType` is `BLOCKING`, the operation spins until a slot
    //! becomes available.
    //!
    //! \warning `BLOCKING` calls must not be invoked from ISR context.
    //!
    //! \warning A spurious `FULL` triggers overflow handling on async ports (including
    //! `assert`-on-overflow ports). Size queues with margin or select the mutex-based
    //! `Os::Generic::PriorityQueue` where that is unacceptable.
    //!
    //! \param buffer: message data; must be non-null
    //! \param size: size of message data; must be no greater than the configured message size
    //! \param priority: priority of the message
    //! \param blockType: BLOCKING to spin until space is available; NONBLOCKING to fail fast
    //! \return: status of the send
    Status send(const U8* buffer, FwSizeType size, FwQueuePriorityType priority, BlockingType blockType) override;

    //! \brief receive a message from the queue
    //!
    //! Selects the highest-priority slot and, on a tie, the slot with the smallest sequence
    //! number. When `blockType` is `NONBLOCKING`, the operation completes in time bounded by
    //! `depth * MAX_RETRY_PASSES` and returns `EMPTY` if no slot is available. Because slots
    //! held mid-operation by concurrent producers or consumers are not claimable, a
    //! non-blocking receive may return `EMPTY` under contention even though messages are
    //! queued. When `blockType` is `BLOCKING`, the operation spins until a slot is published.
    //!
    //! \warning `BLOCKING` calls must not be invoked from ISR context.
    //!
    //! \param destination: destination for message data; must be non-null
    //! \param capacity: maximum size of message data the destination can hold; asserted to be
    //! at least the size of the dequeued message. Supplying the configured message size is
    //! always sufficient.
    //! \param blockType: BLOCKING to spin for a message; NONBLOCKING to fail fast
    //! \param actualSize: (output) actual size of the message read on success
    //! \param priority: (output) priority of the message read on success
    //! \return: status of the receive
    Status receive(U8* destination,
                   FwSizeType capacity,
                   BlockingType blockType,
                   FwSizeType& actualSize,
                   FwQueuePriorityType& priority) override;

    //! \brief get number of messages currently receivable
    //!
    //! A message counts only from its READY publication until a consumer claims it, so a
    //! nonzero return means a receive of at least one message can complete.
    //!
    //! \return number of receivable messages currently in the queue
    FwSizeType getMessagesAvailable() const override;

    //! \brief get the maximum number of messages that have been queued at once
    //!
    //! \return high-water mark of message count
    FwSizeType getMessageHighWaterMark() const override;

    //! \brief return the underlying queue handle
    QueueHandle* getHandle() override;

    //! Persistent queue state.
    LocklessPriorityQueueHandle m_handle;
};

}  // namespace Generic
}  // namespace Os

#endif  // OS_GENERIC_LOCKLESSPRIORITYQUEUE_HPP
