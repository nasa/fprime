// ======================================================================
// \title Os/Stub/Mutex.hpp
// \brief stub definitions for Os::Mutex
// ======================================================================
#include "Os/Mutex.hpp"

#ifndef OS_STUB_MUTEX_HPP
#define OS_STUB_MUTEX_HPP
namespace Os {
namespace Stub {
namespace Mutex {

struct StubMutexHandle : public MutexHandle {};

//! \brief stub implementation of Os::Mutex
//!
//! Stub implementation of `MutexInterface` for use as a delegate class handling error-only file operations.
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
