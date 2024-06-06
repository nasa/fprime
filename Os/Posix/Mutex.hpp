// ======================================================================
// \title Os/Posix/Mutex.hpp
// \brief Posix definitions for Os::Mutex
// ======================================================================
#include "Os/Mutex.hpp"

#ifndef OS_POSIX_MUTEX_HPP
#define OS_POSIX_MUTEX_HPP
namespace Os {
namespace Posix {
namespace Mutex {

struct PosixMutexHandle : public MutexHandle {
    static constexpr PlatformIntType INVALID_MUTEX_DESCRIPTOR = -1;
    PlatformIntType m_mutex_descriptor = INVALID_MUTEX_DESCRIPTOR;  // TODO: is m_mutex_descriptor what we want?
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

    void lock() override;    //!<  lock the mutex
    void unLock() override;  //!<  unlock the mutex

  private:
    //! Handle for PosixMutex
    PosixMutexHandle m_handle;
};

}  // namespace Mutex
}  // namespace Posix
}  // namespace Os
#endif  // OS_POSIX_MUTEX_HPP
