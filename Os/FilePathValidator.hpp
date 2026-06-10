// ======================================================================
// \title Os/FilePathValidator.hpp
// \brief Utilities for validating and resolving file paths
// ======================================================================
#ifndef Os_FilePathValidator_hpp_
#define Os_FilePathValidator_hpp_

#include <Fw/FPrimeBasicTypes.hpp>

namespace Os {

//! \brief Utility for canonicalizing file paths and checking directory containment
//!
//! Provides standalone functions for resolving relative paths (collapsing `.` and `..` segments)
//! and verifying that a resolved path falls within an allowed directory prefix.
//! These utilities can be used independently of SandboxedFile for any path-validation need.
//!
namespace FilePathValidator {

enum Status {
    VALID,              //!< Path is valid and within the allowed directory
    OUTSIDE_SANDBOX,    //!< Resolved path falls outside the allowed directory
    INVALID_PATH,       //!< Path is malformed or cannot be resolved
};

//! Maximum supported path length for resolution buffers
static constexpr FwSizeType MAX_PATH_LENGTH = 256;

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
Status resolvePath(const char* path,
                   const char* baseDir,
                   char* resolvedOut,
                   FwSizeType resolvedSize);

//! \brief Check whether a resolved path is within an allowed directory
//!
//! Verifies that `resolvedPath` begins with `allowedDirectory` as a proper
//! directory prefix (i.e. the match must end at a `/` boundary to prevent
//! `/allowed_dir_extra/` from matching `/allowed_dir/`).
//!
//! Both paths should already be resolved (no `.` or `..` segments).
//!
//! \param resolvedPath: the fully resolved path to check
//! \param allowedDirectory: the allowed directory prefix (must end with `/`)
//! \return VALID if path is within the allowed directory, OUTSIDE_SANDBOX otherwise
//!
Status checkContainment(const char* resolvedPath,
                        const char* allowedDirectory);

//! \brief Validate that a path falls within an allowed directory
//!
//! Convenience function combining resolvePath and checkContainment.
//! Resolves the path, then checks containment against the allowed directory.
//!
//! \param path: input path to validate (may be relative or absolute)
//! \param allowedDirectory: the allowed directory prefix (must be absolute, must end with `/`)
//! \return VALID if path is within the allowed directory, OUTSIDE_SANDBOX or INVALID_PATH otherwise
//!
Status validatePath(const char* path,
                    const char* allowedDirectory);

}  // namespace FilePathValidator
}  // namespace Os

#endif
