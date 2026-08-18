// ======================================================================
// \title Os/RawTime.hpp
// \brief public Os::RawTime interface and alias
//
// This header aggregates all definitions needed to use Os::RawTime:
// the interface, the configured alias, and the concrete delegate type.
//
// WARNING — include order is load-bearing. Do not reorder.
//
// The dependency constraints are:
//
//   1. config/OsDelegateRawTime.hpp (CFG) defines the Os::RawTime type alias by
//      forward-declaring a link-time delegate (e.g. DelegateRawTime) or directly
//      aliasing a concrete implementation (e.g. Va416x0Os::TimerRawTime).
//      Must not include Os OSAL headers (they aren't yet defined).
//      Should only be included in Os/RawTimeInterface.hpp.
//
//   2. Os/RawTimeInterface.hpp (IF) needs the Os::RawTime alias in its
//      method signatures, so it includes CFG first. It then defines
//      RawTimeHandle and RawTimeInterface.
//
//   3. OS_RAW_TIME_HEADER (IMPL) is defined by CFG and points to the concrete
//      implementation header. If using delegation, this points to
//      Os/DelegateRawTime.hpp. If using compile-time selection, it points
//      directly to a platform-specific implementation (e.g. TimerRawTime.hpp).
//
// RawTimeInterface.hpp must precede OS_RAW_TIME_HEADER here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef OS_RAWTIME_HPP_
#define OS_RAWTIME_HPP_

#include "Os/RawTimeInterface.hpp"

// Validate that OS_RAW_TIME_HEADER was defined by config/OsDelegateRawTime.hpp
#ifndef OS_RAW_TIME_HEADER
#error "OS_RAW_TIME_HEADER must be defined in config/OsDelegateRawTime.hpp"
#endif

#include OS_RAW_TIME_HEADER

#endif  // OS_RAWTIME_HPP_
