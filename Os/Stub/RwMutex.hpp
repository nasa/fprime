// ======================================================================
// \title Os/Stub/RwMutex.hpp
// \brief stub definitions for Os::RwMutex
// ======================================================================
#ifndef OS_STUB_RWMUTEX_HPP
#define OS_STUB_RWMUTEX_HPP
#include <atomic>
#include <Os/RwMutex.hpp>

namespace Os {
namespace Stub {
namespace Mutex {

struct StubRwMutexHandle : public MutexHandle {
    //! \brief State constants for the lock state machine
    enum LockState : FwSignedSizeType {
        FREE = 0,
        WRITER_LOCKED = -1
    };

    //! \brief Count of writers waiting to acquire the lock
    //!
    //! Used to implement writer-preference policy: when this count is non-zero,
    //! new readers block to allow waiting writers to proceed. This prevents
    //! writer starvation in high-contention scenarios.
    //!
    //! \note This counter is incremented before attempting to acquire the lock
    //!       in take(), and decremented after release in release().
    std::atomic<FwSizeType> m_writers_waiting = {0u};

    //! \brief Combined lock state: reader count, free, or writer-locked
    //!
    //! The m_lock_state atomic variable uses signed values to encode three states:
    //!
    //! - n > 0: Number of active shared (read) lock holders
    //!
    //! - 0 (`LockState::FREE`): Mutex is unlocked, no readers or writers
    //!
    //! - -1 (`LockState::WRITER_LOCKED`): Mutex is held exclusively by a writer
    //!
    //! \note State transitions must be performed atomically via compare_exchange
    //!       to avoid races between readers and writers.
    std::atomic<FwSignedSizeType> m_lock_state = {LockState::FREE};
};

//! \brief Blocking stub implementation of Os::RwMutex
//!
//! This delegate class provides a blocking simulation of the
//! reader-writer mutex interface. It is intended for platforms
//! without native RW-mutex support but with threading capability.
//!
//! \b Behavior:
//!
//! - \b Writer \b preference \b policy: Waiting writers block new readers to prevent
//!   writer starvation. Once a writer requests the lock, subsequent readers wait
//!   until the writer completes.
//!
//! - Multiple readers may hold the lock concurrently when no writer is active
//!
//! - Recursive locking by the same thread may result in deadlock
//!
//! - No actual thread suspension: blocking is simulated via spin-lock
//!
//! \warning This implementation uses atomic operations for state tracking but does not
//! provide real cross-thread synchronization. It assumes cooperative scheduling.
//!
class StubRwMutex : public RwMutexInterface {
  public:
    //! \brief constructor
    //!
    StubRwMutex() = default;

    //! \brief destructor
    //!
    ~StubRwMutex() override = default;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    MutexHandle* getHandle() override;

    // ------------------------------------
    // Exclusive (write) lock operations
    // ------------------------------------

    Status take() override;         //!<  Acquire an exclusive (write) lock and return status
    Status release() override;      //!<  Release an exclusive (write) lock and return status

    // ------------------------------------
    // Shared (read) lock operations
    // ------------------------------------

    Status takeRead() override;     //!<  Acquire a shared (read) lock and return status
    Status releaseRead() override;  //!<  Release a shared (read) lock and return status

  private:
    //! Handle for StubMutex
    StubRwMutexHandle m_handle;
};

}  // namespace Mutex
}  // namespace Stub
}  // namespace Os
#endif  // OS_STUB_RWMUTEX_HPP
