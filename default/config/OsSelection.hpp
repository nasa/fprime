// ======================================================================
// \title config/OsSelection.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how the Os services below (RawTime, Mutex, and
// ConditionVariable) resolve to concrete implementations. Two mechanisms are
// available:
//
// 1. Link-time selection (default): each service aliases to an Os::Delegate*
//   class that wraps the corresponding *Interface. At construction, the
//   delegate calls <Service>Interface::getDelegate() to construct the
//   platform-specific implementation (e.g., PosixRawTime, StubMutex) via
//   placement-new. The linker selects which getDelegate() based on which
//   Default<Service>.cpp is linked. Calls dispatch through the vtable at
//   runtime.
//
// 2. Compile-time selection (performance optimization): platforms may alias
//   the Os::<Service> name directly to a concrete implementation. This
//   eliminates the wrapper and virtual dispatch, enabling inlining and
//   aggressive LTO optimization (e.g., Mutex lock()/unLock()/ScopeLock are
//   defined inline on MutexInterface, so acquisitions devirtualize without
//   requiring LTO).
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::<Service> aliases.
//   - Each OS_*_HEADER below points to the implementation header, which is
//     included AFTER the corresponding *Interface.hpp by the public
//     Os/<Service>.hpp header (e.g., OS_RAW_TIME_HEADER is included after
//     RawTimeInterface.hpp in Os/RawTime.hpp).
//   - Violating this constraint will create circular dependencies.
//
// IMPORTANT: a project override of this header replaces it entirely, so the
// override must define ALL aliases and OS_*_HEADER macros below, not just the
// service being changed.
//
// ----------------------------------------------------------------------
// RawTime
//
// Compile-time selection eliminates the delegate wrapper and virtual dispatch
// on the timestamp read path, which is valuable for platforms with tight
// timing constraints.
//
// Example compile-time selection override:
//
//     namespace Va416x0Os { class TimerRawTime; }
//     namespace Os { using RawTime = Va416x0Os::TimerRawTime; }
//     #define OS_RAW_TIME_HEADER "Va416x0/Os/TimerRawTime/TimerRawTime.hpp"
//
// ----------------------------------------------------------------------
// Mutex and ConditionVariable
//
// The two are configured together because a condition variable operates on the
// handle of the configured Os::Mutex, so they must always be selected as a
// compatible pair.
//
// WARNING: compile-time selections MUST derive from Os::MutexInterface
// (lock()/unLock()/ScopeLock are defined there) and
// Os::ConditionVariableInterface (wait() is defined there). Os::Mutex and
// Os::ConditionVariable MUST be overridden together, and the selected
// ConditionVariable MUST accept the handle of the selected Os::Mutex: a
// ConditionVariable implementation typically casts Os::Mutex::getHandle() to
// its own MutexHandle type (e.g. Os/Posix/ConditionVariable.cpp
// reinterpret_casts it to PosixMutexHandle). Pairing a Mutex with a
// ConditionVariable that expects a different handle type is undefined behavior
// that the compiler cannot detect. The one case it can detect is enforced:
// Os/ConditionVariableInterface.hpp static_asserts that either both or neither
// of the two are the link-time delegates, so aliasing Os::Mutex alone is a
// build error. A ConditionVariable that does not inspect the mutex handle
// (e.g. Os::Stub::Mutex::StubConditionVariable) is compatible with any Mutex.
//
// The alias only changes the C++ type; the implementation module providing the
// aliased classes must still be in the link (via CHOOSES_IMPLEMENTATIONS for an
// in-tree Os_Mutex_* implementation, or DEPENDS for a project module). The
// Os_Mutex module always REQUIRES_IMPLEMENTATIONS Os_Mutex, so a chosen
// implementation is still needed even though its getDelegate() goes unused.
//
// Example compile-time selection override (project mutex, no-op condition
// variable):
//
//     namespace Va416x0Os { namespace AtomicMutex { class AtomicMutex; } }
//     namespace Os { namespace Stub { namespace Mutex { class StubConditionVariable; } } }
//     namespace Os {
//     using Mutex = Va416x0Os::AtomicMutex::AtomicMutex;
//     using ConditionVariable = Os::Stub::Mutex::StubConditionVariable;
//     }  // namespace Os
//     #define OS_MUTEX_HEADER "Va416x0/Os/AtomicMutex/AtomicMutex.hpp"
//     #define OS_CONDITION_VARIABLE_HEADER "Os/Stub/ConditionVariable.hpp"
// ======================================================================
#ifndef CONFIG_OSSELECTION_HPP
#define CONFIG_OSSELECTION_HPP

//!< Forward declarations of the link-time delegates
//!< Default: select the implementations at link time
namespace Os {

class DelegateRawTime;
using RawTime = DelegateRawTime;

class DelegateMutex;
using Mutex = DelegateMutex;

class DelegateConditionVariable;
using ConditionVariable = DelegateConditionVariable;

}  // namespace Os

#define OS_RAW_TIME_HEADER <Os/DelegateRawTime.hpp>
#define OS_MUTEX_HEADER <Os/DelegateMutex.hpp>
#define OS_CONDITION_VARIABLE_HEADER <Os/DelegateConditionVariable.hpp>

#endif  // CONFIG_OSSELECTION_HPP
