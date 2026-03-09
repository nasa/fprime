// ======================================================================
// \title Os/Posix/RwMutex.hpp
// \brief Posix definitions for Os::RwMutex
// ======================================================================
#ifndef OS_POSIX_RWMUTEX_HPP
#define OS_POSIX_RWMUTEX_HPP
#include <pthread.h>
#include <Os/RwMutex.hpp>

namespace Os {
namespace Posix {
namespace Mutex {

struct PosixRwMutexHandle : public MutexHandle {
    pthread_rwlock_t m_rwlock_descriptor = PTHREAD_RWLOCK_INITIALIZER;
};

//! \brief Posix implementation of Os::RwMutex
//!
//! Posix implementation of `RwMutexInterface` for use as a delegate class handling error-only file operations.
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
//! \warning Does \b NOT support priority inheritance (`PTHREAD_PRIO_INHERIT`): priority
//! inversion is possible in real-time scenarios. Use `Os::Mutex` with priority
//! inheritance for critical sections where this matters.
//!
//! \warning Does \b NOT support error-checking mode (`PTHREAD_MUTEX_ERRORCHECK`): recursive
//! locks or unlocking by non-owner will cause undefined behavior, not return
//! an error code. Ensure correct usage.
//!
class PosixRwMutex : public RwMutexInterface {
  public:
    //! \brief constructor
    //!
    PosixRwMutex();

    //! \brief destructor
    //!
    ~PosixRwMutex() override;

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
    //! Handle for PosixRwMutex
    PosixRwMutexHandle m_handle;
};

}  // namespace Mutex
}  // namespace Posix
}  // namespace Os
#endif  // OS_POSIX_RWMUTEX_HPP
