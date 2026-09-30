// ======================================================================
// \title Os/Condition.hpp
// \brief public Os::ConditionVariable interface and alias
//
// This header aggregates all definitions needed to use Os::ConditionVariable:
// the interface, the configured alias, and the concrete delegate type.
//
// The includes below must appear in the order given: each depends on names
// defined by the one before it, so reordering them will fail to compile.
// The ordering constraints are:
//
//   1. config/OsDelegateMutex.hpp (CFG) defines the Os::ConditionVariable type
//      alias (alongside Os::Mutex, as the two must be selected as a pair) by
//      forward-declaring a link-time delegate (e.g. DelegateConditionVariable)
//      or directly aliasing a concrete implementation.
//      Must not include Os OSAL headers (they aren't yet defined).
//      Should only be included in Os/MutexInterface.hpp and
//      Os/ConditionVariableInterface.hpp.
//
//   2. Os/ConditionVariableInterface.hpp (IF) includes CFG first, then defines
//      ConditionVariableHandle and ConditionVariableInterface.
//
//   3. OS_CONDITION_VARIABLE_HEADER (IMPL) is defined by CFG and points to the
//      concrete implementation header. If using delegation, this points to
//      Os/DelegateConditionVariable.hpp. If using compile-time selection, it
//      points directly to a platform-specific implementation.
//
// ConditionVariableInterface.hpp must precede OS_CONDITION_VARIABLE_HEADER here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef OS_CONDITION_HPP_
#define OS_CONDITION_HPP_

#include "Os/ConditionVariableInterface.hpp"

// Validate that OS_CONDITION_VARIABLE_HEADER was defined by config/OsDelegateMutex.hpp
#ifndef OS_CONDITION_VARIABLE_HEADER
#error "OS_CONDITION_VARIABLE_HEADER must be defined in config/OsDelegateMutex.hpp, paired with Os::Mutex"
#endif

#include OS_CONDITION_VARIABLE_HEADER

#endif  // OS_CONDITION_HPP_
