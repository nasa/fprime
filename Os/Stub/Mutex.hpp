// ======================================================================
// \title Os/Stub/Mutex.hpp
// \brief stub definitions for Os::Mutex
// ======================================================================
#include "Os/Mutex.hpp"

#include <atomic>

#ifndef OS_STUB_MUTEX_HPP
#define OS_STUB_MUTEX_HPP
namespace Os {
namespace Stub {
namespace Mutex {

struct StubMutexHandle : public MutexHandle {
    //! True if the mutex has been acquired without being released.
    std::atomic<bool> m_mutex_taken = {false};
};

//! \brief Nonblocking stub implementation of Os::Mutex
//!
//! Stub implementation of `MutexInterface` for use as a delegate class.
//!
//! This mutex will never block, which allows it to be used on any platform without OS dependencies.
//! It is unsuitable for use in environments where threads need to contend over mutexes.
//! However, it is appropriate for use in environments with multiple threads that are not intended to
//! contend over mutexes, and where contention would indicate a coding defect worthy of an assertion.
//!
class StubMutex : public MutexInterface {
  public:
    //! \brief constructor
    //!
    StubMutex() = default;

    //! \brief destructor
    //!
    ~StubMutex() override = default;

    //! \brief return the underlying mutex handle (implementation specific)
    //! \return internal mutex handle representation
    MutexHandle* getHandle() override;

    Status take() override;     //!<  lock the mutex and get return status
    Status release() override;  //!<  unlock the mutex and get return status

  private:
    //! Handle for StubMutex
    StubMutexHandle m_handle;
};

} // namespace Mutex
} // namespace Stub
} // namespace Os
#endif // OS_STUB_MUTEX_HPP
