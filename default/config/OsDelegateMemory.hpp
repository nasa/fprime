// ======================================================================
// \title config/OsDelegateMemory.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::Memory resolves to a concrete implementation.
// Two mechanisms are available:
//
// 1. Link-time selection (default): Os::Memory aliases to Os::DelegateMemory,
//    which wraps a MemoryInterface reference. At construction, DelegateMemory
//    calls MemoryInterface::getDelegate() to construct the platform-specific
//    implementation (e.g., LinuxMemory, StubMemory) via placement-new.
//    The linker selects which getDelegate() based on which DefaultMemory.cpp
//    is linked. Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::Memory directly to a concrete implementation.
//    This eliminates wrapper and virtual dispatch, enabling inlining and
//    aggressive LTO optimization.
//
// Example compile-time selection override:
//
//     namespace SomePlatform { class PlatformMemory; }
//     namespace Os { using Memory = SomePlatform::PlatformMemory; }
//     #define OS_MEMORY_HEADER "SomePlatform/Os/Memory/PlatformMemory.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::Memory alias.
//   - OS_MEMORY_HEADER should point to the implementation header, which will
//     be included AFTER MemoryInterface.hpp in Os/Memory.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATEMEMORY_HPP
#define CONFIG_OS_DELEGATEMEMORY_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the Memory implementation at link time
namespace Os {

class DelegateMemory;
using Memory = DelegateMemory;

}  // namespace Os

#define OS_MEMORY_HEADER <Os/DelegateMemory.hpp>

#endif  // CONFIG_OS_DELEGATEMEMORY_HPP
