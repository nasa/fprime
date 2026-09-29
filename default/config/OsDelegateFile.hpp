// ======================================================================
// \title config/OsDelegateFile.hpp
// \brief configured selection of Os OSAL implementations
//
// This header configures how Os::File resolves to a concrete implementation.
// Two mechanisms are available:
//
// 1. Link-time selection (default): Os::File aliases to Os::DelegateFile,
//    which wraps a FileInterface reference. At construction, DelegateFile
//    calls FileInterface::getDelegate() to construct the platform-specific
//    implementation (e.g., PosixFile, StubFile) via placement-new.
//    The linker selects which getDelegate() based on which DefaultFile.cpp
//    is linked. Calls dispatch through the vtable at runtime.
//
// 2. Compile-time selection (performance optimization): Platforms may override
//    this header to alias Os::File directly to a concrete implementation
//    (e.g., Va416x0Os::SomeFile). This eliminates wrapper and virtual
//    dispatch, enabling inlining and aggressive LTO optimization.
//
// Example compile-time selection override:
//
//     namespace Va416x0Os { class SomeFile; }
//     namespace Os { using File = Va416x0Os::SomeFile; }
//     #define OS_FILE_HEADER "Va416x0/Os/SomeFile/SomeFile.hpp"
//
// IMPORTANT: CIRCULAR DEPENDENCY PREVENTION
//
//   - This header MUST NOT include any Os OSAL headers (Os/*.hpp).
//   - Only forward-declare types and define the Os::File alias.
//   - OS_FILE_HEADER should point to the implementation header, which will
//     be included AFTER FileInterface.hpp in Os/File.hpp.
//   - Violating this constraint will create circular dependencies.
// ======================================================================
#ifndef CONFIG_OS_DELEGATEFILE_HPP
#define CONFIG_OS_DELEGATEFILE_HPP

//!< Forward declaration of the link-time delegate
//!< Default: select the File implementation at link time
namespace Os {

class DelegateFile;
using File = DelegateFile;

}  // namespace Os

#define OS_FILE_HEADER <Os/DelegateFile.hpp>

#endif  // CONFIG_OS_DELEGATEFILE_HPP
