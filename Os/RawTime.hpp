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
//   1. config/OsDelegateRawTime.hpp (CFG) defines the Os::RawTime type alias and
//      forward-declares the concrete type (e.g. DelegateRawTime). It must
//      not itself include any Os OSAL headers, because it is included before
//      those headers are fully defined.
//
//   2. Os/RawTimeInterface.hpp (IF) needs the Os::RawTime alias in its
//      method signatures, so it includes CFG first. It then defines
//      RawTimeHandle and RawTimeInterface.
//
//   3. Os/DelegateRawTime.hpp (ALIAS) needs RawTimeInterface, so it includes
//      IF. It then provides the full definition of the concrete type that
//      CFG forward-declared.
//
// Consequence: RawTimeInterface.hpp must precede DelegateRawTime.hpp here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef OS_RAWTIME_HPP_
#define OS_RAWTIME_HPP_

#include "Os/RawTimeInterface.hpp"
#include OS_RAW_TIME_HEADER

#endif  // OS_RAWTIME_HPP_
