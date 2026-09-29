// ======================================================================
// \title Os/Cpu.hpp
// \brief public Os::Cpu interface and alias
//
// This header aggregates all definitions needed to use Os::Cpu:
// the interface, the configured alias, and the concrete delegate type.
//
// WARNING — include order is load-bearing. Do not reorder.
//
// The dependency constraints are:
//
//   1. config/OsDelegateCpu.hpp (CFG) defines the Os::Cpu type alias by
//      forward-declaring a link-time delegate (e.g. DelegateCpu) or directly
//      aliasing a concrete implementation.
//      Must not include Os OSAL headers (they aren't yet defined).
//      Should only be included in Os/CpuInterface.hpp.
//
//   2. Os/CpuInterface.hpp (IF) includes CFG first (so the Os::Cpu alias is
//      available), then defines CpuHandle and CpuInterface.
//
//   3. OS_CPU_HEADER (IMPL) is defined by CFG and points to the concrete
//      implementation header. If using delegation, this points to
//      Os/DelegateCpu.hpp. If using compile-time selection, it points
//      directly to a platform-specific implementation.
//
// CpuInterface.hpp must precede OS_CPU_HEADER here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef Os_Cpu_hpp
#define Os_Cpu_hpp

#include "Os/CpuInterface.hpp"

// Validate that OS_CPU_HEADER was defined by config/OsDelegateCpu.hpp
#ifndef OS_CPU_HEADER
#error "OS_CPU_HEADER must be defined in config/OsDelegateCpu.hpp"
#endif

#include OS_CPU_HEADER

#endif  // Os_Cpu_hpp
