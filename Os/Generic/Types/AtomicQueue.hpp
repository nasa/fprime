// ======================================================================
// \title  AtomicQueue.hpp
// \author B. Duckett
// \brief  A lock-free FIFO queue using atomics for thread/ISR safety
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef OS_GENERIC_TYPES_ATOMIC_QUEUE_HPP
#define OS_GENERIC_TYPES_ATOMIC_QUEUE_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/ByteArray.hpp>
#include <Fw/Types/MemAllocator.hpp>
#include <Os/CountingSemaphore.hpp>
#include <atomic>

namespace Types {

//! \class AtomicQueue
//! \brief A lock-free MPMC FIFO circular buffer with fixed-size buffer storage
//!
//! This queue stores fixed-size message buffers using memcpy semantics. Each slot
//! contains an embedded buffer (not just a pointer). The circular buffer uses atomic
//! sequence numbers for lock-free coordination between multiple producers/consumers.
//!
//! Features:
//! - O(1) enqueue/dequeue with bounded CAS retries
//! - Embedded buffer storage (memcpy on send/receive)
//! - Optional blocking enqueue via Os::CountingSemaphore (platform-agnostic)
//! - Uses only word-size atomics (no DWCAS), portable to all architectures
//!
//! Pre-allocates memory for slots and buffers during create()
//! \note Power-of-2 capacity uses fast bitwise AND; other sizes use modulo (~5-20% slower)
//!
//! \warning Position Counter Wrap-Around: The m_enqueuePos and m_dequeuePos counters are
//! FwSizeType (platform word size). On 64-bit platforms (FwSizeType=U64), wrap-around
//! is functionally impossible (~584 years at 1 GHz). However, on 32-bit platforms
//! (FwSizeType=U32), wrap-around occurs after 2^32 operations (~1.2 hours at 1M ops/sec).
//! The algorithm remains correct after wrap (sequence numbers prevent ABA), but applications
//! with sustained high throughput on 32-bit systems should be aware. FwSizeType=U32 is
//! used (instead of forcing U64) to support platforms with 32-bit native word size where
//! 64-bit atomics may not be lock-free or require expensive emulation.
class AtomicQueue {
  public:
    //! \brief AtomicQueue constructor
    AtomicQueue();

    //! \brief AtomicQueue destructor
    ~AtomicQueue();

    //! \brief Create the queue with embedded buffer storage
    //!
    //! Creates queue with blocking semaphore support for enqueueBlocking().
    //!
    //! \param numBuffers maximum number of messages (any value > 0)
    //! \param bufferSize size of each message buffer in bytes
    //! \param allocator memory allocator for dynamic allocation
    //! \param allocatorId allocator identifier for tracking
    void create(FwSizeType numBuffers, FwSizeType bufferSize, Fw::MemAllocator& allocator, FwEnumStoreType allocatorId);

    //! \brief Teardown the queue and free allocated memory
    void teardown();

    //! \brief Enqueue a message (multi-producer safe, non-blocking, O(1))
    //!
    //! Copies message data into an available slot using memcpy. Multiple producers
    //! can safely call this concurrently. Never blocks.
    //!
    //! ISR-SAFETY: Platform-dependent (calls semaphore tryWait). ISR-safe on:
    //! VxWorks, FreeRTOS, INTEGRITY, ThreadX, RTEMS, QNX, Zephyr, embOS, µC/OS.
    //! NOT ISR-safe on: POSIX RT, Linux (standard/non-RT). Verify platform before ISR use.
    //!
    //! \param buffer source buffer to copy from
    //! \param size size of data to copy (must be ≤ bufferSize)
    //! \return true if successful, false if queue is full or retry limit exceeded
    bool enqueue(const U8* buffer, FwSizeType size);

    //! \brief Enqueue with optional blocking (multi-producer safe, O(1))
    //!
    //! Copies message data into available slot. If queue is full and blocking enabled,
    //! waits on semaphore until space available.
    //!
    //! WARNING: When blockIfFull=true, this function can block and is NOT ISR-safe.
    //! For ISR contexts, always use enqueue() or set blockIfFull=false.
    //!
    //! \param buffer source buffer to copy from
    //! \param size size of data to copy (must be ≤ bufferSize)
    //! \param blockIfFull if true, blocks when full (caller must ensure not in ISR)
    //! \return true if successful, false if queue full (non-blocking mode)
    bool enqueueBlocking(const U8* buffer, FwSizeType size, bool blockIfFull);

