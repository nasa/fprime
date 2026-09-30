// ======================================================================
// \title config/OsDelegateMutex.hpp
// \brief configured selection of the Os::Mutex and Os::ConditionVariable implementations
//
// This header configures how Os::Mutex and Os::ConditionVariable resolve to
// concrete implementations. The two are configured together because a
// condition variable operates on the handle of the configured Os::Mutex, so
// they must always be selected as a compatible pair. Two mechanisms are
// available:
//
// 1. Link-time selection (default): Os::Mutex aliases to Os::DelegateMutex,
//    which wraps a MutexInterface reference. At construction, DelegateMutex
//    calls MutexInterface::getDelegate() to construct the platform-specific
//    implementation (e.g., Stub::Mutex::StubMutex) via placement-new. The linker
//    selects which getDelegate() based on which DefaultMutex.cpp is linked.
//    Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::Mutex directly to a concrete implementation
//    (e.g., Va416x0Os::AtomicMutex::AtomicMutex). This eliminates the wrapper and
//    virtual dispatch, enabling inlining (lock()/unLock()/ScopeLock are defined
//    inline on MutexInterface, so acquisitions devirtualize without requiring LTO).
//
//    WARNING: the aliased types MUST derive from Os::MutexInterface (lock()/unLock()/
//    ScopeLock are defined there) and Os::ConditionVariableInterface (wait() is
//    defined there). Os::Mutex and Os::ConditionVariable MUST be overridden
//    together, and the selected ConditionVariable MUST accept the handle of the
//    selected Os::Mutex: a ConditionVariable implementation typically casts
//    Os::Mutex::getHandle() to its own MutexHandle type (e.g.
//    Os/Posix/ConditionVariable.cpp reinterpret_casts it to PosixMutexHandle).
//    Pairing a Mutex with a ConditionVariable that expects a different handle
//    type is undefined behavior that the compiler cannot detect. The one case it
//    can detect is enforced: Os/ConditionVariableInterface.hpp static_asserts
//    that either both or neither of the two are the link-time delegates, so
//    aliasing Os::Mutex alone is a build error. A ConditionVariable that does
//    not inspect the mutex handle (e.g. Os::Stub::Mutex::StubConditionVariable)
//    is compatible with any Mutex.
//
//    The alias only changes the C++ type; the implementation module providing
//    the aliased classes must still be in the link (via CHOOSES_IMPLEMENTATIONS
//    for an in-tree Os_Mutex_* implementation, or DEPENDS for a project module).
//    The Os_Mutex module always REQUIRES_IMPLEMENTATIONS Os_Mutex, so a chosen
//    implementation is still needed even though its getDelegate() goes unused.
//
// Example compile-time selection override (project mutex, no-op condition variable):
//
//     namespace Va416x0Os { namespace AtomicMutex { class AtomicMutex; } }
//     namespace Os { namespace Stub { namespace Mutex { class StubConditionVariable; } } }
//     namespace Os {
//     using Mutex = Va416x0Os::AtomicMutex::AtomicMutex;
//     using ConditionVariable = Os::Stub::Mutex::StubConditionVariable;
//     }  // namespace Os
//     #define OS_MUTEX_HEADER "Va416x0/Os/AtomicMutex/AtomicMutex.hpp"
//     #define OS_CONDITION_VARIABLE_HEADER "Os/Stub/ConditionVariable.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::Mutex and
//     Os::ConditionVariable aliases.
//   - OS_MUTEX_HEADER should point to the implementation header, which will
//     be included AFTER MutexInterface.hpp in Os/Mutex.hpp.
//   - OS_CONDITION_VARIABLE_HEADER should point to the implementation header,
//     which will be included AFTER ConditionVariableInterface.hpp in
//     Os/Condition.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATEMUTEX_HPP
#define CONFIG_OS_DELEGATEMUTEX_HPP

//!< Forward declaration of the link-time delegates
//!< Default: select the Mutex and ConditionVariable implementations at link time
namespace Os {

class DelegateMutex;
using Mutex = DelegateMutex;

class DelegateConditionVariable;
using ConditionVariable = DelegateConditionVariable;

}  // namespace Os

#define OS_MUTEX_HEADER <Os/DelegateMutex.hpp>
#define OS_CONDITION_VARIABLE_HEADER <Os/DelegateConditionVariable.hpp>

#endif  // CONFIG_OS_DELEGATEMUTEX_HPP
