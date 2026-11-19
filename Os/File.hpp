// ======================================================================
// \title Os/File.hpp
// \brief public Os::File interface and alias
//
// This header aggregates all definitions needed to use Os::File:
// the interface, the configured alias, and the concrete delegate type.
//
// WARNING — include order is load-bearing. Do not reorder.
//
// The dependency constraints are:
//
//   1. config/OsSelection.hpp (CFG) defines the Os::File type alias by
//      forward-declaring a link-time delegate (e.g. DelegateFile) or directly
//      aliasing a concrete implementation (e.g. Va416x0Os::SomeFile).
//      Must not include Os OSAL headers (they aren't yet defined).
//      Included by Os/FileInterface.hpp, which this header includes first.
//
//   2. Os/FileInterface.hpp (IF) includes CFG first (so the Os::File alias is
//      available), then defines FileHandle and FileInterface.
//
//   3. OS_FILE_HEADER (IMPL) is defined by CFG and points to the concrete
//      implementation header. If using delegation, this points to
//      Os/DelegateFile.hpp. If using compile-time selection, it points
//      directly to a platform-specific implementation.
//
// FileInterface.hpp must precede OS_FILE_HEADER here,
// and CFG must never include either of them (that would form a cycle).
// ======================================================================
#ifndef Os_File_hpp_
#define Os_File_hpp_

#include "Os/FileInterface.hpp"

// A project config/OsSelection.hpp that predates the File selection defines neither the Os::File
// alias nor OS_FILE_HEADER; fall back to the link-time delegate so such projects keep building.
#ifndef OS_FILE_HEADER
#include "Os/DelegateFile.hpp"
namespace Os {
using File = DelegateFile;
}
#else
#include OS_FILE_HEADER
#endif

#endif  // Os_File_hpp_
