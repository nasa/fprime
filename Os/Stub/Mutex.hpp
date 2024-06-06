// ======================================================================
// \title Os/Stub/Mutex.hpp
// \brief stub file definitions for Os::Mutex
// ======================================================================
#include "Os/Mutex.hpp"

#ifndef OS_STUB_MUTEX_HPP
#define OS_STUB_MUTEX_HPP
namespace Os {
namespace Stub {
namespace Mutex {

struct StubMutexHandle : public MutexHandle {
    static constexpr PlatformIntType INVALID_MUTEX_DESCRIPTOR = -1;
    PlatformIntType m_handle = INVALID_MUTEX_DESCRIPTOR;
};

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

    void lock() override; //!<  lock the mutex
    void unLock() override; //!<  unlock the mutex

  private:
    //! Handle for StubMutex
    StubMutexHandle m_handle;
};

} // namespace Mutex
} // namespace Stub
} // namespace Os
#endif // OS_STUB_MUTEX_HPP
