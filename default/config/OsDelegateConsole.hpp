// ======================================================================
// \title config/OsDelegateConsole.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::Console resolves to a concrete implementation.
// Two mechanisms are available:
//
// 1. Link-time selection (default): Os::Console aliases to Os::DelegateConsole,
//    which wraps a ConsoleInterface reference. At construction, DelegateConsole
//    calls ConsoleInterface::getDelegate() to construct the platform-specific
//    implementation (e.g., PosixConsole, StubConsole) via placement-new.
//    The linker selects which getDelegate() based on which DefaultConsole.cpp
//    is linked. Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::Console directly to a concrete implementation
//    (e.g., Va416x0Os::SeggerConsole). This eliminates wrapper and virtual
//    dispatch, enabling inlining and aggressive LTO optimization.
//
// Example compile-time selection override:
//
//     namespace Va416x0Os { class SeggerConsole; }
//     namespace Os { using Console = Va416x0Os::SeggerConsole; }
//     #define OS_CONSOLE_HEADER "Va416x0/Os/SeggerConsole/SeggerConsole.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::Console alias.
//   - OS_CONSOLE_HEADER should point to the implementation header, which will
//     be included AFTER ConsoleInterface.hpp in Os/Console.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATECONSOLE_HPP
#define CONFIG_OS_DELEGATECONSOLE_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the Console implementation at link time
namespace Os {

class DelegateConsole;
using Console = DelegateConsole;

}  // namespace Os

#define OS_CONSOLE_HEADER <Os/DelegateConsole.hpp>

#endif  // CONFIG_OS_DELEGATECONSOLE_HPP
