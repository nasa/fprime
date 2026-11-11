// ======================================================================
// \title Os/Directory.hpp
// \brief public Os::Directory interface and alias
//
// This header aggregates all definitions needed to use Os::Directory:
// the interface, the configured alias, and the concrete delegate type.
//
// WARNING — include order is load-bearing. Do not reorder.
//
// The dependency constraints are:
//
//   1. config/OsSelection.hpp (CFG) defines the Os::Directory type alias by
//      forward-declaring a link-time delegate (e.g. DelegateDirectory) or directly
//      aliasing a concrete implementation.
//      Must not include Os OSAL headers (they aren't yet defined).
//      Included by Os/DirectoryInterface.hpp, which this header includes first.
//
//   2. Os/DirectoryInterface.hpp (IF) includes CFG first (so the Os::Directory alias is
//      available), then defines DirectoryHandle and DirectoryInterface.
//
//   3. OS_DIRECTORY_HEADER (IMPL) is defined by CFG and points to the concrete
//      implementation header. If using delegation, this points to
//      Os/DelegateDirectory.hpp. If using compile-time selection, it points
//      directly to a platform-specific implementation.
//
// DirectoryInterface.hpp must precede OS_DIRECTORY_HEADER here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef Os_Directory_hpp
#define Os_Directory_hpp

#include "Os/DirectoryInterface.hpp"

// A project config/OsSelection.hpp that predates the Directory selection defines neither the Os::Directory
// alias nor OS_DIRECTORY_HEADER; fall back to the link-time delegate so such projects keep building.
#ifndef OS_DIRECTORY_HEADER
#include "Os/DelegateDirectory.hpp"
namespace Os {
using Directory = DelegateDirectory;
}
#else
#include OS_DIRECTORY_HEADER
#endif

#endif  // Os_Directory_hpp
