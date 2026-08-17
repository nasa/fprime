// ======================================================================
// \title Os/CountingSemaphore.hpp
// \brief public Os::CountingSemaphore interface and alias
//
// This header aggregates all definitions needed to use Os::CountingSemaphore:
// the interface, the configured alias, and the concrete delegate type.
//
// WARNING — include order is load-bearing. Do not reorder.
//
// The dependency constraints are:
//
//   1. config/OsDelegateCountingSemaphore.hpp (CFG) defines the
//      Os::CountingSemaphore type alias by forward-declaring a link-time
//      delegate (e.g. DelegateCountingSemaphore) or directly aliasing a
//      concrete implementation.
//      Must not include Os OSAL headers (they aren't yet defined).
//      Should only be included in Os/CountingSemaphoreInterface.hpp.
//
//   2. Os/CountingSemaphoreInterface.hpp (IF) includes CFG first (so the
//      Os::CountingSemaphore alias is available), then defines
//      CountingSemaphoreHandle and CountingSemaphoreInterface.
//
//   3. OS_COUNTING_SEMAPHORE_HEADER (IMPL) is defined by CFG and points to
//      the concrete implementation header. If using delegation, this points
//      to Os/DelegateCountingSemaphore.hpp. If using compile-time selection,
//      it points directly to a platform-specific implementation.
//
// CountingSemaphoreInterface.hpp must precede OS_COUNTING_SEMAPHORE_HEADER
// here, and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef OS_COUNTING_SEMAPHORE_HPP_
#define OS_COUNTING_SEMAPHORE_HPP_

#include "Os/CountingSemaphoreInterface.hpp"

// Validate that OS_COUNTING_SEMAPHORE_HEADER was defined by config/OsDelegateCountingSemaphore.hpp
#ifndef OS_COUNTING_SEMAPHORE_HEADER
#error "OS_COUNTING_SEMAPHORE_HEADER must be defined in config/OsDelegateCountingSemaphore.hpp"
#endif

#include OS_COUNTING_SEMAPHORE_HEADER

#endif  // OS_COUNTING_SEMAPHORE_HPP_
