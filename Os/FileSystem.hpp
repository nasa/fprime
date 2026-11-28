// ======================================================================
// \title Os/FileSystem.hpp
// \brief public Os::FileSystem interface and alias
//
// This header aggregates all definitions needed to use Os::FileSystem:
// the interface, the configured alias, and the concrete delegate type.
//
// WARNING — include order is load-bearing. Do not reorder.
//
// The dependency constraints are:
//
//   1. config/OsSelection.hpp (CFG) defines the Os::FileSystem type
//      alias by forward-declaring a link-time delegate (e.g. DelegateFileSystem)
//      or directly aliasing a concrete implementation.
//      Must not include Os OSAL headers (they aren't yet defined).
//      Included by Os/FileSystemInterface.hpp, which this header includes first.
//
//   2. Os/FileSystemInterface.hpp (IF) includes CFG first (so the
//      Os::FileSystem alias is available), then defines FileSystemHandle
//      and FileSystemInterface.
//
//   3. OS_FILE_SYSTEM_HEADER (IMPL) is defined by CFG and points to the
//      concrete implementation header. If using delegation, this points to
//      Os/DelegateFileSystem.hpp. If using compile-time selection, it points
//      directly to a platform-specific implementation.
//
// FileSystemInterface.hpp must precede OS_FILE_SYSTEM_HEADER here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================

#ifndef _OS_FILESYSTEM_HPP_
#define _OS_FILESYSTEM_HPP_

#include "Os/Directory.hpp"
#include "Os/File.hpp"
#include "Os/FileSystemInterface.hpp"

// A project config/OsSelection.hpp that predates the FileSystem selection defines neither the Os::FileSystem
// alias nor OS_FILE_SYSTEM_HEADER; fall back to the link-time delegate so such projects keep building.
#ifndef OS_FILE_SYSTEM_HEADER
#include "Os/DelegateFileSystem.hpp"
namespace Os {
using FileSystem = DelegateFileSystem;
}
#else
#include OS_FILE_SYSTEM_HEADER
#endif

#endif
