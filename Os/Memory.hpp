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
//   1. config/OsDelegateMemory.hpp (CFG) defines the Os::Memory type alias by
//      forward-declaring a link-time delegate (e.g. DelegateMemory) or directly
//      aliasing a concrete implementation.
//      Must not include Os OSAL headers (they aren't yet defined).
//      Should only be included in Os/MemoryInterface.hpp.
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

// Validate that OS_MEMORY_HEADER was defined by config/OsDelegateMemory.hpp
#ifndef OS_MEMORY_HEADER
#error "OS_MEMORY_HEADER must be defined in config/OsDelegateMemory.hpp"
#endif

#include OS_MEMORY_HEADER

#endif  // Os_Memory_hpp
