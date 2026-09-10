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
//      aliasing a concrete implementation (e.g. Va416x0Os::AtomicMutex::AtomicMutex).
//      Must not include Os OSAL headers (they aren't yet defined).
//      Should only be included in Os/MutexInterface.hpp.
//
//   2. Os/MutexInterface.hpp (IF) includes CFG first, then defines
//      MutexHandle, MutexInterface, and ScopeLock.
//
//   3. OS_MUTEX_HEADER (IMPL) is defined by CFG and points to the concrete
//      implementation header. If using delegation, this points to
//      Os/DelegateMutex.hpp. If using compile-time selection, it points
//      directly to a platform-specific implementation (e.g. AtomicMutex.hpp).
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

#endif  // Os_Mutex_hpp