    //! \brief Dequeue a message (multi-consumer safe, non-blocking, O(1))
    //!
    //! Copies message data from oldest slot using memcpy. Multiple consumers can
    //! safely call this concurrently. Never blocks.
    //!
    //! ISR-SAFETY: Depends on platform semaphore post() implementation. ISR-safe on:
    //! VxWorks, FreeRTOS, Green Hills INTEGRITY, ThreadX, RTEMS, QNX Neutrino,
    //! Zephyr RTOS, embOS, µC/OS-II, µC/OS-III, SafeRTOS, Azure RTOS.
    //! NOT ISR-safe on: POSIX RT (strict spec), Linux (standard/non-RT), Embedded
    //! Linux without RT patches. Verify platform semaphore before ISR use.
    //!
    //! \param buffer destination buffer to copy into
    //! \param capacity size of destination buffer
    //! \param actualSize output parameter for actual message size copied
    //! \return true if successful, false if queue is empty or retry limit exceeded
    bool dequeue(U8* buffer, FwSizeType capacity, FwSizeType& actualSize);

    //! \brief Check if the queue is full
    //!
    //! \return true if all nodes are in use, false otherwise
    bool isFull() const;

    //! \brief Check if the queue is empty
    //!
    //! \return true if no nodes are in the queue, false otherwise
    bool isEmpty() const;

    //! \brief Get the current number of elements in the queue
    //!
    //! \return current size of the queue
    FwSizeType getSize() const;

    //! \brief Get the maximum capacity of the queue
    //!
    //! \return maximum number of buffers
    FwSizeType getCapacity() const;

    //! \brief Get the buffer size for each message
    //!
    //! \return buffer size in bytes
    FwSizeType getBufferSize() const;

    //! \brief Check if queue has been successfully created
    //!
    //! \return true if create() completed successfully, false otherwise
    bool isCreated() const { return this->m_slots != nullptr && this->m_capacity > 0; }

  private:
    //! Maximum CAS retry attempts (JPL Power of Ten: bounded loops)
    static constexpr FwSizeType MAX_CAS_RETRIES = 100;

    //! \brief Slot structure for circular buffer with embedded buffer storage
    //!
    //! Each slot contains an embedded buffer and sequence number for lock-free coordination.
    //! Sequence encoding:
    //! - seq == index: ready for write (producer can claim)
    //! - seq == index + 1: ready for read (consumer can claim)
    //! - seq == index + capacity: completed read, next cycle's write position
    //!
    //! Memory layout: Natural alignment (~24 bytes on 64-bit platforms)
    //! For large slot counts (>20K), this saves significant memory vs cache line alignment
    struct Slot {
        U8* buffer;                        // Embedded message buffer
        FwSizeType size;                   // Actual message size stored
        std::atomic<FwSizeType> sequence;  // Coordination sequence number
    };

    //! \brief Calculate slot index from position
    //!
    //! Uses bitwise AND for power-of-2 capacity (fast), modulo otherwise.
    //! \param pos position value
    //! \return slot index in range [0, capacity)
    inline FwSizeType getIndex(FwSizeType pos) const {
        return (this->m_mask != 0) ? (pos & this->m_mask) : (pos % this->m_capacity);
    }

    //! \brief Compute simple checksum for diagnostic logging
    static U32 computeChecksum(const U8* buffer, FwSizeType size);

    //! \brief Internal enqueue without semaphore interaction
    //!
    //! Used by both enqueue() and enqueueBlocking() to avoid double-decrementing
    //! the semaphore. Performs the lock-free enqueue operation only.
    //!
    //! \param buffer source buffer to copy from
    //! \param size size of data to copy
    //! \return true if successful, false if queue full
    bool enqueueInternal(const U8* buffer, FwSizeType size);

    // Private members:
    Slot* m_slots;                         // Circular slot array
    U8* m_bufferMemory;                    // Contiguous buffer memory block
    FwSizeType m_capacity;                 // Number of message buffers
    FwSizeType m_bufferSize;               // Size of each message buffer
    FwSizeType m_mask;                     // Bitmask if power-of-2, else 0
    std::atomic<FwSizeType> m_enqueuePos;  // Next enqueue position (producer cursor)
    std::atomic<FwSizeType> m_dequeuePos;  // Next dequeue position (consumer cursor)
    Fw::MemAllocator* m_allocator;         // Memory allocator (nullptr if not using allocator)
    FwEnumStoreType m_allocatorId;         // Allocator identifier for deallocation
    Os::CountingSemaphore* m_notFullSem;   // Semaphore for blocking enqueue (all platforms)
    FwEnumStoreType m_id;                  // Queue identifier for logging
};

}  // namespace Types

#endif  // OS_GENERIC_TYPES_ATOMIC_QUEUE_HPP
