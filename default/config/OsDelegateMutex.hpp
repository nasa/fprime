// ======================================================================
// \title config/OsDelegateMutex.hpp
// \brief configured selection of the Os::Mutex and Os::ConditionVariable implementations
//
// This header configures how Os::Mutex and Os::ConditionVariable resolve to
// concrete implementations. The two are configured together because a
// condition variable operates on the handle of the configured Os::Mutex, so
// they must always come from the same implementation. Two mechanisms are
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
//    defined there). Os::Mutex and Os::ConditionVariable MUST be overridden as a
//    pair from the same implementation: a ConditionVariable implementation casts
//    Os::Mutex::getHandle() to its own MutexHandle type (e.g.
//    Os/Posix/ConditionVariable.cpp reinterpret_casts it to PosixMutexHandle), so
//    a compile-time Mutex paired with a different ConditionVariable implementation
//    is undefined behavior with no build-time error.
//
// Example compile-time selection override:
//
//     namespace Va416x0Os { namespace AtomicMutex { class AtomicMutex; class AtomicConditionVariable; } }
//     namespace Os {
//     using Mutex = Va416x0Os::AtomicMutex::AtomicMutex;
//     using ConditionVariable = Va416x0Os::AtomicMutex::AtomicConditionVariable;
//     }  // namespace Os
//     #define OS_MUTEX_HEADER "Va416x0/Os/AtomicMutex/AtomicMutex.hpp"
//     #define OS_CONDITION_VARIABLE_HEADER "Va416x0/Os/AtomicMutex/AtomicConditionVariable.hpp"
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
