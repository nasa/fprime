// ======================================================================
// \title config/OsDelegateCpu.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::Cpu resolves to a concrete implementation.
// Two mechanisms are available:
//
// 1. Link-time selection (default): Os::Cpu aliases to Os::DelegateCpu,
//    which wraps a CpuInterface reference. At construction, DelegateCpu
//    calls CpuInterface::getDelegate() to construct the platform-specific
//    implementation (e.g., LinuxCpu, StubCpu) via placement-new.
//    The linker selects which getDelegate() based on which DefaultCpu.cpp
//    is linked. Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::Cpu directly to a concrete implementation.
//    This eliminates wrapper and virtual dispatch, enabling inlining and
//    aggressive LTO optimization.
//
// Example compile-time selection override:
//
//     namespace SomePlatform { class PlatformCpu; }
//     namespace Os { using Cpu = SomePlatform::PlatformCpu; }
//     #define OS_CPU_HEADER "SomePlatform/Os/Cpu/PlatformCpu.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::Cpu alias.
//   - OS_CPU_HEADER should point to the implementation header, which will
//     be included AFTER CpuInterface.hpp in Os/Cpu.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATECPU_HPP
#define CONFIG_OS_DELEGATECPU_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the Cpu implementation at link time
namespace Os {

class DelegateCpu;
using Cpu = DelegateCpu;

}  // namespace Os

#define OS_CPU_HEADER <Os/DelegateCpu.hpp>

#endif  // CONFIG_OS_DELEGATECPU_HPP
