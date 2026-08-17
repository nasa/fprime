// ======================================================================
// \title Os/Mutex.hpp
// \brief public Os::Mutex interface and alias
//
// This header aggregates all definitions needed to use Os::Mutex:
// the interface, the configured alias, and the concrete delegate type.
//
// WARNING — include order is load-bearing. Do not reorder.
//
// The dependency constraints are:
//
//   1. config/OsDelegateMutex.hpp (CFG) defines the Os::Mutex type alias by
//      forward-declaring a link-time delegate (e.g. DelegateMutex) or directly
//      aliasing a concrete implementation (e.g. Va416x0Os::MaskingMutex).
//      Must not include Os OSAL headers (they aren't yet defined).
//      Should only be included in Os/MutexInterface.hpp.
//
//   2. Os/MutexInterface.hpp (IF) includes CFG first (so the Os::Mutex alias is
//      available), then defines MutexHandle and MutexInterface.
//
//   3. OS_MUTEX_HEADER (IMPL) is defined by CFG and points to the concrete
//      implementation header. If using delegation, this points to
//      Os/DelegateMutex.hpp. If using compile-time selection, it points
//      directly to a platform-specific implementation (e.g. MaskingMutex.hpp).
//
// MutexInterface.hpp must precede OS_MUTEX_HEADER here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef Os_Mutex_hpp
#define Os_Mutex_hpp

#include "Os/MutexInterface.hpp"

// Validate that OS_MUTEX_HEADER was defined by config/OsDelegateMutex.hpp
#ifndef OS_MUTEX_HEADER
#error "OS_MUTEX_HEADER must be defined in config/OsDelegateMutex.hpp"
#endif

#include OS_MUTEX_HEADER

namespace Os {

//! \brief locks a mutex within the current scope
//!
//! The scope lock will lock the associated mutex immediately and will ensure the mutex is unlock when the scope lock
//! is destroyed.
//!
//! \warning it is unadvisable to dynamically allocate ScopeLock as this violates the implied usage.
class ScopeLock {
  public:
    //! \brief construct the scope lock
    //!
    //! Will lock the supplied mutex and will unlock the mutex when this object goes out of scope.
    //! \param mutex
    explicit ScopeLock(Mutex& mutex);

    //!\brief unlock the scoped mutex
    ~ScopeLock();

    //! \brief copy constructor is forbidden
    ScopeLock(const ScopeLock& other) = delete;

    //! \brief copy constructor is forbidden
    ScopeLock(const ScopeLock* other) = delete;

    //! \brief assignment operator is forbidden
    ScopeLock& operator=(const ScopeLock& other) = delete;

  private:
    Mutex& m_mutex;  //!< Stores the mutex reference
};
}  // namespace Os

#endif
