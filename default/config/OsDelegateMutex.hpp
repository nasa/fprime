// ======================================================================
// \title config/OsDelegateMutex.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::Mutex resolves to a concrete implementation.
// Two mechanisms are available:
//
// 1. Link-time selection (default): Os::Mutex aliases to Os::DelegateMutex,
//    which wraps a MutexInterface reference. At construction, DelegateMutex
//    calls MutexInterface::getDelegate() to construct the platform-specific
//    implementation (e.g., PosixMutex, StubMutex) via placement-new.
//    The linker selects which getDelegate() based on which DefaultMutex.cpp
//    is linked. Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::Mutex directly to a concrete implementation
//    (e.g., Va416x0Os::MaskingMutex). This eliminates wrapper and virtual
//    dispatch, enabling inlining and aggressive LTO optimization.
//
// Example compile-time selection override:
//
//     namespace Va416x0Os { namespace MaskingMutex { class MaskingMutex; } }
//     namespace Os { using Mutex = Va416x0Os::MaskingMutex::MaskingMutex; }
//     #define OS_MUTEX_HEADER "Va416x0/Os/MaskingMutex/MaskingMutex.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::Mutex alias.
//   - OS_MUTEX_HEADER should point to the implementation header, which will
//     be included AFTER MutexInterface.hpp in Os/Mutex.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATEMUTEX_HPP
#define CONFIG_OS_DELEGATEMUTEX_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the Mutex implementation at link time
namespace Os {

class DelegateMutex;
using Mutex = DelegateMutex;

}  // namespace Os

#define OS_MUTEX_HEADER <Os/DelegateMutex.hpp>

#endif  // CONFIG_OS_DELEGATEMUTEX_HPP
