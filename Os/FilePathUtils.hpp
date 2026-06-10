// ======================================================================
// \title Os/FilePathUtils.hpp
// \brief Utilities for resolving and validating file paths
// ======================================================================
#ifndef Os_FilePathUtils_hpp_
#define Os_FilePathUtils_hpp_

#include <Fw/FPrimeBasicTypes.hpp>
#include <config/FppConstantsAc.hpp>

namespace Os {

//! \brief Utility for canonicalizing file paths and checking directory containment
//!
//! Provides standalone functions for resolving relative paths (collapsing `.` and `..` segments)
//! and verifying that a resolved path falls within an allowed directory prefix.
//! These utilities can be used independently of SandboxedFile for any path-validation need.
//!
namespace FilePathUtils {

enum Status {
    VALID,            //!< Path is valid and within the allowed directory
    OUTSIDE_SANDBOX,  //!< Resolved path falls outside the allowed directory
    INVALID_PATH,     //!< Path is malformed or cannot be resolved
};

//! Maximum supported path length for resolution buffers
static constexpr FwSizeType MAX_PATH_LENGTH = FileNameStringSize;

//! \brief Resolve a path by collapsing `.` and `..` segments
//!
//! Performs purely textual path resolution without filesystem access.
//! Handles `./`, `../`, and redundant `/` separators.
//! The result is always an absolute path; relative inputs are resolved
//! against the provided base directory.
//!
//! \param path: input path to resolve (may be relative or absolute)
//! \param baseDir: base directory for resolving relative paths (must be absolute, null-terminated)
//! \param resolvedOut: output buffer for the resolved path
//! \param resolvedSize: size of the output buffer
//! \return VALID on success, INVALID_PATH if the path cannot be resolved or the buffer is too small
//!
Status resolvePath(const char* path, const char* baseDir, char* resolvedOut, FwSizeType resolvedSize);

//! \brief Check whether a path is within an allowed directory
//!
//! Resolves the path (collapsing `.` and `..`), then verifies
//! that the resolved path begins with `allowedDirectory` as a proper
//! directory prefix (i.e. the match must end at a `/` boundary to prevent
//! `/allowed_dir_extra/` from matching `/allowed_dir/`).
//!
//! \param path: the input path to check (may be relative or absolute)
//! \param allowedDirectory: the allowed directory prefix (must be absolute, must end with `/`)
//! \param resolvedOut: optional output buffer for the resolved path (may be nullptr)
//! \param resolvedSize: size of the optional output buffer
//! \return VALID if path is within the allowed directory, OUTSIDE_SANDBOX or INVALID_PATH otherwise
//!
Status isSubDirectory(const char* path,
                      const char* allowedDirectory,
                      char* resolvedOut = nullptr,
                      FwSizeType resolvedSize = 0);

}  // namespace FilePathUtils
}  // namespace Os

#endif
