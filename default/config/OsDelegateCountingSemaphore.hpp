// ======================================================================
// \title config/OsDelegateCountingSemaphore.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::CountingSemaphore resolves to a concrete
// implementation. Two mechanisms are available:
//
// 1. Link-time selection (default): Os::CountingSemaphore aliases to
//    Os::DelegateCountingSemaphore, which wraps a CountingSemaphoreInterface
//    reference. At construction, DelegateCountingSemaphore calls
//    CountingSemaphoreInterface::getDelegate() to construct the
//    platform-specific implementation (e.g., PosixCountingSemaphore,
//    StubCountingSemaphore) via placement-new. The linker selects which
//    getDelegate() based on which DefaultCountingSemaphore.cpp is linked.
//    Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may
//    override this header to alias Os::CountingSemaphore directly to a
//    concrete implementation. This eliminates wrapper and virtual dispatch,
//    enabling inlining and aggressive LTO optimization.
//
// Example compile-time selection override:
//
//     namespace SomeOs { class ConcreteCountingSemaphore; }
//     namespace Os { using CountingSemaphore = SomeOs::ConcreteCountingSemaphore; }
//     #define OS_COUNTING_SEMAPHORE_HEADER "SomeOs/ConcreteCountingSemaphore.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::CountingSemaphore alias.
//   - OS_COUNTING_SEMAPHORE_HEADER should point to the implementation
//     header, which will be included AFTER CountingSemaphoreInterface.hpp
//     in Os/CountingSemaphore.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATECOUNTINGSEMAPHORE_HPP
#define CONFIG_OS_DELEGATECOUNTINGSEMAPHORE_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the CountingSemaphore implementation at link time
namespace Os {

class DelegateCountingSemaphore;
using CountingSemaphore = DelegateCountingSemaphore;

}  // namespace Os

#define OS_COUNTING_SEMAPHORE_HEADER <Os/DelegateCountingSemaphore.hpp>

#endif  // CONFIG_OS_DELEGATECOUNTINGSEMAPHORE_HPP
