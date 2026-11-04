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
//   1. config/OsSelection.hpp (CFG) defines the
//      Os::CountingSemaphore type alias by forward-declaring a link-time
//      delegate (e.g. DelegateCountingSemaphore) or directly aliasing a
//      concrete implementation.
//      Must not include Os OSAL headers (they aren't yet defined).
//      Included by Os/CountingSemaphoreInterface.hpp, which this header includes first.
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

// A project config/OsSelection.hpp that predates the CountingSemaphore selection defines neither the
// Os::CountingSemaphore alias nor OS_COUNTING_SEMAPHORE_HEADER; fall back to the link-time delegate so such projects
// keep building.
#ifndef OS_COUNTING_SEMAPHORE_HEADER
#include "Os/DelegateCountingSemaphore.hpp"
namespace Os {
using CountingSemaphore = DelegateCountingSemaphore;
}
#else
#include OS_COUNTING_SEMAPHORE_HEADER
#endif

#endif  // OS_COUNTING_SEMAPHORE_HPP_
