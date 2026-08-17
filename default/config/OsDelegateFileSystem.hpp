// ======================================================================
// \title config/OsDelegateFileSystem.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::FileSystem resolves to a concrete implementation.
// Two mechanisms are available:
//
// 1. Link-time selection (default): Os::FileSystem aliases to
//    Os::DelegateFileSystem, which wraps a FileSystemInterface reference. At
//    construction, DelegateFileSystem calls FileSystemInterface::getDelegate()
//    to construct the platform-specific implementation (e.g., PosixFileSystem,
//    StubFileSystem) via placement-new. The linker selects which getDelegate()
//    based on which DefaultFile.cpp is linked. Calls dispatch through the
//    vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::FileSystem directly to a concrete implementation.
//    This eliminates wrapper and virtual dispatch, enabling inlining and
//    aggressive LTO optimization.
//
// Example compile-time selection override:
//
//     namespace SomeOs { class ConcreteFileSystem; }
//     namespace Os { using FileSystem = SomeOs::ConcreteFileSystem; }
//     #define OS_FILE_SYSTEM_HEADER "SomeOs/ConcreteFileSystem.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::FileSystem alias.
//   - OS_FILE_SYSTEM_HEADER should point to the implementation header, which
//     will be included AFTER FileSystemInterface.hpp in Os/FileSystem.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATEFILESYSTEM_HPP
#define CONFIG_OS_DELEGATEFILESYSTEM_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the FileSystem implementation at link time
namespace Os {

class DelegateFileSystem;
using FileSystem = DelegateFileSystem;

}  // namespace Os

#define OS_FILE_SYSTEM_HEADER <Os/DelegateFileSystem.hpp>

#endif  // CONFIG_OS_DELEGATEFILESYSTEM_HPP
