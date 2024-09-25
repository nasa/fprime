// ======================================================================
// \title Os/Posix/Mutex.hpp
// \brief Posix definitions for Os::Mutex
// ======================================================================
#ifndef OS_POSIX_MUTEX_HPP
#define OS_POSIX_MUTEX_HPP
#include <pthread.h>
#include <Os/Mutex.hpp>

namespace Os {
namespace Posix {
namespace Mutex {

struct PosixMutexHandle : public MutexHandle {
    pthread_mutex_t m_mutex_descriptor = PTHREAD_MUTEX_INITIALIZER;
};

//! \brief Posix implementation of Os::Mutex
//!
//! Posix implementation of `MutexInterface` for use as a delegate class handling error-only file operations.
//!
class PosixMutex : public MutexInterface {
  public:
    //! \brief constructor
    //!
    PosixMutex();

    //! \brief destructor
    //!
    ~PosixMutex() override;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    MutexHandle* getHandle() override;

    Status take() override;     //!<  lock the mutex and get return status
    Status release() override;  //!<  unlock the mutex and get return status

  private:
    //! Handle for PosixMutex
    PosixMutexHandle m_handle;
};

}  // namespace Mutex
}  // namespace Posix
}  // namespace Os
#endif  // OS_POSIX_MUTEX_HPP
