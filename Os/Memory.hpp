// ======================================================================
// \title Os/Memory.hpp
// \brief public Os::Memory interface and alias
//
// This header aggregates all definitions needed to use Os::Memory:
// the interface, the configured alias, and the concrete delegate type.
//
// WARNING — include order is load-bearing. Do not reorder.
//
// The dependency constraints are:
//
//   1. config/OsSelection.hpp (CFG) defines the Os::Memory type alias by
//      forward-declaring a link-time delegate (e.g. DelegateMemory) or directly
//      aliasing a concrete implementation.
//      Must not include Os OSAL headers (they aren't yet defined).
//      Included by Os/MemoryInterface.hpp, which this header includes first.
//
//   2. Os/MemoryInterface.hpp (IF) includes CFG first (so the Os::Memory alias is
//      available), then defines MemoryHandle and MemoryInterface.
//
//   3. OS_MEMORY_HEADER (IMPL) is defined by CFG and points to the concrete
//      implementation header. If using delegation, this points to
//      Os/DelegateMemory.hpp. If using compile-time selection, it points
//      directly to a platform-specific implementation.
//
// MemoryInterface.hpp must precede OS_MEMORY_HEADER here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef Os_Memory_hpp
#define Os_Memory_hpp

#include "Os/MemoryInterface.hpp"

// A project config/OsSelection.hpp that predates the Memory selection defines neither the Os::Memory
// alias nor OS_MEMORY_HEADER; fall back to the link-time delegate so such projects keep building.
#ifndef OS_MEMORY_HEADER
#include "Os/DelegateMemory.hpp"
namespace Os {
using Memory = DelegateMemory;
}
#else
#include OS_MEMORY_HEADER
#endif

#endif  // Os_Memory_hpp
