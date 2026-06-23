// ======================================================================
// \title Os/FilePathUtils.hpp
// \brief Utilities for resolving and validating file paths
// ======================================================================
#ifndef Os_FilePathUtils_hpp_
#define Os_FilePathUtils_hpp_

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/StringBase.hpp>
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
    TOO_LONG,         //!< Combined path length exceeds the output buffer size
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
//! \param baseDir: base directory for resolving relative paths (must be absolute, null-terminated;
//!                 ignored if path is absolute)
//! \param resolvedOut: output buffer for the resolved path
//! \param resolvedSize: size of the output buffer
//! \return VALID on success, INVALID_PATH if the path cannot be resolved, TOO_LONG if the buffer
//!         is too small
//!
Status resolvePath(const char* path, const char* baseDir, char* resolvedOut, FwSizeType resolvedSize);

//! \brief Resolve a path (Fw::StringBase overload)
//!
//! \param path: input path string
//! \param baseDir: base directory for relative paths
//! \param resolvedOut: output string (resized to hold result)
//! \return VALID on success, INVALID_PATH or TOO_LONG on failure
//!
Status resolvePath(const Fw::ConstStringBase& path, const Fw::ConstStringBase& baseDir, Fw::StringBase& resolvedOut);

//! \brief Check whether a path is within an allowed directory
//!
//! Resolves both the path and the allowed directory (collapsing `.` and `..`),
//! then verifies that the resolved path begins with the normalized allowed
//! directory as a proper directory prefix (i.e. the match must end at a `/`
//! boundary to prevent `/allowed_dir_extra/` from matching `/allowed_dir/`).
//!
//! Relative paths are resolved against the process's current working directory
//! (obtained via Os::FileSystem::getWorkingDirectory). If the CWD cannot be
//! determined, relative paths are rejected with INVALID_PATH.
//!
//! \param path: the input path to check (may be relative or absolute)
//! \param allowedDirectory: the allowed directory prefix (must be absolute, must end with `/`)
//! \return VALID if path is within the allowed directory, OUTSIDE_SANDBOX or INVALID_PATH otherwise
//!
Status isSubDirectory(const char* path, const char* allowedDirectory);

//! \brief Check whether an already-resolved path is within an allowed directory
//!
//! Unlike isSubDirectory, this performs no path resolution — both arguments must
//! already be canonical absolute paths. Use this when you have already called
//! resolvePath and want to avoid redundant work.
//!
//! \param resolvedPath: canonical absolute path (output of resolvePath)
//! \param allowedDirectory: canonical allowed directory prefix (must end with `/`)
//! \return VALID if contained, OUTSIDE_SANDBOX or INVALID_PATH otherwise
//!
Status checkContainment(const char* resolvedPath, const char* allowedDirectory);

//! \brief Check whether a path is within an allowed directory (Fw::StringBase overload)
//!
//! \param path: input path string
//! \param allowedDirectory: allowed directory string (must end with `/`)
//! \return VALID if path is within the allowed directory, OUTSIDE_SANDBOX or INVALID_PATH otherwise
//!
Status isSubDirectory(const Fw::ConstStringBase& path, const Fw::ConstStringBase& allowedDirectory);

}  // namespace FilePathUtils
}  // namespace Os

#endif
