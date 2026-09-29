// ======================================================================
// \title config/OsDelegateDirectory.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::Directory resolves to a concrete implementation.
// Two mechanisms are available:
//
// 1. Link-time selection (default): Os::Directory aliases to Os::DelegateDirectory,
//    which wraps a DirectoryInterface reference. At construction, DelegateDirectory
//    calls DirectoryInterface::getDelegate() to construct the platform-specific
//    implementation (e.g., PosixDirectory, StubDirectory) via placement-new.
//    The linker selects which getDelegate() based on which DefaultDirectory.cpp
//    is linked. Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::Directory directly to a concrete implementation.
//    This eliminates wrapper and virtual dispatch, enabling inlining and
//    aggressive LTO optimization.
//
// Example compile-time selection override:
//
//     namespace SomePlatform { class PlatformDirectory; }
//     namespace Os { using Directory = SomePlatform::PlatformDirectory; }
//     #define OS_DIRECTORY_HEADER "SomePlatform/Os/Directory/PlatformDirectory.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::Directory alias.
//   - OS_DIRECTORY_HEADER should point to the implementation header, which will
//     be included AFTER DirectoryInterface.hpp in Os/Directory.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATEDIRECTORY_HPP
#define CONFIG_OS_DELEGATEDIRECTORY_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the Directory implementation at link time
namespace Os {

class DelegateDirectory;
using Directory = DelegateDirectory;

}  // namespace Os

#define OS_DIRECTORY_HEADER <Os/DelegateDirectory.hpp>

#endif  // CONFIG_OS_DELEGATEDIRECTORY_HPP
